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

NetDev::NetDev(DevMgr *dm, QTcpSocket *socket) : RemDev(dm, true) {
	status = DefaultEncryptionStatus;
	s = socket;
}

NetDev::~NetDev() {
	delete s;
}

void NetDev::sub_init() noexcept {
/*	ues->setParent(this);
	bool local;
	QByteArray encryptionPhrase("encryption:");
	if (local) {
		status = LocalEnabled | RemoteUnknown | DeterminingState;
		encryptionPhrase.append("enabled");
	}
	else {
		status = LocalRequired | RemoteUnknown | DeterminingState;
		encryptionPhrase.append("required");
	}
	ues->write(encryptionPhrase.append("\n"));*/
}

void NetDev::terminate(DisconnectReason reason, bool fromRemote) noexcept {
	(void) reason;
	(void) fromRemote;
	s->disconnectFromHost();
}

void NetDev::writeItem(rapidjson::StringBuffer *buffer) noexcept {
	s->write(buffer->GetString(), buffer->GetSize());
	delete buffer;
}

void NetDev::handleData() {
	
}

void NetDev::handleEncryptionPhrase() {
	if (status & RemoteKnownFlag) return;
	if (s->canReadLine()) {
		// Get remote encryption status
		if ( ! determineEncryption())
			close(EncryptionRequired);
		return;
	}
	// If it's just spewing data at us, this is probably not a DDX device
	else if (s->bytesAvailable() > 24) {
		status = (EncryptionStatus) 0;
		s->abort();
		close(BufferOverflow);
	}
}

inline bool NetDev::determineEncryption() {
	// This function does not currently support disabling encryption
	Q_ASSERT((status & LocalFilter) != LocalDisabled);
	uint_fast8_t local = status & LocalFilter;
	uint_fast8_t remote = 0;
	{
		QByteArray phrase = s->readLine();
		if (phrase.contains("required")) remote = RemoteRequired;
		else if (phrase.contains("requested")) remote = RemoteRequested;
		else if (phrase.contains("enabled")) remote = RemoteEnabled;
		else if (phrase.contains("disabled")) remote = RemoteDisabled;
		else return false;
	}
	status |= remote;
	// Disconnect if connection unsafe
	if (local == LocalRequired && remote == RemoteDisabled) return false;
	// Use no encryption
	if (local == LocalEnabled && (remote == RemoteEnabled || remote == RemoteDisabled)) {
		status |= ReadyFlag;
		connectionReady();
		return true;
	}
	// Use encryption
	qintptr socketDescriptor = s->socketDescriptor();
	s->disconnect();  // Prevent any error signals from escaping
	s->setSocketDescriptor(0, QAbstractSocket::UnconnectedState);
	
	disconnect(this, &NetDev::handleEncryptionPhrase, 0, 0);
	
	s = new QSslSocket(this);
	s->setProtocol(QSsl::TlsV1_2);
	if (s->protocol() != QSsl::TlsV1_2) return false;
	if ( ! s->setSocketDescriptor(socketDescriptor)) {
		connect(s, &QSslSocket::encrypted, this, &NetDev::handleEncryptionPhrase);
		s->startServerEncryption();
	} else {
		delete s;
		s->deleteLater();
		s = 0;
	}
	return true;
}
