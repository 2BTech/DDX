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

Path::Path(Daemon *parent) : QObject(parent)
{
	inTestMode = false;
	isRunning = false;
	connect(this, &Path::sendAlert, parent, &Daemon::receiveAlert);
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
	
	
	
-	t2 = new QThread(this);		
-	qDebug("Initiating Inlet");		
-	in = new Inlet();		
-	in->moveToThread(t1);		
-	in->init();		
-	qDebug("Initiating Outlet");		
-	out = new Outlet();		
-	out->moveToThread(t2);		
-	out->init();		
-			
-	connect(t2, SIGNAL(started()), out, SLOT(run()));		
-	connect(out, SIGNAL(finished()), t2, SLOT(quit()));		
-	connect(out, SIGNAL(finished()), out, SLOT(deleteLater()));		
-	connect(t2, SIGNAL(finished()), t2, SLOT(deleteLater()));		
-	t2->start();
	*/
}

Path::~Path()
{
	// TODO
	// Especially check if it's okay to call QThread::finished and QThread::quit
	// more than once - if not, ensure that finished() is only emitted once
	emit finished();
}

Module* Path::findModule(QString name) const {
	// TODO
	name = QString();
	return 0;
}

void Path::init() {
	// TODO
}

void Path::start() {
	// TODO
}

QString Path::getDefaultModuleName(const QString type) {
	// TODO
	int i = 0;
	QString n;
	do {
		i++;
		n = type;
		n.append(QString::number(i));
	} while (findModule(n));
	return n;
}

void Path::alert(const QString msg, const Module *m) const {
	// Start with Path name
	QString out(name);
	// Add Module name if applicable
	if (m) out.append(":").append(m->getName());
	// Append & send
	out.append(": ").append(msg);
	emit sendAlert(msg);
}
