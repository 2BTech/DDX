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
#include <QSslConfiguration>
#include <QSslCertificate>
#include <QSslKey>
#include <QSslCipher>
#include <QAbstractSocket>
#include <QtAlgorithms>
#include <QSslConfiguration>
#include "constants.h"
#include "mainwindow.h"

class EncryptedServer;
class DevMgr;
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
	
	static QString sslErrorsToString(const QList<QSslError> & errors);
	
	const QSslConfiguration &getSslConfig() const {return sslConfig;}
	
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
	
	void handleEncryptionErrors(const QList<QSslError> & errors);
	
	void handleSocketNowEncrypted();
	
	void connectPrivate(int ref, const QString &hostName, quint16 port,
						 QAbstractSocket::NetworkLayerProtocol protocol);
	
private:
	
	MainWindow *mw;
	
	DevMgr *dm;
	
	EncryptedServer *server;
	
	QSslConfiguration sslConfig;
	
	void log(const QString msg, bool isAlert = false) const;
	
};

class EncryptedServer : public QTcpServer
{
	Q_OBJECT
public:
	friend class Network;
private:
	
	Network *n;
	
	/*!
	 * \brief EncryptedServer
	 * \param parent
	 */
	EncryptedServer(Network *parent) {n = parent;}
	
	/*!
	 * \brief Handle an incoming connection
	 * \param socketDescriptor The OS socket descriptor
	 * 
	 * Instantiates NetDev on the socket.
	 */
	void incomingConnection(qintptr socketDescriptor) override;
	
	// ENCRYPTED SERVER
	
};

#endif // NETWORK_H
