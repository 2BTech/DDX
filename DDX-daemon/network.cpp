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

Network::Network(Daemon *parent) : QObject(parent)
{
	d = parent;
	server = new QTcpServer(this);
	connect(this, &Network::sendLog, parent, &Daemon::log);
}

Network::~Network() {
	// Forcibly close open connections (should have been gracefully handled in
	// shutdown() if possible)
	server->close();
	// These send disconnect signals - this needs to be handled.
	qDeleteAll(sockets);
	qDeleteAll(ur_sockets);
	delete server;

}

void Network::setupTcpServer() {
	int port = d->s("network/GUIPort").toInt();
	if ( ! server->listen(QHostAddress::Any, port)) {
		// TODO:  Should be alert
		log(tr("Server creation failed with error '%1'.  This is likely "
				 "because another DDX daemon is already running on this machine.")
			  .arg(server->errorString()));
		d->quit(E_TCP_SERVER_FAILED);
		return;
	}
	connect(server, &QTcpServer::acceptError, this, &Network::handleNetworkError);
	connect(server, &QTcpServer::newConnection, this, &Network::handleConnection);
}

void Network::shutdown() {
	log(tr("Closing network connections"));
	// TODO:  Close all connections gracefully without using event loop
	// This function must be thread-safe with regards to being called by
	// the daemon
	server->close();
}

void Network::handleData() {
	// TODO:  Add buffer size checks; if they exceed value (setting),
	// clear the buffer and send an error
	QHash<QString, QAbstractSocket*>::const_iterator it;
	for (it = sockets.constBegin(); it != sockets.constEnd(); ++it) {
		if ((*it)->canReadLine()) {
			log("Can read data");
		}
	}
	for (int i = 0; i < ur_sockets.size(); i++) {
		if (ur_sockets.at(i)->canReadLine()) {
			QString line = QString(ur_sockets.at(i)->readLine()).trimmed();
			log(QString("Device said '%1'").arg(line));
			if (QString::compare(line, "exit") == 0) {
				metaObject()->invokeMethod(d, "quit", Qt::QueuedConnection);
				return;
			}
		}
	}
}

void Network::handleConnection() {
	QTcpSocket *s;
	while ((s = server->nextPendingConnection())) {
		if (s->state() != QAbstractSocket::ConnectedState)
			log("Not in connected state?");
		log("Connection found");
		//if ( ! d->s("network/AllowExternalManagement").toBool())
		if (s->peerAddress() != QHostAddress(QHostAddress::LocalHost)) {
			log("Connection not from ipv4 localhost");
		}
		ur_sockets.append(s);
		// QTcpServer::error is overloaded, so we need to use this nasty thing
		connect(s, static_cast<void(QTcpSocket::*)(QAbstractSocket::SocketError)>(&QTcpSocket::error),
				this, &Network::handleNetworkError);
		connect(s, &QTcpSocket::disconnected, this, &Network::handleDisconnection);
		connect(s, &QTcpSocket::readyRead, this, &Network::handleData);
		if (s->bytesAvailable()) handleData();
	}
	log(tr("Server has %1 children").arg(QString::number(server->children().size())));
}

void Network::handleDisconnection() {
	QHash<QString, QAbstractSocket*>::iterator it = sockets.begin();
	QAbstractSocket *s;
	while (it != sockets.end()) {
		if ((*it)->state() == QAbstractSocket::UnconnectedState) {
			s = *it;
			it = sockets.erase(it);
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
	log(QString("Disconnected; there are %1 active connections").arg(QString::number(sockets.size()+ur_sockets.size())));
	//log("Disconnect");
	// This should loop through all active RPC requests and return an error
	// for any that relied on the connection that failed
}

void Network::handleNetworkError(QAbstractSocket::SocketError error) {
	// TODO
	
	// RemoteClosedError is emitted even on normal disconnections
	if (error == QAbstractSocket::RemoteHostClosedError) return;
	
	log(QString("DDX bug: Unhandled network error (QAbstractSocket): '%1'").arg(error));
}

void Network::log(const QString msg) const {
	// TODO
	QString out("network:");
	out.append(msg);
	emit sendLog(out);
}
