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
#include "daemon.h"
#include "module.h"
#include "inlet.h"
#include "unitmanager.h"

Path::Path(Daemon *parent, const QString name) : QObject(parent)
{
	this->name = name;
	daemon = parent;
	isReady = false;
	isRunning = false;
	lastInitIndex = 0;
	processPosition = 1;
	modules = new QList<Module*>;
	
	connect(this, &Path::sendAlert, parent, &Daemon::receiveAlert);
	// TODO:  Check the validity of this
	connect(this, &Path::readyForDeletion, this, &Path::deleteLater);
}

Path::~Path()
{
	// TODO
	alert("PATH DESTROYED");
}

Module* Path::findModule(QString name) const {
	for (int i = 0; i < modules->size(); ++i)
		if (QString::compare(modules->at(i)->getName(), name, Qt::CaseInsensitive) == 0)
			return modules->at(i);
	return 0;
}

QJsonObject Path::publishSettings() const {
	QJsonObject s;
	for (int i = 0; i < modules->size(); ++i)
		s.insert(modules->at(i)->getName(), modules->at(i)->publishSettings());
	return s;
}

QJsonObject Path::publishActions() const {
	QJsonObject a;
	// TODO:  Append start and stop actions????
	for (int i = 0; i < modules->size(); ++i)
		a.insert(modules->at(i)->getName(), modules->at(i)->publishActions());
	return a;
}

void Path::terminate(QString msg) {
	alert(msg);
	alert(tr("This is fatal; terminating path"));
	cleanup();
}

void Path::process() {
	if ( ! isRunning) {
		alert("DDX bug: process() called while not running");
		return;
	}
	QList<Module*>::const_iterator it = modules->constBegin();
	processPosition = 1;
	for (++it; it < modules->constEnd(); ++it) {  // Start after the inlet
		processPosition++;
		(*it)->process();
	}
	processPosition = 1;
	QCoreApplication::processEvents(QEventLoop::AllEvents, POSTPROCESS_EVENT_HANDLING_TIMEOUT);
}

void Path::reconfigure() {
	if ( ! isReady) {
		alert("DDX bug: reconfigure() called while not running");
		return;
	}
	for (int i = processPosition; i < modules->size(); ++i)
		modules->at(i)->reconfigure();
}

void Path::test() {
	
}

void Path::init() {
	// Get scheme
	UnitManager *um = daemon->getUnitManager();
	if ( ! um) {
		terminate("DDX bug: Path initialized with no UnitManager in place");
		return;
	}
	alert("um exists");
	QByteArray scheme = um->getPathScheme(name);
#ifdef PATH_PARSING_CHECKS
	QString parseError = um->verifyPathScheme(scheme);
	if (parseError.isNull()) {
		terminate(tr("Path failed scheme verification, reported '%1'").arg(parseError));
		return;
	}
#endif
	// Parse scheme
	QJsonDocument schemeDoc = QJsonDocument::fromJson(scheme);
	
	// Instantiate and connect all constituent Modules
	QJsonArray schemeArray = schemeDoc.array();
	QJsonArray::const_iterator i;
	for (i = schemeArray.constBegin(); i != schemeArray.constEnd(); ++i) {
		QJsonObject obj = i->toObject();
		QString n = obj.value("n").toString();
		QString t = obj.value("t").toString();
		modules->append(um->constructModule(t, this, n));
		
		
		
		
		
		
		
		
		
		
		
		
		alert("loop 1");
	}
	
	
	
	
	
	
	
	
	
	// Set name and register it
	/*for (int i = 0; i < modules->size(); ++i) {
		QJsonObject::const_iterator found = model.find("n");
		if (found == model.end()) name = QString();
		else name = found.value().toString();
		if (name.isEmpty()) {
			name = path->getDefaultModuleName();
			alert(tr("Module of type '%1' has no name specified, using '%2'")
				  .arg(this->metaObject()->className(), name));
		}
		if ( ! path->registerModule(this, name)) {
			QString oldName(name);
			name = path->getDefaultModuleName();
			alert(tr("Path has multiple modules with name '%1', using '%2'")
				  .arg(oldName, name));
			path->registerModule(this, name);
		}
		lastInitIndex++;
	}*/
	
	// TODO:  Wait for all requested Beacons to be ready before continuing??
	emit ready(name);
	alert("finished init");
}

void Path::start() {
	if ( ! isReady) {
		alert(tr("Path started before it was ready"));
		return;
	}
	// TODO
	emit running(name);
}

void Path::stop() {
	// TODO
	// e
}

void Path::cleanup() {
	// TODO
	for (int i = 0; i < lastInitIndex; ++i)
		modules->at(i)->cleanup();
	for (int i = 0; i < modules->size(); ++i)
		delete modules->at(i);
	delete modules;
	// TODO: remove
	emit readyForDeletion();
}

void Path::alert(const QString msg, const Module *m) const {
	// Start with Path name
	QString out(name);
	// Add Module name if applicable
	if (m) out.append(":").append(m->getName());
	// Append & send
	out.append(": ").append(msg);
	emit sendAlert(out);
}
