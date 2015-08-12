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

#ifndef REMDEV_H
#define REMDEV_H

#include <QObject>
#include <QString>
#include <QJsonObject>
#include <QJsonValue>
#include <QDateTime>
#include <QTimer>
#include <QTimeZone>

class Daemon;
class Logger;
class Settings;

class RemDev : public QObject
{
	Q_OBJECT
public:
	
	explicit RemDev(Daemon *parent = 0);
	
	~RemDev();
	
	bool valid() {return !cid.isNull();}
	
	static const QJsonObject rpc_seed;
	
signals:
	
public slots:
	
protected:
	
	Daemon *d;
	
	Logger *lg;
	
	Settings *sg;
	
	QByteArray cid;
	
	QByteArray client_cid;
	
	QByteArray locale;
	
	qint64 connectTime;
	
	bool inbound;
	
	QJsonObject rpc_newNotification(const QString &method, const QJsonObject &params = QJsonObject()) const;
	
	QJsonObject rpc_newRequest(int id, const QString &method, const QJsonObject &params = QJsonObject()) const;
	
	QJsonObject rpc_newError(int id, int code, const QString &msg, const QJsonValue &data = QJsonValue::Undefined) const;
	
	void registerTimeout();
	
	
};
	
	const QJsonObject RemDev::rpc_seed{{"jsonrpc","2.0"}};

#endif // REMDEV_H
