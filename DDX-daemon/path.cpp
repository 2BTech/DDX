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

Path::Path(Daemon *parent, const QString name, const QByteArray model) : QObject(parent)
{
	this->name = name;
	isReady = false;
	isRunning = false;
	rawModel = model;
	
	connect(this, &Path::sendAlert, parent, &Daemon::receiveAlert);
	// TODO:  Check the validity of this
	connect(this, &Path::finished, this, &Path::deleteLater);
}

Path::~Path()
{
	// TODO
	// Especially check if it's okay to call QThread::finished and QThread::quit
	// more than once - if not, ensure that finished() is only emitted once
	emit finished();
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
	QJsonParseError *pe = 0;
	QJsonDocument modelDoc = QJsonDocument::fromJson(rawModel, pe);
	if (pe->error != QJsonParseError::NoError) {
		alert(tr("Path model failed to parse, reported: '%1'").arg(pe->errorString()));
		alert(tr("This is fatal; terminating path"));
		// TODO:  Write up documentation to let it know that this can happen during init()
		emit finished();
		return;
	}
	if ( ! modelDoc.isObject()) {
		alert(tr("Path model is not a JSON object"));
		alert(tr("This is fatal; terminating path"));
		// TODO:  Write up documentation to let it know that this can happen during init()
		emit finished();
		return;
	}
	model = modelDoc.object();
	rawModel.clear();  // Save memory
	
	// TODO:  Make it so that 
	
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
	}*/
	emit ready();
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
	emit finished();
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
