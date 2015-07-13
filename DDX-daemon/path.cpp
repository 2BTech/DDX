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

Path::Path(Daemon *parent, const QString name, const QByteArray model) : QObject(parent)
{
	this->name = name;
	daemon = parent;
	isReady = false;
	isRunning = false;
	this->model = model;
	lastInitIndex = 0;
	
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
	for (int i = 0; i < modules->size(); i++)
		if (QString::compare(modules->at(i)->getName(), name, Qt::CaseInsensitive) == 0)
			return modules->at(i);
	return 0;
}

QJsonObject Path::publishSettings() const {
	QJsonObject s;
	for (int i = 0; i < modules->size(); i++)
		s.insert(modules->at(i)->getName(), modules->at(i)->publishSettings());
	return s;
}

QJsonObject Path::publishActions() const {
	QJsonObject a;
	for (int i = 0; i < modules->size(); i++)
		a.insert(modules->at(i)->getName(), modules->at(i)->publishActions());
	return a;
}

void Path::init() {
	// Parse model
	QJsonParseError pe;
	QJsonDocument modelDoc = QJsonDocument::fromJson(model, &pe);
#ifdef PATH_PARSING_CHECKS
	if (pe.error != QJsonParseError::NoError) {
		alert(tr("Model failed to parse, reported: '%1'").arg(pe.errorString()));
		terminate();
		return;
	}
	if ( ! modelDoc.isArray()) {
		alert(tr("Model is not a JSON array"));
		terminate();
		return;
	}
	QStringList moduleNameList;  // For duplicate checking in loop below
#endif
	QJsonArray modelArray = modelDoc.array();
	
	// Instantiate and connect all constituent Modules
	QJsonArray::const_iterator i;
	for (i = modelArray.constBegin(); i != modelArray.constEnd(); i++) {
#ifdef PATH_PARSING_CHECKS
		if ( ! i->isObject()) {
			alert(tr("Model contains a member which is not a JSON object"));
			terminate();
			return;
		}
#endif
		QJsonObject obj = i->toObject();
#ifdef PATH_PARSING_CHECKS
		if ( ! obj.contains("n") || ! obj.contains("t")) {
			alert(tr("Model contains a member which does not have a name or type"));
			terminate();
			return;
		}
#endif
		QString n = obj.value("n").toString();
		QString t = obj.value("t").toString();
#ifdef PATH_PARSING_CHECKS
		if (moduleNameList.contains(n, Qt::CaseInsensitive)) {
			alert(tr("Path contains multiple modules named '%1'").arg(n));
			terminate();
			return;
		}
		if ( ! daemon->um->moduleExists(t)) {
			alert(tr("Model requests module of type '%1', which does not exist").arg(t));
			terminate();
			return;
		}
#endif
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		alert("loop 1");
	}
	
	
	
	
	
	
	
	
	
	
	// Set name and register it
	/*for (int i = 0; i < modules->size(); i++) {
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
	emit ready();
	model.clear();  // Save memory; we don't need this anymore
	alert("finished init");
}

void Path::start() {
	if ( ! isReady) {
		alert(tr("Path started before it was ready"));
		return;
	}
	// TODO
	emit running();
}

void Path::stop() {
	// TODO
	// e
}

void Path::cleanup() {
	// TODO
	for (int i = 0; i < modules->size(); i++)
		modules->at(i)->cleanup();
	emit readyForDeletion();
}

void Path::terminate() {
	alert(tr("This is fatal; terminating path"));
	for (int i = 0; i < lastInitIndex; i++)
		modules->at(i)->cleanup();
	emit readyForDeletion();
	return;
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
