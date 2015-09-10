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

#ifndef TESTDEV_H
#define TESTDEV_H

#include <QObject>
#include <QTimer>
#include <random>
#include "remdev.h"
#include "ddxrpc.h"

class DevMgr;

class TestDev : public RemDev
{
	Q_OBJECT
public:
	
	explicit TestDev(DevMgr *dm, bool inbound);
	
	~TestDev();
	
	Q_INVOKABLE void responseHandler(Response *r);
	
	Q_INVOKABLE void requestHandler(Request *r);
	
	// These are defined in RemDev if QT_DEBUG is defined
#ifndef QT_DEBUG
	void printReqs() const;
#endif
	
protected:
	
	void sub_init() noexcept override;
	
	void terminate() noexcept override;
	
	void writeItem(rapidjson::StringBuffer *buffer) noexcept override;
	
	const char *getType() const noexcept override {return "Test";}
	
public slots:
	
	void timeout();
	
private:
	
	int eventCt = 0;
	
	QList<int> validResponses;
	
	int lastValidId;
	
	int lastInvalidId;
	
	bool failNextRequest;
	
	std::mt19937 mt;
	
	int getInvalidId();
	
};

#endif // TESTDEV_H
