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
#include "path.h"
#include "rapidjson_using.h"

Module::Module(Path *parent, const QByteArray &name) : QObject(parent)
{
	path = parent;
	this->name = name;
	/* This is safe to instantiate in the constructor because it is only a data
	 * class.  Moving a Module to a separate thread should not harm anything. */
	newColumns = 0;
}

Module::~Module()
{
	alert("MODULE DESTROYED");
	if (newColumns) {
		emptyNewColumns();
		delete newColumns;
	}
}

void Module::init(rapidjson::Value &config) {
	alert("DDX bug: init() not reimplemented!");
	(void) config;
}

void Module::handleReconfigure() {
	alert("DDX bug: handleReconfigure() not reimplemented!");
}

void Module::cleanup() {
}

rapidjson::Value Module::publishSettings(rapidjson::MemoryPoolAllocator<> &a) const {
	return Value(kArrayType);  // Return no settings
}

rapidjson::Value Module::publishActions(rapidjson::MemoryPoolAllocator<> &a) const {
	return Value(kArrayType);  // Return no actions
}

void Module::reconfigure() {
	if (newColumns) emptyNewColumns();
	outputColumns = *inputColumns;
	handleReconfigure();
}

void Module::alert(const QString msg) const {
	path->alert(msg, this);
}

void Module::log(const QString msg) const {
	path->log(msg, this);
}

Column* Module::findColumn(const QString name) const {
	for (int i = 0; i < outputColumns.size(); ++i)
		if (QString::compare(outputColumns.at(i)->n, name, Qt::CaseInsensitive) == 0)
			return outputColumns.at(i);
	return 0;
}

Column *Module::insertColumn(const QString name, int index) {
	if (findColumn(name)) return 0;
	if ( ! newColumns) newColumns = new DataDef();
	Column *c = new Column(name, this);
	newColumns->append(c);
	outputColumns.insert(index, c);
	return c;
}

void Module::removeColumn(const Column *c) {
	outputColumns.removeOne((Column*) c);
	if (c->p == this) {
		newColumns->removeOne((Column*) c);
		delete c;
	}
}

void Module::terminate(const QString msg) {
	alert(msg);
	path->terminate();
}

inline void Module::emptyNewColumns() {
	qDeleteAll(*newColumns);
	newColumns->clear();
}
