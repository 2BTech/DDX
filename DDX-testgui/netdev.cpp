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
	ues = socket;
	es = 0;
}

NetDev::~NetDev() {
	if (es) delete es;
	if (ues) delete ues;
}

void NetDev::sub_init() noexcept {
	ues->setParent(this);
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
	ues->write(encryptionPhrase.append("\n"));
}

void NetDev::terminate(DisconnectReason reason, bool fromRemote) noexcept {
	(void) reason;
	(void) fromRemote;
	s()->disconnectFromHost();
}

void NetDev::writeItem(rapidjson::StringBuffer *buffer) noexcept {
	if (status & ReadyFlag)
		s()->write(buffer->GetString(), buffer->GetSize());
	delete buffer;
}

void NetDev::handleData() {
	
}

void NetDev::handleEncryptionPhrase() {
	if (ues->canReadLine()) {
		disconnect(this, &NetDev::handleEncryptionPhrase, 0, 0);
		// Get remote encryption status
		char line[30];
		ues->readLine(line, 30);
		if (strlen(line) > 11) {
			const char *sub = line + 11;
			uint_fast8_t local = status & LocalFilter;
			uint_fast8_t remote = 0;
			if (strcmp(sub, "required", 8) == 0) remote = RemoteRequired;
			else if (strcmp(sub, "enabled", 7) == 0) remote = RemoteEnabled;
			else if (strcmp(sub, "requested", 9) == 0) remote = RemoteRequested;
			else if (strcmp(sub, "disabled", 8) == 0) remote = RemoteDisabled;
			// Determine encryption usage
		}
		// The encryption phrase could not be read
		close(EncryptionRequired);
		return;
	}
	// If it's just spewing data at us, this is probably not a DDX device
	else if (ues->bytesAvailable() > 30) {
		status = (EncryptionStatus) 0;
		ues->abort();
		close(BufferOverflow);
	}
	
	qintptr socketDescriptor = ues->socketDescriptor();
	ues->disconnect();  // Prevent any error signals from escaping
	ues->setSocketDescriptor(0, QAbstractSocket::UnconnectedState);
	
	es = new QSslSocket(this);
	if ( ! es->setSocketDescriptor(socketDescriptor)) {
		connect(es, &QSslSocket::encrypted, this, &NetDev::handleEncryptionPhrase);
		es->startServerEncryption();
	} else {
		delete serverSocket;
	}
	
	ues->deleteLater();
	ues = 0;
	
}

inline QTcpSocket *NetDev::s() {
	if (es) return es;
	return ues;
}
