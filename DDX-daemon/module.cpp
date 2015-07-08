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

#include "module.h"

void Module::init(const QJsonObject settings) {
	alert("init() not reimplemented!");
}

void Module::handleReconfigure() {
	alert("handleReconfigure() not reimplemented!");
}


void Module::process() {
	alert("process() not reimplemented!");
}

void Module::cleanup() {
}

QJsonObject Module::publishSettings() {
	return QJsonObject();
}

Module::Module(const QJsonObject model, Path *parent) : QObject(parent)
{
	path = parent;
	
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
	
	// Call init
	found = model.find("s");
	if (found == model.end()) init(QJsonObject());
	else (init(found.value().toObject()));
}

Module::~Module()
{
	delete outputColumns;
	for (int i = 0; i < newColumns->size(); i++)
		delete newColumns->at(i);
	delete newColumns;
}

void Module::reconfigure() {
	newColumns->clear();
	*outputColumns = *inputColumns;
	
	handleReconfigure();
}

void Module::alert(QString msg) {
	QString out;
	if (path) out.append(path->getName()).append(":");
	out.append(name).append(": ");
	out.append(msg);
	emit sendAlert(out);
}

Column *Module::findColumn(QString name) const {
	for (int i = 0; i < outputColumns->size(); i++)
		if (QString::compare(outputColumns->at(i)->n, name, Qt::CaseInsensitive) == 0)
			return outputColumns->at(i);
	return 0;
}

QString* Module::insertColumn(QString name, int index) {
	if (findColumn(name)) return 0;
	Column *c = new Column(name, this);
	newColumns->append(c);
	outputColumns->insert(index, c);
	return c->buffer();
}

void Module::removeColumn(const Column *c) {
	// TODO:  Test whether this even works with pointers
	outputColumns->removeAll((Column*) c);
	if (c->p == this)
		newColumns->removeAll((Column*) c);
}
