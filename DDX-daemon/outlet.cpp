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

#include "outlet.h"

Outlet::Outlet(QObject *parent) : QObject(parent)
{
	
}

Outlet::~Outlet()
{
	
}


void Outlet::init() {
	// Setup the incoming buffer and stream
	buffer = new QByteArray(DEFAULT_SLOW_BUFFER_SIZE, '\0');
	inStream = new QTextStream(buffer, QIODevice::ReadOnly);
}


void Outlet::run() {
	for (int i=0; i<5; i++) {
		qDebug("Outlet");
		QTest::qWait(800);
	}
}


QTextStream* Outlet::getStreamIn() {
	return 0;
}


void Outlet::publishMessage(QString msg) {
	//qDebug("goody");
}
