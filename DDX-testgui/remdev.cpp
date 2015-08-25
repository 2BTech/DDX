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
	registerAccepted = false;
	this->inbound = inbound;
	// Add to master device list and get temporary cid
	cid = dm->addDevice(this);
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

/*int RemDev::sendRequest(ResponseHandler handler, const char *method,
						const rapidjson::Value *params, qint64 timeout) {
	if ( ! registered) return 0;
	// Obtain a server-unique ID
	LocalId id;
	RequestRef ref(handler, timeout);
	req_id_lock.lock();
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
	// Start timeout timer if required
	if (timeout && ! timeoutPoller->isActive())
		timeoutPoller->start();
	// Build & send request
	sendObject(newRequest(id, method, params));
	return id;
}

bool RemDev::sendResponse(rapidjson::Value *id, rapidjson::Value *result) {
	if ( ! registered) return false;
	sendObject(newResponse(id, result));
	return true;
}*/

/*bool RemDev::sendNotification(const char *method, rapidjson::Value *params) {
	if ( ! registered) return false;
	sendObject(newNotification(method, params));
	return true;
}
*/
void RemDev::close(DisconnectReason reason, bool fromRemote) noexcept {
/*	registered = false;
	terminate(reason, fromRemote);
	emit deviceDisconnected(this, reason, fromRemote);
	if (reqs.size()) {
		req_id_lock.lock();
		QJsonObject error({{"code", E_DEVICE_DISCONNECTED},
						   {"message", tr("Device disconnected")},
						   {"data", reason}});
		RequestHash::const_iterator it;
		for (it = reqs.constBegin(); it != reqs.constEnd(); ++it)
			(*it->handler)(it.key(), error, false);
	}
	deleteLater();*/
}

void RemDev::timeoutPoll() noexcept {
/*	qint64 time = QDateTime::currentMSecsSinceEpoch();
	QJsonObject error({{"code", E_REQUEST_TIMEOUT},
					   {"message", tr("Request timed out")}});
	QMutexLocker l(&req_id_lock);
	RequestHash::iterator it = reqs.begin();
	while (it != reqs.end()) {
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
	registrationTimeoutTime = connectTime + (registrationPeriod * 1000);
	// Set up timeout polling
	timeoutPoller = new QTimer(this);
	timeoutPoller->setTimerType(Qt::CoarseTimer);
	timeoutPoller->setInterval(TIMEOUT_POLL_INTERVAL);
	connect(timeoutPoller, &QTimer::timeout, this, &RemDev::timeoutPoll);
	// Call the subclass init function
	sub_init();
	timeoutPoller->start();  // Start immediately for registration timeout*/
}

void RemDev::handleItem(char *data) noexcept {
	// Parse document
	Document doc;
	if (registered) {  // Parsing procedure is more lenient with registered connections
		doc.ParseInsitu(data);
		if (doc.HasParseError()) {
			
		}
	}
	else {  // Unregistered: strict parsing, return if errors
		doc.ParseInsitu<rapidjson::kParseValidateEncodingFlag |
						rapidjson::kParseIterativeFlag>
				(data);
		// Assume this is a fake connection; return nothing if there's a parse error
		if (doc.HasParseError()) {
			delete data;
			return;
		}
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
	}
	delete data;*/
}

void RemDev::sendError(rapidjson::Value *id, int code, const QString &msg, rapidjson::Document *doc, rapidjson::Value *data) noexcept {
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
	prepareDocument(doc);
	doc->AddMember("error", e, a);
	if (id) doc->AddMember("id", *id, a);
	else doc->AddMember("id", Value().Move(), a);  // Add null if no id present
	sendDocument(doc);
}

void RemDev::log(const QByteArray &msg) const noexcept {
	QByteArray out(cid);
	out.append(": ").append(msg).append("\n");
	postToLogArea(msg);
}

/*QJsonObject RemDev::newRequest(LocalId id, const QString &method, const QJsonObject &params) const {
	/*QJsonObject o(newNotification(method, params));
	o.insert("id", id);
	return o;
}

QJsonObject RemDev::newResponse(QJsonValue id, const QJsonValue &result) {
	/*QJsonObject o(rpc_seed);
	o.insert("id", id);
	o.insert("result", result);
	return o;
}

QJsonObject RemDev::newError(QJsonValue id, int code, const QString &msg, const QJsonValue &data) const {
	QJsonObject e;
	e.insert("code", code);
	e.insert("message", msg);
	if (data.type() != QJsonValue::Undefined) e.insert("data", data);
	QJsonObject o(rpc_seed);
	o.insert("error", e);
	if (id.type() == QJsonValue::Undefined) id = QJsonValue::Null;
	o.insert("id", id);
	return o;
}

QJsonObject RemDev::newNotification(const QString &method, const QJsonObject &params) const {
	QJsonObject o(rpc_seed);
	o.insert("method", method);
	if (params.size()) o.insert("params", params);
	return o;
}*/

/*void RemDev::simulateError(LocalId id, const RequestRef *ref, int code) {
	
}*/

void RemDev::sendDocument(rapidjson::Document *doc) {
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	doc->Accept(writer);
	delete doc;
	writeItem(buffer.GetString());
}

void RemDev::handleRequest(const QJsonObject &obj) {
	
}

void RemDev::handleRegistration(const QJsonObject &obj) {
/*	// If this is not a register request or response, return without error
	if (inbound && QString::compare(obj.value("method").toString(), "register")) return;
	if ( ! inbound && ! obj.contains("result")) return;
	QJsonValue id = obj.value("id");
	// Check minimum version
	QString sent = obj.value("DDX_version").toString();
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
	
	registered = true;
	d->registerDevice(this);*/
}

void RemDev::prepareDocument(rapidjson::Document *doc) {
	doc->SetObject();
	doc->AddMember("jsonrpc", "2.0", doc->GetAllocator());
}








