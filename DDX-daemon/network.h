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
#include "daemon_constants.h"
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
	
	void handleConnection(QTcpSocket *socket);
	
	void handleDisconnection();
	
	void handleNetworkError(QAbstractSocket::SocketError error);
	
private:
	Daemon *d;  //!< Convenience pointer to Daemon instance
	Logger *lg;  //!< Convenience pointer to Logger instance
	Settings *sg;  //!< Convenience pointer to Settings instance
	
	QHash<QTcpSocket*, NetDev*> cons;
	
	QTcpServer *server;
	
	void log(const QString msg) const;
	
	QByteArray generateCid(const QByteArray &base) const;
	
	QNetworkAccessManager nam;
	
};

#endif // NETWORK_H
