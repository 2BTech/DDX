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

#include "netdev.h"
#include "devmgr.h"
#include "network.h"

NetDev::NetDev(Network *n, DevMgr *dm, qintptr socketDescriptor) : RemDev(dm, true) {
	s = 0;
	this->n = n;
	descriptor = socketDescriptor;
}

NetDev::NetDev(Network *n, DevMgr *dm, const QString &hostName, quint16 port,
			   QAbstractSocket::NetworkLayerProtocol protocol) : RemDev(dm, false) {
	s = 0;
	this->n = n;
	descriptor = 0;
	this->hostName = hostName;
	this->port = port;
	this->protocol = protocol;
}

NetDev::~NetDev() {
	if (s) delete s;
}

void NetDev::sub_init() noexcept {
	// Build, configure & connect socket
	s = new QSslSocket(this);
	s->setSslConfiguration(n->getSslConfig());
	// The error signals are overloaded, so we need to use these nasty things
	connect(s, static_cast<void(QSslSocket::*)(QAbstractSocket::SocketError)>(&QSslSocket::error),
			this, &NetDev::handleNetworkError);
	connect(s, static_cast<void(QSslSocket::*)(const QList<QSslError> &)>(&QSslSocket::sslErrors),
			this, &NetDev::handleEncryptionErrors);
	connect(s, &QSslSocket::encrypted, this, &NetDev::handleNowEncrypted);
	// Handle incoming connection
	if (descriptor) {
		if ( ! s->setSocketDescriptor(descriptor)) {
			connectionClosed(tr("Could not ingest socket"));
			return;
		}
		if (s->state() != QAbstractSocket::ConnectedState) {
			connectionClosed(tr("Socket disconnected"));
			return;
		}
		// TODO:  Figure out if the connection is local or not
		bool isLocal = false;
		// TODO:  Check whether non-local connections are allowed
		log(tr("Connection is from %1:%2").arg(s->peerAddress().toString(), QString::number(s->peerPort())));
		s->startServerEncryption();
	}
	// Create outgoing connection
	else {
		log(tr("Target device is at %1:%2").arg(hostName, QString::number(port)));
		s->connectToHostEncrypted(hostName, port, QAbstractSocket::ReadWrite, protocol);
		hostName.clear();
	}
}

void NetDev::terminate() noexcept {
	s->disconnectFromHost();
}

void NetDev::writeItem(rapidjson::StringBuffer *buffer) noexcept {
	emit doWritePrivate(buffer);
}

void NetDev::handleNowEncrypted() noexcept {
	
	
	/*if (s->protocol() != QSsl::TlsV1_2) {
		pendingFailed(i--, tr("Pending connection not using TLS v1.2"));
		continue;
	}
	if (s->state() != QAbstractSocket::ConnectedState) {
		pendingFailed(i--, tr("Pending connection was invalid"));
		continue;
	}*/
	
	
	// Determine basic information about the connection itself
	bool usingIPv6 = s->peerAddress().protocol() == QAbstractSocket::IPv6Protocol;
	QHostAddress localhost = usingIPv6 ? QHostAddress::LocalHostIPv6 : QHostAddress::LocalHost;
	bool isLocal = s->peerAddress() == localhost;
	
	s->setSocketOption(QAbstractSocket::LowDelayOption, 1);  // Disable Nagel's algorithm
	if ( ! isLocal) s->setSocketOption(QAbstractSocket::KeepAliveOption, 1);
	
	// TODO
	connect(this, &NetDev::doWritePrivate, this, &NetDev::writePrivate);
	connectionReady();
}

void NetDev::handleData() noexcept {
	if ( ! s->isEncrypted()) return;
	while (s->canReadLine()) {
		qint64 size = s->bytesAvailable() + 1;
		char *buffer = new char[size];
		qint64 amount = s->readLine(buffer, size);
		if (amount > 0) handleItem(buffer);
		else delete buffer;
	}
}

void NetDev::handleNetworkError(QAbstractSocket::SocketError error) noexcept {
	// RemoteClosedError is emitted even on normal disconnections
	if (error == QAbstractSocket::RemoteHostClosedError) {
		connectionClosed(QString(), true);
		return;
	}
	// TODO: Turn this into a string
	connectionClosed(tr("Network error #%1").arg(error));
}

void NetDev::handleEncryptionErrors(const QList<QSslError> & errors) noexcept {
	log(tr("OpenSSL complained about the following certificate error; ignoring: %1").arg(Network::sslErrorsToString(errors)));
	//connectionClosed(tr("OpenSSl: %1").arg(Network::sslErrorsToString(errors)));
}

void NetDev::writePrivate(rapidjson::StringBuffer *buffer) noexcept {
	s->write(buffer->GetString(), buffer->GetSize());
	delete buffer;
}
