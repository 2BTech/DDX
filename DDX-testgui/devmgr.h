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

#ifndef DEVMGR_H
#define DEVMGR_H

#include <QObject>
#include <QByteArray>
#include <QList>
#include <QHash>
#include <QMutex>
#include <QPlainTextEdit>
#include "remdev.h"

class MainWindow;

class DevMgr : public QObject
{
	Q_OBJECT
public:
	friend class RemDev;
	
	explicit DevMgr(MainWindow *parent = 0);
	
	typedef QHash<QByteArray, RemDev*> DeviceHash;
	
	typedef QList<RemDev*> DeviceList;
	
	~DevMgr();
	
	void closeAll(RemDev::DisconnectReason reason = RemDev::ShuttingDown);
	
signals:
	
public slots:
	
private:
	
	MainWindow *mw;
	
	DeviceList devices;
	
	QMutex dLock;
	
	int unregCt;
	
	bool closing;
	
	QByteArray addDevice(RemDev *dev);
	
	void removeDevice(RemDev *dev);
	
};

#endif // DEVMGR_H
