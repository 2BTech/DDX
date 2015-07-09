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

#ifndef MODULE_REGISTER_CPP
#define MODULE_REGISTER_CPP

#include "../module.h"

#include "genmod.h"

QList<QMetaObject> UnitManager::registerModules() {
	QList<QMetaObject> m;
	
	// List all Modules here
	m.append(GenMod::staticMetaObject);
	
	return m;
}

QJsonObject UnitManager::getModuleList() const {
	QJsonObject l;
	
	l.insert("GenMod", tr("General modifications (TODO)"));
	
	return l;
}

#endif // MODULE_REGISTER_CPP
