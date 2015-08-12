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

#ifndef NETWORK_H
#define NETWORK_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonValue>
#include <QtAlgorithms>
#include <QTimer>
#include <QNetworkAccessManager>
#include "constants.h"
#include "daemon.h"
#include "netdev.h"

class Logger;
class Settings;

class Network : public QObject
{
	Q_OBJECT
public:
	
	explicit Network(Daemon *daemon);
	
	~Network();
	
	void shutdown();
	
	void responseError(const Connection *c, int code, const QString &msg = QString());
	
signals:
	
public slots:
	
	/*!
	 * \brief Install a TCP server on the appropriate port
	 * 
	 * Note that this function may quit the application on failure.  Callers
	 * Callers should be prepared to do so cleanly if necessary.
	 */
	void init();
	
private slots:
	
	void handleData();
	
	void handleConnection();
	
	void handleDisconnection();
	
	void handleNetworkError(QAbstractSocket::SocketError error);
	
private:
	
	Logger *lg;
	
	Settings *sg;
	
	RemDev::DeviceHash cons;
	
	Daemon *d;
	
	QTcpServer *server;
	
	void log(const QString msg) const;
	
	QByteArray generateCid(const QByteArray &base) const;
	
	QNetworkAccessManager nam;
	
};

#endif // NETWORK_H
