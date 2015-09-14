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
#include <QSslSocket>
#include <QAbstractSocket>
#include <QHostAddress>
#include "ddxrpc.h"
#include "remdev.h"

class DevMgr;
class Network;

/*!
 * \brief TCP-based RemDev subclass
 * 
 * Note that no lock is placed on the socket instance.  This is because it is only accessed from
 * within thread-local slots.  This is also why writeItem() is implemented by emitting the
 * doWritePrivate() signal, which is connected to writePrivate().
 * 
 * \ingroup testgui
 */
class NetDev : public RemDev
{
	Q_OBJECT
public:
	
	explicit NetDev(Network *n, DevMgr *dm, qintptr socketDescriptor);
	
	explicit NetDev(Network *n, DevMgr *dm, const QString &hostName, quint16 port,
					QAbstractSocket::NetworkLayerProtocol protocol = QAbstractSocket::AnyIPProtocol);
	
	~NetDev();
	
signals:
	
	void doWritePrivate(rapidjson::StringBuffer *buffer) const;
	
protected:
	
	void sub_init() noexcept override;
	
	void terminate() noexcept override;
	
	void writeItem(rapidjson::StringBuffer *buffer) noexcept override;
	
	const char *getType() const noexcept override {return "TCP";}
	
private slots:
	
	void handleNowEncrypted() noexcept;
	
	void handleData() noexcept;
	
	void handleNetworkError(QAbstractSocket::SocketError error) noexcept;
	
	void handleEncryptionErrors(const QList<QSslError> & errors) noexcept;
	
	void writePrivate(rapidjson::StringBuffer *buffer) noexcept;
	
private:
	
	Network *n;
	
	QSslSocket *s;
	
	qintptr descriptor;
	
	QString hostName;
	
	quint16 port;
	
	QAbstractSocket::NetworkLayerProtocol protocol;
	
};

#endif // NETDEV_H
