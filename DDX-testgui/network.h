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
	
	int connectDevice(const QString &hostName, quint16 port,
					   QAbstractSocket::NetworkLayerProtocol protocol = QAbstractSocket::AnyIPProtocol);
	
	void startServer();
	
	void stopServer();
	
	bool serverRunning() {return server;}
	
signals:
	
	void postToLogArea(const QString &msg) const;
	
	void doConnectPrivate(int ref, const QString &hostName, quint16 port,
				   QAbstractSocket::NetworkLayerProtocol protocol) const;
	
public slots:
	
	void init();
	
	/*!
	 * \brief Shutdown signal
	 * 
	 * Shut down the server, terminate and report any pending connections, and delete this instance.
	 * 
	 * **Mactitf**: Must be called with queued connections!
	 */
	void shutdown();
	
private slots:
	
	void handleNetworkError(QAbstractSocket::SocketError error);
	
	void handleSocketNowEncrypted();
	
	void handleEncryptionErrors(const QList<QSslError> & errors);
	
	void connectPrivate(int ref, const QString &hostName, quint16 port,
						 QAbstractSocket::NetworkLayerProtocol protocol);
	
private:
	
	struct PendingConnection {
		PendingConnection(QSslSocket *socket, int ref) {
			this->socket = socket;
			this->ref = ref;
		}
		QSslSocket *socket;
		int ref;
	};
	
	MainWindow *mw;
	
	typedef QList<PendingConnection> PendingList;
	
	PendingList pending;
	
	EncryptedServer *server;
	
	void handleSocket(qintptr sd);
	
	/*!
	 * \brief Log/report and removed a failed pending connection
	 * \param i The connection's index in #pending
	 * \param error The error message
	 */
	void pendingFailed(int index, const QString &error);
	
	static bool conditionSocket(QSslSocket *s);
	
	void log(const QString msg, bool isAlert = false) const;
	
};

class EncryptedServer : public QTcpServer
{
	Q_OBJECT
public:
	friend class Network;
private:
	
	Network *n;
	
	EncryptedServer(Network *parent)
		{n = parent;}
	
	void incomingConnection(qintptr socketDescriptor) override
		{n->handleSocket(socketDescriptor);}
	
	// ENCRYPTED SERVER
	
};

#endif // NETWORK_H
