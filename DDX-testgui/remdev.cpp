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
	// TODO:  I don't believe req_id_lock needs to be recursive anymore?
	// Initializations
	connectTime = QDateTime::currentMSecsSinceEpoch();
	this->dm = dm;
	lastId = 0;
	registered = false;
	regState = UnregisteredState;
	closed = false;
	this->inbound = inbound;
	// Add to master device list and get temporary cid
	cid = dm->addDevice(this);
	log(tr("New unregistered device"));
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
	delete timeoutPoller;
}

int RemDev::sendRequest(QObject *self, const char *handler, const char *method, rapidjson::Document *doc,
						rapidjson::Value *params, qint64 timeout) {
	LocalId id;
	RequestRef ref(self, handler, timeout);
	req_id_lock.lock();
	// Check that the connection is still open while we've got the lock
	if (closed) {
		req_id_lock.unlock();
		if (doc) delete doc;
		return -1;
	}
	// Obtain a server-unique ID
	do {
		id = ++lastId;
		if (id == std::numeric_limits<LocalId>::max()) {
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
	// Start timeout timer if required
	if (timeout && ! timeoutPoller->isActive())
		timeoutPoller->start();
	printReqs();
	return id;
}

void RemDev::sendResponse(rapidjson::Value &id, rapidjson::Document *doc, rapidjson::Value *result) {
	if (closed) {
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

void RemDev::sendError(rapidjson::Value *id, int code, const QString &msg, rapidjson::Document *doc, rapidjson::Value *data) noexcept {
	if (closed) {
		if (doc) delete doc;
		return;
	}
	if ( ! doc) doc = new Document;
	rapidjson::MemoryPoolAllocator<> &a = doc->GetAllocator();
	Value e(kObjectType);
	{
		Value v(code);
		e.AddMember("code", v, a);
		QByteArray encodedMsg = msg.toUtf8();
		v.SetString(encodedMsg.constData(), encodedMsg.size(), a);
		e.AddMember("message", v, a);
	}
	if (data) e.AddMember("data", *data, a);
	prepareDocument(doc, a);
	doc->AddMember("error", e, a);
	if (id) doc->AddMember("id", *id, a);
	else doc->AddMember("id", Value(rapidjson::kNullType), a);  // Add null if no id present
	sendDocument(doc);
}

void RemDev::sendNotification(const char *method, rapidjson::Document *doc, rapidjson::Value *params) noexcept {
	if (closed) {
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

void RemDev::close(DisconnectReason reason, bool fromRemote) noexcept {
	emit deviceDisconnected(this, reason, fromRemote);
	req_id_lock.lock();
	closed = true;
	if (reqs.size()) {
		// TODO
		/*QJsonObject error({{"code", E_DEVICE_DISCONNECTED},
						   {"message", tr("Device disconnected")},
						   {"data", reason}});
		RequestHash::const_iterator it;
		for (it = reqs.constBegin(); it != reqs.constEnd(); ++it)
			(*it->handler)(it.key(), error, false);*/
	}
	req_id_lock.unlock();
	terminate(reason, fromRemote);
	log(tr("Connection closed"));
	dm->removeDevice(this);
	deleteLater();
}

void RemDev::timeoutPoll() noexcept {
/*	qint64 time = QDateTime::currentMSecsSinceEpoch();
	QJsonObject error({{"code", E_REQUEST_TIMEOUT},
					   {"message", tr("Request timed out")}});
	QMutexLocker l(&req_id_lock);
	RequestHash::iterator it = reqs.begin();
	while (it != reqs.end()) {
	// TODO: Use valid()
		if (it->timeout_time && it->timeout_time < time) {
			(*it->handler)(it.key(), error, false);
			it = reqs.erase(it);
		}
		else ++it;
	}
	l.unlock();
	if ( ! registered && registrationTimeoutTime < time) {
		// TODO: Disconnect for registration timeout
	}*/
}

void RemDev::init() noexcept {
/*	int registrationPeriod = sg->v("RegistrationPeriod", SG_RPC).toInt();
	if (registrationPeriod < 1) {
		lg->log(tr("User attempted to set a registration period less than 1 second; defaulting"));
		registrationPeriod = sg->reset("RegistrationPeriod", SG_RPC).toInt();
	}
	registrationTimeoutTime = connectTime + (registrationPeriod * 1000);*/
	// Set up timeout polling
	timeoutPoller = new QTimer(this);
	timeoutPoller->setTimerType(Qt::CoarseTimer);
	timeoutPoller->setInterval(TIMEOUT_POLL_INTERVAL);
	connect(timeoutPoller, &QTimer::timeout, this, &RemDev::timeoutPoll);
	// Call the subclass init function
	sub_init();
	//timeoutPoller->start();  // Start immediately for registration timeout*/
}

void RemDev::handleItem(char *data) noexcept {
	// Parse document
	Document *doc = new Document;
	registered = true;  // TODO:  Remove this
	if (registered) {  // Parsing procedure is more lenient with registered connections
		doc->ParseInsitu(data);
		if (doc->HasParseError()) {
			delete data;
			// TODO
			// Does this leave doc hanging?  We're overwriting a previously parsed doc...
			// It might be a memory leak.  Also of note is that we delete the data in which
			// this document was parsed before we overwrite the doc.
			sendError(0, -32700, tr("Parse error"), doc);
			return;
		}
	}
	else {  // Unregistered: strict parsing, return if errors
		doc->ParseInsitu<rapidjson::kParseValidateEncodingFlag |
						rapidjson::kParseIterativeFlag>
				(data);
		// Assume this is a fake connection; return nothing if there's a parse error
		if (doc->HasParseError()) {
			delete data;
			delete doc;
			return;
		}
		handleRegistration(doc);
		delete data;
		delete doc;
		return;
	}
	if (doc->HasMember("method")) {
		handleRequest_Notif(doc, data);
		return;
	}
	if (doc->HasMember("id")) {
		handleResponse(doc, data);
		return;
	}
	/*
	// Check for registration before handling parsing errors because it will just
	// return if any errors occurred (we don't want to send anything to prevent
	// overload attacks if we're unregistered)
	if ( ! registered) {
		if ( ! doc.isObject()) return;
		handleRegistration(doc.object());
		delete data;
		return;
	}
	if (error.error != QJsonParseError::NoError) {
		
		delete data;
		return;
	}
	if (doc.isArray()) {
		// TODO
		delete data;
		return;
	}
	if (doc.isObject()) {
		QJsonObject obj = doc.object();
		handleObject(obj);
		delete data;
		return;
	}*/
	delete data;
	delete doc;
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

void RemDev::handleRequest_Notif(rapidjson::Document *doc, char *buffer) {
	// TODO
	log(tr("Unhandled request or notification"));
}

void RemDev::handleResponse(rapidjson::Document *doc, char *buffer) {
	// Find and type-check all elements efficiently
	Value *idVal = 0, *mainVal = 0;
	bool error = false, foundRpc = false, wasSuccessful;
	// Scan the document for all possible members
	for (Value::ConstMemberIterator it = doc->MemberBegin(); it != doc->MemberEnd(); ++it) {
		const char *name = it->name.GetString();
		// TODO:  These pointers might be invalidated on every loop
		Value &value = (Value &) it->value;
		if (strcmp("jsonrpc", name) == 0) continue;
		if (strcmp("id", name) == 0) {
			if ( ! value.IsInt() || idVal) {
				error = true;
				break;
			}
			idVal = &value;
			continue;
		}
		if ( ! mainVal) {
			if (strcmp("result", name) == 0) {
				// Result does not require type checking
				mainVal = &value;
				wasSuccessful = true;
				continue;
			}
			if (strcmp("error", name) == 0) {
				if ( ! value.IsObject()) {
					error = true;
					break;
				}
				// TODO:  Check that error item has the correct elements
				mainVal = &value;
				wasSuccessful = false;
				continue;
			}
		}
		error = true;  // There was an unknown (or, theoretically, duplicate) member
		break;
	}
	// TODO:  Handle null errors!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	if ( ! error) {
		int id = idVal->GetInt();
		req_id_lock.lock();
		RequestRef &&req = reqs.take(id);  // Will be invalid if the id does not exist
		req_id_lock.unlock();
		if (req.valid(0)) {  // Proceed to sending error if otherwise
			Response *res = new Response(wasSuccessful, id, doc, buffer, mainVal);
			metaObject()->invokeMethod(req.handlerObj, req.handlerFn,
									   Qt::QueuedConnection, Q_ARG(Response *, res));
			return;
		}
	}
	delete buffer;
	// TODO
	// Does this leave doc hanging?  We're overwriting a previously parsed doc...
	// It might be a memory leak.  Also of note is that we delete the data in which
	// this document was parsed before we overwrite the doc.
	sendError(0, E_INVALID_RESPONSE, tr("Invalid response"), doc);
}

void RemDev::handleRegistration(const rapidjson::Document *doc) {
	// If this is not a register request or response, return without error
	if ((regState & RegSentFlag) && doc->HasMember("result")) {  // This is a response to our registration
		
	}
	/*if (inbound && QString::compare(doc.value("method").toString(), "register")) return;
	if ( ! inbound && ! doc.contains("result")) return;
	QJsonValue id = doc.value("id");
	// Check minimum version
	QString sent = doc.value("DDX_version").toString();
	QString check = sg->v("MinVersion", SG_RPC).toString();
	if (QString::compare(check, "any")) {  // If check is required...
		int vc = Daemon::versionCompare(sent, check);
		if (vc == VERSION_COMPARE_FAILED) {
			if (inbound) sendError(id, E_VERSION_UNREADABLE, tr("Version unreadable"));
			return;
		}
		if (vc < 0) {
			// Send error
		}
	}
	// UPDATE:  I think both devices should independently send register requests to each other.  Why not, right?
	// It would make a lot of this MUCH cleaner...
	// A successful result response and a register request should both be required
	
	
	//log(tr("Now known as %1").arg(
	registered = true;
	d->registerDevice(this);*/
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

QByteArray RemDev::serializeValue(rapidjson::Value &v) const {
	Document doc;
	doc.CopyFrom(v, doc.GetAllocator());
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);
	QByteArray array(buffer.GetString());
	Q_ASSERT(buffer.GetSize() == (uint) array.size());
	return array;
}
#endif








