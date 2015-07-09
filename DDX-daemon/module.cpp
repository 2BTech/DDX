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

Module::Module(Path *parent, const QString name) : QObject(parent)
{
	path = parent;
	this->name = name;
	/* This is safe to instantiate in the constructor because it has no events.
	 * Moving a Module to a separate thread should not harm anything. */
	outputColumns = new DataDef();
	newColumns = 0;
}

Module::~Module()
{
	delete outputColumns;
	if (newColumns) {
		emptyNewColumns();
		delete newColumns;
	}
}

void Module::init(const QJsonObject settings) {
	alert("init() not reimplemented!");
	settings.count();  // Suppress unused warning
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
	return QJsonObject();  // Return no settings
}

void Module::reconfigure() {
	if (newColumns) emptyNewColumns();
	*outputColumns = *inputColumns;
	handleReconfigure();
}

inline void Module::alert(const QString msg) const {
	path->alert(msg, this);
}

Column* Module::findColumn(const QString name) const {
	for (int i = 0; i < outputColumns->size(); i++)
		if (QString::compare(outputColumns->at(i)->n, name, Qt::CaseInsensitive) == 0)
			return outputColumns->at(i);
	return 0;
}

QString* Module::insertColumn(const QString name, int index) {
	if (findColumn(name)) return 0;
	if ( ! newColumns) newColumns = new DataDef();
	Column *c = new Column(name, this);
	newColumns->append(c);
	outputColumns->insert(index, c);
	return c->buffer();
}

void Module::removeColumn(const Column *c) {
	// TODO:  Test whether this even works with pointers
	outputColumns->removeAll((Column*) c);
	if (c->p == this) {
		newColumns->removeAll((Column*) c);
		delete c;
	}
}

inline void Module::emptyNewColumns() {
	for (int i = 0; i < newColumns->size(); i++)
		delete newColumns->at(i);
}
