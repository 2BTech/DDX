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

#include "path.h"

Path::Path(QObject *parent) : QObject(parent)
{
	qDebug("PATH CONSTRUCTOR");
}

Path::~Path()
{
	
}


void Path::init() {
	qDebug("PATH INIT");
	threads->append(new QThread(this));
	threads->append(new QThread(this));
	qDebug("Initiating Inlet");
	in = new Inlet();
	in->moveToThread(threads->at(0));
	in->init();
	qDebug("Initiating Outlet");
	out = new Outlet();
	out->moveToThread(threads->at(1));
	out->init();
	
	connect(threads->at(1), SIGNAL(started()), out, SLOT(run()));
	connect(out, SIGNAL(finished()), threads->at(1), SLOT(quit()));
	connect(out, SIGNAL(finished()), out, SLOT(deleteLater()));
	connect(threads->at(1), SIGNAL(finished()), threads->at(1), SLOT(deleteLater()));
	threads->at(1)->start();
	
	connect(threads->at(0), SIGNAL(started()), in, SLOT(run()));
	connect(in, SIGNAL(finished()), threads->at(0), SLOT(quit()));
	connect(in, SIGNAL(finished()), in, SLOT(deleteLater()));
	connect(threads->at(0), SIGNAL(finished()), threads->at(0), SLOT(deleteLater()));
	threads->at(0)->start();
	
	// TODO:  add/implement ready() and start() slots, add vector for storing QThread*s
}
