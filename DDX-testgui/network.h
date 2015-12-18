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
#include <QSslEllipticCurve>
#include "testgui_constants.h"
#include "mainwindow.h"
#include "ddxrpc.h"

class EncryptedServer;
class DevMgr;
class NetDev;

/*!
 * \brief Network manager
 * 
 * Opens a TCP server with the EncryptedServer class and maintains a global
 * QNetworkAccessManager for the whole system
 * 
 * \ingroup testgui
 */
class Network : public QObject
{
	Q_OBJECT
public:
	friend class EncryptedServer;
	
	explicit Network(MainWindow *parent);
	
	~Network();
	
	/*!
	 * \brief Start the server
	 * 
	 * TODO:  Make this function thread-safe
	 */
	void startServer();
	
	/*!
	 * \brief Stop the server
	 * 
	 * TODO:  Make this function thread-safe
	 */
	void stopServer();
	
	//! Return whether the server is currently running
	bool serverRunning() {return server;}
	
	//! Concatenate SSL errors
	static QString sslErrorsToString(const QList<QSslError>& errors);
	
	/*!
	 * \brief getSslConfig
	 * \return 
	 * 
	 * TODO:  Document the SSL (actually TLS) configuration
	 */
	const QSslConfiguration &getSslConfig() const {return sslConfig;}
	
signals:
	
	void postToLogArea(const QString &msg) const;
	
public slots:
	
	void init();
	
	/*!
	 * \brief Delete this instance
	 * 
	 * TODO:  Make this function thread-safe
	 */
	void shutdown();
	
private slots:
	
	void handleNetworkError(QAbstractSocket::SocketError error);
	
private:
	
	MainWindow *mw;
	
	DevMgr *dm;
	
	EncryptedServer *server;
	
	QSslConfiguration sslConfig;
	
	void log(const QString msg, bool isAlert = false) const;
	
};

/*!
 * \brief Opens a TCP server which instantiates NetDev for each incoming connection
 * 
 * \ingroup testgui
 */
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
