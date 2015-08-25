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
#include "remdev.h"

class DevMgr;

class NetDev : public RemDev
{
	Q_OBJECT
public:
	
	explicit NetDev(DevMgr *dm, bool inbound);
	
	~NetDev();
	
protected:
	
	void sub_init() noexcept override;
	
	void terminate(DisconnectReason reason, bool fromRemote) noexcept override;
	
	void writeItem(const char *data) noexcept override;
	
	const char *getType() const noexcept override {return "Network";}
	
private:
	
};

#endif // NETDEV_H
