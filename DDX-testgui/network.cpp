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

#include "network.h"
#include "netdev.h"

Network::Network(MainWindow *parent) : QObject(0)
{
	// Initialization
	mw = parent;
	// Connections
	connect(this, &Network::postToLogArea, mw->getLogArea(), &QPlainTextEdit::appendPlainText);
	// Threading
#ifdef NETWORK_THREAD
	QThread *t = new QThread(daemon);
	moveToThread(t);
	connect(t, &QThread::started, this, &Network::init);
	connect(this, &Network::destroyed, t, &QThread::quit);
	connect(t, &QThread::finished, t, &QThread::deleteLater);
	t->start();
#else
	init();
#endif
}

Network::~Network() {
	if (encrypted) delete encrypted;
	if (unencrypted) delete unencrypted;
	qDeleteAll(pendingSockets);
}

void Network::init() {
	// TODO: add a QNetworkAccessManager and related stuff so Modules can use the high-level APIs
	
	// Initialize the encrypted server
	encrypted = new EncryptedServer(this);
	connect(encrypted, &EncryptedServer::acceptError, this, &Network::handleNetworkError);
	int port = 4384;
	QHostAddress a = QHostAddress::Any;
	if ( ! encrypted->listen(a, port)) {
		encrypted->deleteLater();
		encrypted = 0;
		log("Starting encrypted server failed");
	}
	else log("Started encrypted server");
	
	// Initialize the unencrypted server
	unencrypted = new QTcpServer(this);
	connect(unencrypted, &QTcpServer::acceptError, this, &Network::handleNetworkError);
	connect(unencrypted, &QTcpServer::newConnection, this, &Network::handleUnencryptedConnection);
	port = 4388;
	a = QHostAddress::Any;
	if ( ! unencrypted->listen(a, port)) {
		unencrypted->deleteLater();
		unencrypted = 0;
		log("Starting unencrypted server failed");
	}
	else log("Started unencrypted server");
	
	// Old code
	/*if (sg->v("AllowExternal", SG_NETWORK).toBool())
		a = QHostAddress::Any;
	else {
		if (sg->v("UseIPv6Localhost", SG_NETWORK).toBool())
			a = QHostAddress::LocalHostIPv6;
		else a = QHostAddress::LocalHost;
	}
	if ( ! server->listen(a, port)) {
		// TODO:  Should be alert
		log(tr("Server creation failed with error '%1'.  This is likely "
				 "because another DDX daemon is already running on this machine.")
			  .arg(server->errorString()));
		d->quit(E_TCP_SERVER_FAILED);
		return;
	}*/
}

void Network::shutdown() {
	if (encrypted) encrypted->close();
	if (unencrypted) unencrypted->close();
}

void Network::handleUnencryptedConnection() {
	QTcpSocket *s;
	while ((s = unencrypted->nextPendingConnection())) {
		if (s->state() != QAbstractSocket::ConnectedState) {
			log(tr("Pending connection was invalid"));
			s->deleteLater();
			continue;
		}
		// Determine basic information about the connection itself
		bool usingIPv6 = s->peerAddress().protocol() == QAbstractSocket::IPv6Protocol;
		QHostAddress localhost = usingIPv6 ? QHostAddress::LocalHostIPv6 : QHostAddress::LocalHost;
		bool isLocal = s->peerAddress() == localhost;
		
		//QTimer::singleShot(REGISTRATION_TIMEOUT_TIMER, Qt::VeryCoarseTimer, this, &RemDev::registerTimeout);
		
		NetDev *dev = new NetDev(mw->dm, s);
	}
}

/*void Network::handleDisconnection() {
	QHash<QTcpSocket*, NetDev*>::iterator it = cons.begin();
	QTcpSocket *s;
	while (it != cons.end()) {
		if ((*it)->socket()->state() == QAbstractSocket::UnconnectedState) {
			s = (*it)->socket();
			it = cons.erase(it);
			s->deleteLater();
		}
		else ++it;
	}
	for (int i = 0; i < ur_sockets.size();) {
		if (ur_sockets.at(i)->state() == QAbstractSocket::UnconnectedState ) {
			s = ur_sockets.at(i);
			ur_sockets.removeAt(i);
			s->deleteLater();
		}
		else i++;
	}
	log(QString("Disconnected; there are %1 active connections").arg(QString::number(cons.size())));
	//log("Disconnect");
	// This should loop through all active RPC requests and return an error
	// for any that relied on the connection that failed
}*/

void Network::handleNetworkError(QAbstractSocket::SocketError error) {
	// TODO
	
	// RemoteClosedError is emitted even on normal disconnections
	if (error == QAbstractSocket::RemoteHostClosedError) return;
	
	log(QString("DDX bug: Unhandled network error (QAbstractSocket): '%1'").arg(error));
}

void Network::handleSocketNowEncrypted() {
	
}

void Network::handleEncryptionErrors(const QList<QSslError> & errors) {
	
}

void Network::handleConnection(QTcpSocket *socket) {
	
}

void Network::handleEncryptedSocket(qintptr sd) {
	// Build a new QSslSocket to manage this socket.
	QSslSocket *s = new QSslSocket(0);
	s->setProtocol(QSsl::TlsV1_2);
	if (s->protocol() != QSsl::TlsV1_2) {
		s->deleteLater();
		return;
	}
	if ( ! s->setSocketDescriptor(sd)) {
		s->deleteLater();
		return;
	}
	pendingSockets.append(s);
	
	// QSslServer::sslErrors is overloaded, so we need to use this nasty thing
	connect(s, static_cast<void(QSslSocket::*)(const QList<QSslError> &)>(&QSslSocket::sslErrors),
			this, &Network::handleEncryptionErrors);
	connect(s, &QSslSocket::encrypted, this, &Network::handleSocketNowEncrypted);
	s->startServerEncryption();
}

void Network::log(const QString msg, bool isAlert) const {
	(void) isAlert;
	QString out("network: ");
	out.append(msg);
	emit postToLogArea(out);
}

EncryptedServer::EncryptedServer(Network *parent) : QTcpServer(parent) {
	n = parent;
}

void EncryptedServer::incomingConnection(qintptr socketDescriptor) {
	n->handleEncryptedSocket(socketDescriptor);
}
