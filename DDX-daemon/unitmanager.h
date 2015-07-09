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

#ifndef UNITMANAGER_H
#define UNITMANAGER_H

#include <QObject>
#include <QJsonObject>
#include "module.h"
#include "daemon.h"

class Daemon;
class Module;
class Path;

/*!
 * \brief Manages the creation and configuration of Modules, Beacons, and Paths
 * 
 * ## Module and Beacon Registration
 * Modules and Beacons must be registered in order to be used.  Registration
 * entails these steps:
 * - Subclassing the parent class and putting it in its respective source folder
 * ([source root]/modules or [source root]/beacons)
 * - Adding an entry in the registerModules() or registerBeacons() function in
 * module_register.cpp or beacon_register.cpp (see the documentation on these
 * functions for more specific instructions)
 * - Adding an entry in the getModuleList() or getBeaconList() function also in
 * module_register.cpp or beacon_register.cpp (see the documentation on these
 * functions for more specific instructions)
 * 
 * Failing to register your Modules and Beacons properly can cause them to not
 * be seen by the UnitManager or can crash the application.
 */
class UnitManager : public QObject
{
	Q_OBJECT
public:
	
	explicit UnitManager(Daemon *parent);
	
	~UnitManager();
	
	/*!
	 * \brief Check that a Module type exists
	 * \param type The name of the Module subclass
	 * \return Whether it exists
	 */
	bool doesModuleExist(const QString type) const;
	
	/*!
	 * \brief Instantiate a Module subclass
	 * \param type Name of the Module subclass
	 * \param parent Parent path; passed to Module constructor
	 * \param name Module name; passed to Module constructor
	 * \return An instance of a Module subclass
	 * 
	 * This function returns a pointer to an instance of a Module subclass.
	 * Calling on a type which does not exist in the UnitManager's module list
	 * is undefined behavior.
	 */
	Module* constructModule(const QString type, Path *parent, const QString name) const;
	
	/*!
	 * \brief Get a list of Modules
	 * \return JSON list of Modules and their translated descriptions
	 * 
	 * ## Registration Instructions
	 * TODO
	 */
	QJsonObject getModuleList() const;
	
signals:
	
public slots:
	
private:
	QHash<QString, QMetaObject> *modules;
	QHash<QString, QMetaObject> *beacons;

	/*!
	 * \brief Register all Modules with UnitManager
	 * \return The list of Modules to register
	 * 
	 * ## Registration Instructions
	 * TODO
	 */
	QList<QMetaObject> registerModules();
};

#endif // UNITMANAGER_H
