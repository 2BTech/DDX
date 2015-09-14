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

#ifndef PATHMANAGER_H
#define PATHMANAGER_H

#include <QObject>
#include <QJsonObject>
#include <QReadWriteLock>
#include <QHash>

class Daemon;
class Logger;
class Settings;
class Module;
class Inlet;
class Path;

/*!
 * \brief Manages the instantiation and configuration of Modules, Beacons, and Paths
 * 
 * ## %Module and %Beacon Registration
 * Modules and Beacons must be registered in order to be used.  Registration
 * entails these steps:
 * - Subclassing the parent class and putting it in its respective source folder
 * ([source root]/modules or [source root]/beacons)
 * - Adding an entry in the object registration function (see the documentation on these
 * functions for more specific instructions)
 *   - For Modules: registerModules() in modules/module_register.cpp
 *   - For Beacons: registerBeacons() in beacons/beacon_register.cpp
 * - Adding an entry in the metadata registration function (see the documentation on these
 * functions for more specific instructions)
 *   - For Modules: getModuleList() in modules/module_register.cpp
 *   - For Beacons: getBeaconList() in beacons/beacon_register.cpp
 * 
 * Failing to register your Modules and Beacons properly can cause them to not
 * be seen by the UnitManager or can crash the application.
 * 
 * \ingroup daemon
 */
class PathManager : public QObject
{
	Q_OBJECT
public:
	
	explicit PathManager(Daemon *parent);
	
	~PathManager();
	
	QByteArray getPathScheme(QString name) const;
	
	/*!
	 * \brief Tests the validity of a Path scheme
	 * \param scheme The scheme to be tested
	 * \return An error string or null QString if there was no error
	 * 
	 * This is a slow, heavy-duty verification function which attempts to catch
	 * scheme formatting errors that could lead to a crash but which aren't
	 * necessary during most DDX standard operation.
	 */
	QString verifyPathScheme(const QByteArray scheme) const;
	
	/*!
	 * \brief Add a path to the manager
	 * \param scheme The path's scheme (will be verified)
	 * \param save Whether to save the Path or not
	 * \return The verification error string or a null QString on success
	 */
	QString addPath(const QByteArray scheme, bool save);
	
	/*!
	 * \brief Check that a Module type exists
	 * \param type The name of the Module subclass
	 * \return Whether it exists
	 */
	bool moduleExists(const QString type) const;
	
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
	
	void handleRpcRequest(const QJsonValue &id, const QString &method, const QJsonValue &params);
	
	/*!
	 * \brief Get a list of Modules
	 * \return JSON list of Modules with descriptions and settings
	 */
	QJsonObject getModuleList() const;
	
signals:
	
public slots:
	
private:
	Daemon *d;  //!< Convenience pointer to Daemon instance
	Logger *lg;  //!< Convenience pointer to Logger instance
	Settings *sg;  //!< Convenience pointer to Settings instance
	
	static QReadWriteLock configFileLock;
	
	bool schemeFileNeedsRewriting;
	
	//! The list of Modules used to instantiate them by name
	QHash<QString, QMetaObject> modules;

	/*!
	 * \brief Register all Modules with UnitManager
	 * \return The list of Modules to register
	 * 
	 * ## Registration Instructions
	 * TODO
	 */
	void registerModules();
	
	/*!
	 * \brief Return a list of all Modules and their descriptions
	 * \return The Modules and their descriptions
	 * 
	 * ## Registration Instructions
	 * TODO
	 */
	QMap<QString, QString> getModuleDescriptions() const;
};

#endif // PATHMANAGER_H
