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

#include "testdev.h"
#include "devmgr.h"

TestDev::TestDev(DevMgr *dm, bool inbound) : RemDev(dm, inbound) {
	eventCt = 0;
}

TestDev::~TestDev() {
	
}

void TestDev::sub_init() noexcept {
	QTimer *timer = new QTimer(this);
	timer->setTimerType(Qt::CoarseTimer);
	timer->setInterval(3000);
	connect(timer, &QTimer::timeout, this, &TestDev::timeout);
	timer->start();  // Start immediately for registration timeout
}

void TestDev::terminate(DisconnectReason reason, bool fromRemote) noexcept {
	
}

void TestDev::writeItem(const char *data) noexcept {
	QString out(tr("Sent: "));
	out.append(data);
	log(out);
}

void TestDev::timeout() {
	eventCt++;
	
	char *data = new char[1000];
	
	if (eventCt == 1) {
		log(tr("[test] sending bad data"));
		strcpy(data, "this means nothing");
		handleItem(data);
	}
}
