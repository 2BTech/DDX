/******************************************************************************
 *                         DATA DISPLAY APPLICATION X                         *
 *                            2B TECHNOLOGIES, INC.                           *
 *                                                                            *
 * The DDX is free software: you can redistribute it and/or modify it under   *
 * the terms of the GNU General Public License as published by the Free       *
 * Software Foundation, either version 3 of the License, or (at your option)  *
 * any later version.  The DDX is distributed in the hope that it will be     *
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty of     *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General  *
 * Public License for more details.  You should have received a copy of the   *
 * GNU General Public License along with the DDX.  If not, see                *
 * <http://www.gnu.org/licenses/>.                                            *
 *                                                                            *
 *  For more information about the DDX, check out the 2B website or GitHub:   *
 *       <http://twobtech.com/DDX>       <https://github.com/2BTech/DDX>      *
 ******************************************************************************/

#include "remdev.h"
#include "devmgr.h"

#define RAPIDJSON_IO
#include "rapidjson_using.h"

RemDev::RemDev(DevMgr *dm, bool inbound) :
		QObject(0), req_id_lock(QMutex::Recursive) {
	// Initializations
	connectTime = QDateTime::currentMSecsSinceEpoch();
	this->dm = dm;
	lastId = 0;
	registered = false;
	state = InitialState;
	open = false;
	cid = QByteArray("UnknownDev").append(dm->getRef());
	this->inbound = inbound;
	if (inbound) log(tr("New unregistered connection"));
	else log(tr("Connecting to remote device"));
	connect(dm, &DevMgr::requestClose, this, &RemDev::close);
	// Threading
#ifdef REMDEV_THREADS
	QThread *t = new QThread(dm);
	moveToThread(t);
	connect(t, &QThread::started, this, &RemDev::init);
	connect(this, &RemDev::destroyed, t, &QThread::quit);
	connect(t, &QThread::finished, t, &QThread::deleteLater);
	t->start();
#else
	init();
#endif
}

RemDev::~RemDev() {
	if (open) close(DevUnknownDisconnect);
}

int RemDev::sendRequest(QObject *self, const char *handler, const char *method,
						rapidjson::Value *params, rapidjson::Document *doc, qint64 timeout) {
	int id;
	RequestRef ref(self, handler, method, timeout);
	req_id_lock.lock();
	// Check that the connection is still open while we've got the lock
	if ( ! open) {
		req_id_lock.unlock();
		if (doc) delete doc;
		return -1;
	}
	// Obtain a server-unique ID
	do {
		id = ++lastId;
		if (id == std::numeric_limits<int>::max()) {
			log("ID generator overflow; resetting");
			lastId = 0;
		}
	} while (reqs.contains(id));
	// Insert into request list
	reqs.insert(id, ref);
	req_id_lock.unlock();
	// Build & send request
	if ( ! doc) doc = new Document;
	rapidjson::MemoryPoolAllocator<> &a = doc->GetAllocator();
	prepareDocument(doc, a);
	doc->AddMember("method", Value().SetString(rapidjson::StringRef(method)), a);
	doc->AddMember("id", Value(id), a);
	if (params) doc->AddMember("params", *params, a);
	sendDocument(doc);
	return id;
}

void RemDev::sendResponse(rapidjson::Value &id, rapidjson::Document *doc, rapidjson::Value *result) {
	if ( ! open) {
		if (doc) delete doc;
		return;
	}
	if ( ! doc) doc = new Document;
	rapidjson::MemoryPoolAllocator<> &a = doc->GetAllocator();
	prepareDocument(doc, a);
	doc->AddMember("id", id, a);
	if (result) doc->AddMember("result", *result, a);
	else doc->AddMember("result", Value(rapidjson::kTrueType), a);
	sendDocument(doc);
}

void RemDev::sendResponse(Request *req, rapidjson::Value *result) {
	if (req->id) {
		sendResponse(*req->id, req->outDoc, result);
		req->outDoc = 0;
	}
	delete req;
}

void RemDev::sendError(rapidjson::Value *id, int code, const QString &msg, rapidjson::Value *data, rapidjson::Document *doc) noexcept {
	if ( ! open) {
		if (doc) delete doc;
		return;
	}
	if ( ! doc) doc = new Document;
	rapidjson::MemoryPoolAllocator<> &a = doc->GetAllocator();
	prepareDocument(doc, a);
	if (id) doc->AddMember("id", *id, a);
	else doc->AddMember("id", Value(rapidjson::kNullType), a);  // Add null if no id present
	// Build error object
	Value e(kObjectType);
	{
		Value v(code);
		e.AddMember("code", v, a);
		QByteArray encodedMsg = msg.toUtf8();
		v.SetString(encodedMsg.constData(), encodedMsg.size(), a);
		e.AddMember("message", v, a);
	}
	if (data) e.AddMember("data", *data, a);
	doc->AddMember("error", e, a);
	sendDocument(doc);
}

void RemDev::sendError(Request *req, int code, const QString &msg, rapidjson::Value *data) noexcept {
	if (req->id) {
		sendError(req->id, code, msg, data, req->outDoc);
		req->outDoc = 0;
	}
	delete req;
}

void RemDev::sendError(Request *req, int code) noexcept {
	QString msg;
	switch (code) {
	case E_JSON_INTERNAL: msg = tr("Internal error"); break;
	case E_ACCESS_DENIED: msg = tr("Access denied"); break;
	case E_NOT_SUPPORTED: msg = tr("Not supported"); break;
	case E_JSON_PARAMS: msg = tr("Invalid params"); break;
	case E_JSON_METHOD: msg = tr("Method not found"); break;
	default: msg = tr("Unknown error"); Q_ASSERT(false);
	}
	sendError(req, code, msg);
}

void RemDev::sendNotification(const char *method, rapidjson::Document *doc, rapidjson::Value *params) noexcept {
	if ( ! open) {
		if (doc) delete doc;
		return;
	}
	if ( ! doc) doc = new Document;
	rapidjson::MemoryPoolAllocator<> &a = doc->GetAllocator();
	prepareDocument(doc, a);
	doc->AddMember("method", Value().SetString(rapidjson::StringRef(method)), a);
	if (params) doc->AddMember("params", *params, a);
	sendDocument(doc);
}

void RemDev::close(int reason, bool fromRemote) noexcept {
	if ( ! registered) dm->markDeviceConnectionFailed(this, tr("Connection error"));
	emit deviceDisconnected(this, reason, fromRemote);
	req_id_lock.lock();
	bool wasOpen = open;
	open = false;
	for (RequestHash::ConstIterator it = reqs.constBegin(); it != reqs.constEnd(); ++it)
		simulateError(it.key(), it.value(), E_DEVICE_DISCONNECTED,
					  tr("Device disconnected"));
	req_id_lock.unlock();
	if (wasOpen && ! fromRemote) {
		Document *params = new Document(kObjectType);
		params->AddMember("reason", Value(reason), params->GetAllocator());
		sendNotification("disconnect", params, params);
	}
	terminate();
	log(tr("Connection closed"));
	deleteLater();
}

void RemDev::timeoutPoll() noexcept {
	qint64 time = QDateTime::currentMSecsSinceEpoch();
	req_id_lock.lock();
	RequestHash::iterator it = reqs.begin();
	while (it != reqs.end()) {
		if ( ! it->valid(time)) {
			simulateError(it.key(), it.value(), E_REQUEST_TIMEOUT,
						  tr("Request timed out"));
			it = reqs.erase(it);
		}
		else ++it;
	}
	req_id_lock.unlock();
	if ( ! registered && registrationTimeoutTime < time) {
		// TODO: Disconnect for registration timeout
	}
}

void RemDev::init() noexcept {
/*	int registrationPeriod = sg->v("RegistrationPeriod", SG_RPC).toInt();
	if (registrationPeriod < 1) {
		lg->log(tr("User attempted to set a registration period less than 1 second; defaulting"));
		registrationPeriod = sg->reset("RegistrationPeriod", SG_RPC).toInt();
	}
	registrationTimeoutTime = connectTime + (registrationPeriod * 1000);*/
	// Call the subclass init function
	sub_init();
}

void RemDev::handleItem(char *data) noexcept {
	// Parse document
	Document *doc = new Document;
	registered = true;  // TODO:  Remove this
	if (registered) {  // Parsing procedure is more lenient with registered connections
		doc->Parse(data);
		free(data);
		if (doc->HasParseError()) {
			sendError(0, E_JSON_PARSE, tr("Parse error"), 0, doc);
			return;
		}
	}
	else {  // Unregistered: strict parsing, return if errors
		doc->Parse<rapidjson::kParseValidateEncodingFlag |
				   rapidjson::kParseIterativeFlag>
				(data);
		free(data);
		// Assume this is a fake connection; return nothing if there's a parse error
		if (doc->HasParseError()) {
			delete doc;
			return;
		}
		handleRegistration(doc);
		return;
	}
	if (doc->IsArray()) {
		sendError(0, E_NO_BATCH, tr("Batch not supported"), 0, doc);
		return;
	}
	if ( ! doc->IsObject()) {
		sendError(0, E_JSON_PARSE, tr("Invalid JSON"), 0, doc);
		return;
	}
	
	// Find members
	Value *methodVal = 0, *idVal = 0, *mainVal = 0;
	int_fast8_t type = ParamsT;  // Assume params until proven otherwise
	for (Value::ConstMemberIterator it = doc->MemberBegin(); it != doc->MemberEnd(); ++it) {
		const char *name = it->name.GetString();
		Value &value = (Value &) it->value;
		if (strcmp("jsonrpc", name) == 0) continue;
		if (strcmp("id", name) == 0) {
			idVal = &value;
			continue;
		}
		if (strcmp("method", name) == 0) {
			methodVal = &value;
			continue;
		}
		if ( ! mainVal) {
			if (strcmp("params", name) == 0) {
				mainVal = &value;
				continue;
			}
			if (strcmp("result", name) == 0) {
				mainVal = &value;
				type = ResultT;
				continue;
			}
			if (strcmp("error", name) == 0) {
				mainVal = &value;
				type = ErrorT;
				continue;
			}
		}
		// There was an unknown member, force error
		methodVal = 0;
		idVal = 0;
		break;
	}
	// Handle a request or notification
	if (methodVal && type == ParamsT) {
		// Type-check method and params
		if (methodVal->IsString() && (mainVal ? mainVal->IsObject() : true)) {
			Request *req = new Request(methodVal->GetString(), mainVal, doc, this, idVal);
			if ( ! dm->dispatchRequest(req))  // Will return false if no method was found
				sendError(req, E_JSON_METHOD, tr("Method not found"), 0);
			return;
		}
	}
	// Handle a response
	if (idVal && mainVal) {
		// "Verify" error objects (see Response docs)
		if (type == ErrorT) {
		    if ( ! mainVal->IsObject())
				type = ParamsT;  // Setting type to ParamsT indicates a bad response at this point
			else {
				Value::ConstMemberIterator eIt = mainVal->FindMember("code");
				if (eIt == mainVal->MemberEnd() || ! eIt->value.IsInt())
					type = ParamsT;
				eIt = mainVal->FindMember("message");
				if (eIt == mainVal->MemberEnd() || ! eIt->value.IsString())
					type = ParamsT;
			}
	    }
		// Handle null-ID errors
		if (idVal->IsNull()) {
			if (type == ErrorT) logError(mainVal);
			delete doc;
			return;
		}
		if (type != ParamsT && idVal->IsInt()) {  // If all types line up...
			int id = idVal->GetInt();
			req_id_lock.lock();
			RequestRef &&req = reqs.take(id);  // Will be invalid if the id does not exist
			req_id_lock.unlock();
			if (req.valid(0)) {  // If the request (still) exists...
				bool successful;
				if (type == ErrorT) {
					successful = false;
					logError(mainVal, req.method);  // Log errors
				}
				else successful = true;
				Response *res = new Response(successful, id, req.method, doc, this, mainVal);
				metaObject()->invokeMethod(req.handlerObj, req.handlerFn,
										   Qt::QueuedConnection, Q_ARG(Response*, res));
				return;
			}
		}
	}
	// If we get here without returning, there was an error
	if (idVal && methodVal)
		sendError(idVal, E_JSON_REQUEST, tr("Invalid request"), 0, doc);
	else if (idVal && mainVal)
		sendError(0, E_INVALID_RESPONSE, tr("Invalid response"), idVal, doc);
	// TODO:  Make sure that this both makes it and is deleted   ^^^^^
	// Update:  I've since verified this, but I'm not sure where it's deleted or what deletes it
	else
		sendError(0, E_JSON_PARSE, tr("Invalid JSON"), 0, doc);
}

void RemDev::connectionReady() noexcept {
	open = true;
	// TODO: send registration
#ifdef QT_DEBUG
	log(tr("Connection ready"));
#endif
}

void RemDev::connectionError(const QString &error) noexcept {
	open = false;
	log(tr("Connection failed: %1").arg(error));
	if ( ! registered) {
		dm->markDeviceConnectionFailed(this, error);
		// Suppress resending of deviceRegistered() signal
		registered = true;
	}
	close(DevStreamClosed, true);
}

void RemDev::log(const QString &msg, bool isAlert) const noexcept {
	(void) isAlert;
	QString out = QString::fromUtf8(cid);
	out.append(": ").append(msg);
	emit postToLogArea(out);
}

void RemDev::sendDocument(rapidjson::Document *doc) {
	StringBuffer *buffer = new StringBuffer;
	Writer<StringBuffer> writer(*buffer);
	doc->Accept(writer);
	delete doc;
	writeItem(buffer);
}

void RemDev::handleRegistration(rapidjson::Document *doc) {
	(void) doc;
	// TODO
}

void RemDev::handleDisconnectNotification(rapidjson::Document *doc) {
	(void) doc;
	// TODO
}

void RemDev::simulateError(int id, const RequestRef &req, int code, const QString &msg) {
	Document *doc = new Document;
	rapidjson::MemoryPoolAllocator<> &a = doc->GetAllocator();
	doc->SetObject();
	{
		Value v(code);
		doc->AddMember("code", v, a);
		QByteArray encodedMsg = msg.toUtf8();
		v.SetString(encodedMsg.constData(), encodedMsg.size(), a);
		doc->AddMember("message", v, a);
	}
	logError(doc, req.method);
	Response *res = new Response(false, id, req.method, doc, this, doc);
	metaObject()->invokeMethod(req.handlerObj, req.handlerFn,
							   Qt::QueuedConnection, Q_ARG(Response*, res));
}

void RemDev::logError(const Value *errorVal, const char *method) const {
	QString logStr;
	if (method)
		logStr = tr("Request to '%1' failed with error %2: %3").arg(QString::fromUtf8(method));
	else
		logStr = tr("Received null error %2: %3");
	const Value &code = errorVal->FindMember("code")->value;
	const Value &message = errorVal->FindMember("message")->value;
	log(logStr.arg(QString::number(code.GetInt()), QString::fromUtf8(message.GetString())));
}

inline void RemDev::prepareDocument(rapidjson::Document *doc, rapidjson::MemoryPoolAllocator<> &a) {
	doc->SetObject();
	doc->AddMember("jsonrpc", "2.0", a);
}

#ifdef QT_DEBUG
void RemDev::printReqs() const {
	Document doc;
	rapidjson::MemoryPoolAllocator<> &a = doc.GetAllocator();
	doc.SetArray();
	req_id_lock.lock();
	for (RequestHash::ConstIterator it = reqs.constBegin(); it != reqs.constEnd(); ++it) {
		Value v(kArrayType);
		v.PushBack(Value(it.key()), a);
		QByteArray time = QDateTime::fromMSecsSinceEpoch(it->timeout_time).toString("HH:mm:ss.zzz").toUtf8();
		Value s;
		s.SetString(time.constData(), a);
		v.PushBack(s, a);
		s.SetString(rapidjson::StringRef(it->handlerFn));
		v.PushBack(s, a);
		if (it->valid(0)) {
			const char *name = it->handlerObj->metaObject()->className();
			s.SetString(rapidjson::StringRef(name));
			v.PushBack(s, a);
			v.PushBack(Value(rapidjson::kTrueType), a);
		}
		else v.PushBack(Value(rapidjson::kFalseType), a);
		doc.PushBack(v, a);
	}
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);
	log(buffer.GetString());
	req_id_lock.unlock();
}
#endif  // QT_DEBUG
