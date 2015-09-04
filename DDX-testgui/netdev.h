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

#ifndef NETDEV_H
#define NETDEV_H

#include <QObject>
#include <QTcpSocket>
#include <QSslSocket>
#include <QAbstractSocket>
#include "remdev.h"

class DevMgr;

class NetDev : public RemDev
{
	Q_OBJECT
public:
	
	explicit NetDev(DevMgr *dm, QTcpSocket *socket);
	
	~NetDev();
	
signals:
	
protected:
	
	void sub_init() noexcept override;
	
	void terminate(DisconnectReason reason, bool fromRemote) noexcept override;
	
	void writeItem(rapidjson::StringBuffer *buffer) noexcept override;
	
	const char *getType() const noexcept override {return "Network";}
	
	bool isEncrypted() const noexcept override {return (status & StateFilter) == ReadyEncryptedState;}
	
private slots:
	
	void handleData();
	
	void handleEncryptionPhrase();
	
private:
	
	enum EncryptionStatus {
		LocalDisabled = 0x0,
		LocalEnabled = 0x1,
		LocalRequested = 0x2,
		LocalRequired = 0x3,
		LocalFilter = 0x3,
		
		RemoteUnknown = 0x0,
		RemoteKnownFlag = 0x10,
		RemoteDisabled = 0x10,
		RemoteEnabled = 0x14,
		RemoteRequested = 0x18,
		RemoteRequired = 0x1C,
		RemoteFilter = 0xC,
		
		DeterminingState = 0x0,
		UsingEncryption = 0x20,
		WaitingForHandshakeState = UsingEncryption,
		HandshakeSuccess = 0x60,
		ReadyFlag = 0x80,
		ReadyEncryptedState = UsingEncryption | HandshakeSuccess | ReadyFlag,
		ReadyUnencryptedState = ReadyFlag,
		StateFilter = 0xE0,
		
		DefaultEncryptionStatus = LocalRequired | RemoteUnknown | DeterminingState
	};
	
	uint_fast8_t status;
	
	QTcpSocket *ues;
	
	QSslSocket *es;
	
	inline QTcpSocket *s();
	
};

#endif // NETDEV_H
