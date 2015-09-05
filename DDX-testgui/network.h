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
#include <QSslSocket>
#include <QAbstractSocket>
#include <QtAlgorithms>
#include "constants.h"
#include "mainwindow.h"

class EncryptedServer;
class NetDev;

class Network : public QObject
{
	Q_OBJECT
public:
	friend class EncryptedServer;
	
	explicit Network(MainWindow *parent);
	
	~Network();
	
	void connectDevice(const QString &hostName, quint16 port, bool encrypted = true,
					   QAbstractSocket::NetworkLayerProtocol protocol = AnyIPProtocol);
	
	void shutdown();
	
signals:
	
	void postToLogArea(const QString &msg) const;
	
public slots:
	
	void init();
	
private slots:
	
	void handleUnencryptedConnection();
	
	void handleNetworkError(QAbstractSocket::SocketError error);
	
	void handleSocketNowEncrypted();
	
	void handleEncryptionErrors(const QList<QSslError> & errors);
	
private:
	
	MainWindow *mw;
	
	QList<QSslSocket*> pendingSockets;
	
	EncryptedServer *encrypted;
	
	QTcpServer *unencrypted;
	
	void handleConnection(QTcpSocket *socket);
	
	void handleEncryptedSocket(qintptr sd);
	
	static void conditionSocket(QTcpSocket *s);
	
	void log(const QString msg, bool isAlert = false) const;
	
};

class EncryptedServer : public QTcpServer
{
	Q_OBJECT
public:
	friend class Network;
	
private:
	
	EncryptedServer(Network *parent);
	
	void incomingConnection(qintptr socketDescriptor) override;
	
	Network *n;
	
	// ENCRYPTED SERVER
	
};

#endif // NETWORK_H
