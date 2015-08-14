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

RemDev::RemDev(const QString &name, Daemon *parent) : QObject(parent) {
	this->name = name;
	d = parent;
	lg = Logger::get();
	sg = d->getSettings();
	lastId = 0;
	connectTime = QDateTime::currentMSecsSinceEpoch();
	/*Connection(QTcpSocket *socket, bool inbound, bool v6) {
		connectTime = QDateTime::currentMSecsSinceEpoch();
		s = socket;
		this->inbound = inbound;
		this->v6 = v6;
	}*/
	timeoutPoller = new QTimer(this);
	timeoutPoller->setTimerType(Qt::VeryCoarseTimer);
	timeoutPoller->setInterval(TIMEOUT_POLL_INTERVAL);
	connect(timeoutPoller, &QTimer::timeout, this, &RemDev::timeoutPoll);
}

RemDev::~RemDev() {
	
}

qint64 RemDev::sendRequest(ResponseHandler handler, const QString &method,
						   const QJsonObject &params, qint64 timeout) {
	if ( ! valid()) return 0;
	// Obtain a server-unique id
	LocalId id = getId();
	
	
	
	// Start timeout timer if required
	if (timeout && ! timeoutPoller->isActive())
		timeoutPoller->start();
	// Insert into request list
	rLock.lock();
	reqs.insert(id, RequestRef(handler, timeout));
	rLock.unlock();
}

bool RemDev::sendResponse(LocalId id, const QJsonValue &result) {
	if ( ! valid()) return false;
	
}

bool RemDev::sendError(LocalId id, int code, const QString &msg, const QJsonValue &data) {
	if ( ! valid()) return false;
	
}

bool RemDev::sendNotification(const QString &method, const QJsonObject &params) {
	if ( ! valid()) return false;
	
}

void RemDev::timeoutPoll() {
	qint64 time = QDateTime::currentMSecsSinceEpoch();
	QMutexLocker l(&rLock);
	RequestHash::iterator it = reqs.begin();
	while (it != reqs.end()) {
		if (it->timeout_time <= time) {
			// TODO:  Generate error object here
			(*it->handler)(QJsonValue(), QJsonValue(), false);
			it = reqs.erase(it);
		}
		else ++it;
	}
}

QJsonObject RemDev::newRequest(LocalId id, const QString &method, const QJsonObject &params) const {
	QJsonObject o = newNotification(method, params);
	if (id) o.insert("id", id);
	else o.insert("id", QJsonValue());
	return o;
}

QJsonObject RemDev::newResponse(QJsonValue id, const QJsonValue &result) {
	
}

QJsonObject RemDev::newNotification(const QString &method, const QJsonObject &params) const {
	QJsonObject o(rpc_seed);
	o.insert("method", method);
	if ( ! params.size()) o.insert("params", params);
	return o;
}

QJsonObject RemDev::newError(int id, int code, const QString &msg, const QJsonValue &data) const {
	QJsonObject e;
	e.insert("code", code);
	e.insert("message", msg);
	if (data.type() != QJsonValue::Undefined) e.insert("data", data);
	QJsonObject o(rpc_seed);
	o.insert("error", e);
	if (id) o.insert("id", id);
	else o.insert("id", QJsonValue());
	return o;
}

void RemDev::log(const QString &msg, bool isAlert) const {
	QString out(name);
	out.append(": ");
	out.append(msg);
	lg->log(msg, isAlert);
}

/*void RemDev::simulateError(LocalId id, const RequestRef *ref, int code) {
	
}*/

void RemDev::sendObject(const QJsonObject &object) {
	QByteArray json = QJsonDocument(object).toJson();
	json.append("\n");
	write(json);
}

RemDev::LocalId RemDev::getId() {
	// I believe I can get rid of this mutex with some volatile
	// stuff, but I'm too lazy to learn it right now and this works fine
	LocalId id;
	idLock.lock();
	id = ++lastId;
	if (id == std::numeric_limits<qint64>::max()) {
		log("ID counter overflow; resetting to 1");
		lastId = 0;
	}
	idLock.unlock();
	return id;
}

const QJsonObject RemDev::rpc_seed{{"jsonrpc","2.0"}};
