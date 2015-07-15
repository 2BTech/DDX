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

#ifndef PATH_H
#define PATH_H

#include <QObject>
#include <QString>
#include <QList>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

class Module;
class Inlet;
class Daemon;
class UnitManager;

/*!
 * \brief A complete string of consecutive Modules which handles data lines
 * 
 * ## Testing %Path Configurations
 * Paths can be built by an external configuration wizard in "test mode".  In
 * test mode, alerts are redirected to a text stream, then Module::init() is
 * called on every Module followed by the initial reconfigure.  This allows
 * users to test the structure of their configurations before a Path goes live.
 * The Module::init() and Module::handleReconfigure() functions should catch as
 * many errors as possible.
 * 
 * ## Threading Information
 * Every Path runs in its own thread.  Modules written to communicate across
 * Paths must take this into account.  Beacons will also be run in their own
 * threads.  Any Module can start its own thread, but all functions called by a
 * Path assume synchronicity.
 */
class Path : public QObject
{
	Q_OBJECT
public:
	
	explicit Path(Daemon *parent, const QString name, const QByteArray scheme);
	
	~Path();
	
	/*!
	 * \brief Retreive a Module pointer by name via slow linear search
	 * \param name The name of the Module
	 * \return Pointer to a Module, or 0 if none found 
	 * 
	 * Note that this is a case-insensitive search.
	 */
	Module* findModule(QString name) const;
	
	QJsonObject publishSettings() const;
	
	QJsonObject publishActions() const;
	
	/*!
	 * \brief Terminate this Path prior to starting
	 * 
	 * In the event of a configuration error which truly prevents a Module
	 * from functioning, it can be caught in Module::init() and handled by
	 * calling this function.
	 */
	void terminate();
	
	/*!
	 * Execute the processing loop once
	 * 
	 * This function must _only_ be called by a Path's Inlet and while the Path
	 * is running.  It loops through all Modules and calls Module::process().
	 */
	void process();
	
	/*!
	 * Reconfigure downstream Modules
	 * 
	 * This function must _only_ be called by a Module's Module::process()
	 * function and while the Path is running.  All Modules after the Module
	 * currently in a process call will have Module::reconfigure() called in
	 * order.  The currently active Module will be skipped.
	 */
	void reconfigure();
	
	void test(QString methodName);
	
	/*!
	 * \brief Echo a statement to all logging Beacons.
	 * \param msg The message
	 * \param m The Module from which it comes; ignore when writing Path alerts
	 * 
	 * Alerts are tagged with the name of the Path they come from.
	 */
	void alert(const QString msg, const Module *m = 0) const;
	
	/*!
	 * \brief Retrieve Module list
	 * \return The list of Modules
	 * 
	 * Can be used so that Modules can search for companion Modules by type.
	 */
	const QList<Module*>* getModules() const {return &modules;}
	
	/*!
	 * \brief Get the Path's name
	 * \return The Path's name
	 */
	QString getName() const {return name;}
	
signals:
	
	//! Emitted when Path is ready to start
	void ready(QString path) const;
	
	//! Emitted when Path has started
	void running(QString path) const;
	
	//! Emitted when Path has reached the end of its inlet stream
	void finished(QString path) const;
	
	//! Emitted when a new set of live actions is available
	void updateLiveActions(QJsonObject actions) const;
	
	//! Emitted when all cleanup operations have finished
	void readyForDeletion() const;
	
	//! Emitted to send an alert.  _Note:_ use alert() when sending alerts.
	void sendAlert(const QString msg) const;
	
public slots:
	/*!
	 * Parse scheme and Module::init() constituents
	 * 
	 * Note: Daemon::um _must_ be valid while this method runs!
	 * 
	 * ### Parsing
	 * The scheme is parsed.  
	 */
	void init();
	void start();
	void stop();
	void cleanup();  // Or shutdown?
	
protected:
	
private:
	//! This Path's name (not editable after construction)
	QString name;
	
	//! This Path's scheme (will be cleared after initialization)
	QByteArray scheme;
	
	//! The ordered Module list
	QList<Module*> modules;
	
	//! Convenience pointer to Inlet
	Inlet *inlet;
	
	//! Convenience pointer to parent daemon
	Daemon *daemon;
	
	//! Whether the Path is ready to start
	bool isReady;
	
	//! Whether the Path is currently running
	bool isRunning;
	
	//! Keeps track of which modules have been initiated in case of termination
	int lastInitIndex;
	
	//! Manages the current processing index for return after reconfiguration
	int processPosition;
	
	//! Used to break out of configuration loops in init() if terminated
	bool terminated;
};

#endif // PATH_H
