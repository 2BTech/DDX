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

RemDev::RemDev(Daemon *parent) : QObject(parent)
{
	d = parent;
	lg = Logger::get();
	sg = d->getSettings();
	connectTime = QDateTime::currentMSecsSinceEpoch();
	/*Connection(QTcpSocket *socket, bool inbound, bool v6) {
		connectTime = QDateTime::currentMSecsSinceEpoch();
		s = socket;
		this->inbound = inbound;
		this->v6 = v6;
	}*/
}

RemDev::~RemDev()
{
	
}

QJsonObject RemDev::rpc_newNotification(const QString &method, const QJsonObject &params) const {
	QJsonObject o(rpc_seed);
	o.insert("method", method);
	if ( ! params.size()) o.insert("params", params);
	return o;
}

QJsonObject RemDev::rpc_newRequest(int id, const QString &method, const QJsonObject &params) const {
	QJsonObject o = rpc_newNotification(method, params);
	if (id) o.insert("id", id);
	else o.insert("id", QJsonValue());
	return o;
}

QJsonObject RemDev::rpc_newError(int id, int code, const QString &msg, const QJsonValue &data) const {
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

void RemDev::registerTimeout() {
	// TODO: put this in a loop
	Logger::get()->log("This function is horribly, horribly incomplete and about to crash");
	//Connection *c;
	if ( ! c->valid()) {
		if ((c->connectTime+REGISTRATION_TIMEOUT) < QDateTime::currentMSecsSinceEpoch()) {
			// Disconnect because of registration timeout
			// TODO
		}
	}
	rpc_newError(2,2,"test", true);
}
