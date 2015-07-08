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
	
	/* From Module constructor
	// Set name and register it
	QJsonObject::const_iterator found = model.find("n");
	if (found == model.end()) name = QString();
	else name = found.value().toString();
	if (name.isEmpty()) {
		name = path->getDefaultModuleName();
		alert(tr("Module of type '%1'' has no name specified, using '%2'")
			  .arg(this->metaObject()->className(), name));
	}
	if ( ! path->registerModule(this, name)) {
		QString oldName(name);
		name = path->getDefaultModuleName();
		alert(tr("Path has multiple modules with name '%1', using '%2'")
			  .arg(oldName, name));
		path->registerModule(this, name);
	}
	*/
}

Path::~Path()
{
	
}


void Path::init() {
	/*qDebug(QString("Ideal threads:").append(QString::number(QThread::idealThreadCount())).toLatin1());
	t1 = new QThread(this);
	t2 = new QThread(this);
	qDebug("Initiating Inlet");
	in = new Inlet();
	in->moveToThread(t1);
	in->init();
	qDebug("Initiating Outlet");
	out = new Outlet();
	out->moveToThread(t2);
	out->init();
	
	connect(t2, SIGNAL(started()), out, SLOT(run()));
	connect(out, SIGNAL(finished()), t2, SLOT(quit()));
	connect(out, SIGNAL(finished()), out, SLOT(deleteLater()));
	connect(t2, SIGNAL(finished()), t2, SLOT(deleteLater()));
	t2->start();
	
	connect(t1, SIGNAL(started()), in, SLOT(run()));
	connect(in, SIGNAL(finished()), t1, SLOT(quit()));
	connect(in, SIGNAL(finished()), in, SLOT(deleteLater()));
	connect(t1, SIGNAL(finished()), t1, SLOT(deleteLater()));
	t1->start();*/
	
	// TODO:  add/implement ready() and start() slots, add vector for storing QThread*s
}
