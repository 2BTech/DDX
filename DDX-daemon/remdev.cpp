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
#include "logger.h"
#include "daemon.h"
#include "settings.h"

RemDev::RemDev(const QString &type, Daemon *parent) : QObject(parent) {
	// TODO:  Move this into the listing code so that we can take advantage of the lock it will use
	this->name = QString("Unregistered%1%2").arg(type, QString::number(d->countRemoteDevices()+1));
	d = parent;
	lg = Logger::get();
	sg = d->getSettings();
	lastId = 0;
	connectTime = QDateTime::currentMSecsSinceEpoch();
	int registrationPeriod = sg->v("RegistrationPeriod", SG_RPC).toInt();
	registrationTimeoutTime = connectTime + (registrationPeriod * 1000);
	// Set up timeout polling
	timeoutPoller = new QTimer(this);
	timeoutPoller->setTimerType(Qt::CoarseTimer);
	timeoutPoller->setInterval(TIMEOUT_POLL_INTERVAL);
	connect(timeoutPoller, &QTimer::timeout, this, &RemDev::timeoutPoll);
	timeoutPoller->start();  // Start immediately for registration timeout
}

RemDev::~RemDev() {
	
}

int RemDev::sendRequest(ResponseHandler handler, const QString &method,
						const QJsonObject &params, qint64 timeout) {
	if ( ! valid()) return 0;
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

bool RemDev::sendResponse(LocalId id, const QJsonValue &result) {
	if ( ! valid()) return false;
	if (result.type() == QJsonValue::Undefined) {
		lg->log(tr("DDX bug: an RPC method returned a response with no result"));
		sendError(id, E_METHOD_RESPONSE_INVALID, tr("Method gave invalid response"));
		return false;
	}
	QJsonObject response = newResponse(id, result);
	sendObject(response);
	return true;
}

bool RemDev::sendError(LocalId id, int code, const QString &msg, const QJsonValue &data) {
	if ( ! valid()) return false;
	QJsonObject error = newError(id, code, msg, data);
	sendObject(error);
	return true;
}

bool RemDev::sendNotification(const QString &method, const QJsonObject &params) {
	if ( ! valid()) return false;
	QJsonObject notification = newNotification(method, params);
	sendObject(notification);
	return true;
}

void RemDev::timeoutPoll() {
	qint64 time = QDateTime::currentMSecsSinceEpoch();
	QMutexLocker l(&req_id_lock);
	RequestHash::iterator it = reqs.begin();
	while (it != reqs.end()) {
		if (it->timeout_time && it->timeout_time < time) {
			// TODO:  Generate error object here
			(*it->handler)(0, QJsonValue(), false);
			it = reqs.erase(it);
		}
		else ++it;
	}
	l.unlock();
	if ( ! valid() && registrationTimeoutTime < time) {
		// TODO: Disconnect for registration timeout
	}
}

void RemDev::handleLine(const QByteArray &data) {
	/*if (data.size() > MAX_TRANSACTION_SIZE) {
		// TODO
		return;
	}*/
	QJsonObject obj;
	{
		QJsonParseError error;
		QJsonDocument doc = QJsonDocument::fromJson(data, &error);
		if (error.error != QJsonParseError::NoError) {
			
			return;
		}
		if ( ! doc.isObject()) {
			// TODO
			return;
		}
		QJsonObject obj = doc.object();
	}
	
}

void RemDev::log(const QString &msg, bool isAlert) const {
	QString out(name);
	out.append(": ").append(msg);
	lg->log(msg, isAlert);
}

QJsonObject RemDev::newRequest(LocalId id, const QString &method, const QJsonObject &params) const {
	QJsonObject o = newNotification(method, params);
	if (id) o.insert("id", id);
	else o.insert("id", QJsonValue::Null);
	return o;
}

QJsonObject RemDev::newResponse(QJsonValue id, const QJsonValue &result) {
	QJsonObject o(rpc_seed);
	o.insert("id", id);
	o.insert("result", result);
	return o;
}

QJsonObject RemDev::newNotification(const QString &method, const QJsonObject &params) const {
	QJsonObject o(rpc_seed);
	o.insert("method", method);
	if (params.size()) o.insert("params", params);
	return o;
}

QJsonObject RemDev::newError(LocalId id, int code, const QString &msg, const QJsonValue &data) const {
	QJsonObject e;
	e.insert("code", code);
	e.insert("message", msg);
	if (data.type() != QJsonValue::Undefined) e.insert("data", data);
	QJsonObject o(rpc_seed);
	o.insert("error", e);
	if (id) o.insert("id", id);
	else o.insert("id", QJsonValue::Null);
	return o;
}

/*void RemDev::simulateError(LocalId id, const RequestRef *ref, int code) {
	
}*/

void RemDev::sendObject(const QJsonObject &object) {
	QByteArray json = QJsonDocument(object).toJson(QJsonDocument::Compact);
	json.append("\n");
	write(json);
}

void RemDev::handleObject(const QJsonObject &obj) {
	if (obj.contains("method")) {
		// handle request/notification
		return;
	}
	if (obj.contains("id")) {
		// handle response/error, including null-id error
		return;
	}
	// The request was invalid
}

const QJsonObject RemDev::rpc_seed{{"jsonrpc","2.0"}};
