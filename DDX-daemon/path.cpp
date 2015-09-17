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
#include "pathmanager.h"
#include "logger.h"

Path::Path(Daemon *daemon, const QByteArray &name, const QByteArray &scheme) : QObject(0)
{
	state = State::Initializing;
	this->name = name;
	this->scheme = scheme;
	d = daemon;
	lg = Logger::get();
	lastInitIndex = 0;
	processPosition = 0;
	
	// TODO:  Check the validity of this:
	// Threading
	QThread *t = new QThread(daemon);
	moveToThread(t);
	connect(t, &QThread::started, this, &Path::init);
	connect(this, &Path::destroyed, t, &QThread::quit);
	connect(t, &QThread::finished, t, &QThread::deleteLater);
	t->start();
}

Path::~Path()
{
	// TODO
	alert("PATH DESTROYED");
}

Module* Path::findModule(QString name) const {
	for (int i = 0; i < modules.size(); ++i)
		if (QString::compare(modules.at(i)->getName(), name, Qt::CaseInsensitive) == 0)
			return modules.at(i);
	return 0;
}

QJsonObject Path::publishSettings() const {
	QJsonObject s;
	for (int i = 0; i < modules.size(); ++i)
		s.insert(modules.at(i)->getName(), modules.at(i)->publishSettings());
	return s;
}

QJsonObject Path::publishActions() const {
	QJsonObject a;
	// TODO:  Append start and stop actions????
	for (int i = 0; i < modules.size(); ++i)
		a.insert(modules.at(i)->getName(), modules.at(i)->publishActions());
	return a;
}

void Path::terminate() {
	if (state != State::Initializing) {
		alert("DDX bug:  Something tried to terminate outside of init()");
		return;
	}
	// TODO:  Ensure this is safe to use here
	d->releaseUnitManager();
	state = State::Terminated;
	alert(tr("This is fatal; terminating path"));
	cleanup();
}

void Path::moduleReady(Module *m) {
	processPosition++;
	if (processPosition == modules.size()) {
		processPosition = 1;
		emit ready(this);
	}
}

void Path::test(QString methodName) {
	methodName.size();
}

void Path::init() {
	PathManager *um = d->getUnitManager();
	// Parse scheme
#ifdef PATH_PARSING_CHECKS
	QString parseError = um->verifyPathScheme(scheme);
	if ( ! parseError.isNull()) {
		alert(tr("Path failed scheme verification, reported '%1'").arg(parseError));
		terminate();
		return;
	}
#endif
	QJsonDocument schemeDoc = QJsonDocument::fromJson(scheme);
	scheme.clear();
	
	// Instantiate and connect all constituent Modules
	QJsonArray schemeArray = schemeDoc.array();
	
	for (QJsonArray::const_iterator it = schemeArray.constBegin(); it != schemeArray.constEnd(); ++it) {
		QJsonObject obj = it->toObject();
		QString n = obj.value("n").toString();
		QString t = obj.value("t").toString();
		modules.append(um->constructModule(t, this, n));
		
		
		
		
		
		
		
		
		
		
		
		alert("loop 1");
	}
	
	// TODO:  Make sure this is safe here rather than at the end of the function
	d->releaseUnitManager();
	
	for (lastInitIndex = 0; lastInitIndex < modules.size(); ++lastInitIndex) {
		modules.at(lastInitIndex)->init(QJsonObject());
		if (state == State::Terminated) return;
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
	
	// Send initial reconfigure
	reconfigure();
	state = State::Ready;
	emit ready(name);
	alert("finished init");
}

void Path::start() {
	if (state != State::Ready) {
		alert(tr("Path started before it was ready"));
		return;
	}
	state = State::Running;
	emit running(name);
	inlet->start();
}

void Path::stop() {
	inlet->stop();
	state = State::Ready;
	emit stopped(name);
}

void Path::cleanup() {
	// TODO
	for (int i = 0; i < lastInitIndex; ++i)
		modules.at(i)->cleanup();
	for (int i = 0; i < modules.size(); ++i)
		delete modules.at(i);
	// TODO: remove
	//emit readyForDeletion();
	deleteLater();
}

void Path::reconfigure() {
#ifdef CAUTIOUS_CHECKS
	if (state != State::Running) {
		alert("DDX bug: reconfigure() called while not running");
		return;
	}
#endif
	for (int i = processPosition; i < modules.size(); ++i)
		modules.at(i)->reconfigure();
}

void Path::process() {
#ifdef CAUTIOUS_CHECKS
	if (state != State::Running) {
		alert("DDX bug: process() called while not running");
		return;
	}
#endif
	ModuleList::const_iterator it = modules.constBegin();
	processPosition = 1;
	for (++it; it < modules.constEnd(); ++it) {  // Start after the inlet
		processPosition++;
		(*it)->process();
	}
	processPosition = 1;
}

void Path::alert(const QString msg, const Module *m) const {
	// Start with Path name
	QString out(name);
	// Add Module name if applicable
	if (m) out.append(":").append(m->getName());
	// Append & send
	out.append(": ").append(msg);
	lg->log(out, true);
}

void Path::log(const QString msg, const Module *m) const {
	// Start with Path name
	QString out(name);
	// Add Module name if applicable
	if (m) out.append(":").append(m->getName());
	// Append & send
	out.append(": ").append(msg);
	lg->log(out);
}
