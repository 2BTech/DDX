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
#include "devmgr.h"

Network::Network(MainWindow *parent) : QObject(0)
{
	// Initialization
	mw = parent;
	server = 0;
	// Connections
	connect(this, &Network::postToLogArea, mw->getLogArea(), &QPlainTextEdit::appendPlainText);
	connect(this, &Network::doConnectPrivate, this, &Network::connectPrivate);
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
	if (server) delete server;
	for (int i = 0; i < pending.size(); i++)
		delete pending.at(i).socket;
}

int Network::connectDevice(const QString &hostName, quint16 port,
							QAbstractSocket::NetworkLayerProtocol protocol) {
	int ref = mw->dm->getRef();
	emit doConnectPrivate(ref, hostName, port, protocol);
	log(tr("Connecting to target at %1 with ref %2").arg(hostName, QString::number(ref)));
	return ref;
}

void Network::startServer() {
	if (server) return;
	server = new EncryptedServer(this);
	connect(server, &EncryptedServer::acceptError, this, &Network::handleNetworkError);
	int port = 4388;
	QHostAddress a = QHostAddress::Any;
	if ( ! server->listen(a, port)) {
		// TODO:  Handle this better
		server->deleteLater();
		server = 0;
		log("Starting server failed");
	}
	else log("Started server");
}

void Network::stopServer() {
	if ( ! server) return;
	server->deleteLater();
	server = 0;
	log(tr("Stopped server"));
}

void Network::init() {
	// TODO: add a QNetworkAccessManager and related stuff so Modules can use the high-level APIs
	
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
	stopServer();
	while (pending.size()) pendingFailed(0, tr("The network manager is shutting down"));
	deleteLater();
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
	
	
	// Note: RemoteHostClosedError is still a valid error at this point in time since
	// we don't handle explicit disconnections while sockets are managed by Network
	
	log(QString("DDX bug: Unhandled network error (QAbstractSocket): '%1'").arg(error));
	//pendingFailed()
}

void Network::handleSocketNowEncrypted() {
	for (int i = 0; i < pending.size(); i++) {
		QSslSocket *s = pending.at(i).socket;
		if (s->isEncrypted()) {
			if (s->protocol() != QSsl::TlsV1_2) {
				pendingFailed(i--, tr("Pending connection not using TLS v1.2"));
				continue;
			}
			if (s->state() != QAbstractSocket::ConnectedState) {
				pendingFailed(i--, tr("Pending connection was invalid"));
				continue;
			}
			new NetDev(mw->dm, (QSslSocket*) sender());
		}
	}
}

void Network::handleEncryptionErrors(const QList<QSslError> & errors) {
	// TODO
	log(tr("Unhandled encryption error"));
}

void Network::connectPrivate(int ref, const QString &hostName, quint16 port,
							  QAbstractSocket::NetworkLayerProtocol protocol) {
	QSslSocket *s = new QSslSocket(0);
	s->setProtocol(QSsl::TlsV1_2);
	pending.append(PendingConnection(s, ref));
	// The error signals are overloaded, so we need to use these nasty things
	connect(s, static_cast<void(QSslSocket::*)(QAbstractSocket::SocketError)>(&QSslSocket::error),
			this, &Network::handleNetworkError);
	connect(s, static_cast<void(QSslSocket::*)(const QList<QSslError> &)>(&QSslSocket::sslErrors),
			this, &Network::handleEncryptionErrors);
	connect(s, &QSslSocket::encrypted, this, &Network::handleSocketNowEncrypted);
	s->connectToHostEncrypted(hostName, port, QAbstractSocket::ReadWrite, protocol);
}

void Network::handleSocket(qintptr sd) {
	// Build a new QSslSocket to manage this socket.
	QSslSocket *s = new QSslSocket(0);
	s->setProtocol(QSsl::TlsV1_2);
	if ( ! s->setSocketDescriptor(sd)) {
		s->deleteLater();
		return;
	}
	if (s->state() != QAbstractSocket::ConnectedState) {
		s->deleteLater();
		return;
	}
	pending.append(PendingConnection(s, 0));
	// QSslServer::sslErrors is overloaded, so we need to use this nasty thing
	connect(s, static_cast<void(QSslSocket::*)(const QList<QSslError> &)>(&QSslSocket::sslErrors),
			this, &Network::handleEncryptionErrors);
	connect(s, &QSslSocket::encrypted, this, &Network::handleSocketNowEncrypted);
	s->startServerEncryption();
	log(tr("Received new connection from %1:%2").arg(s->peerAddress().toString(), QString::number(s->peerPort())));
}

void Network::pendingFailed(int index, const QString &error) {
	const PendingConnection &con = pending.at(index);
	con.socket->deleteLater();
	if (con.ref) mw->dm->reportFailedConnection(con.ref, error);
	else log(tr("Incoming connection failed to start: %1").append(error));
	pending.removeAt(index);
}

bool Network::conditionSocket(QSslSocket *s) {
	
}

void Network::log(const QString msg, bool isAlert) const {
	(void) isAlert;
	QString out("network: ");
	out.append(msg);
	emit postToLogArea(out);
}
