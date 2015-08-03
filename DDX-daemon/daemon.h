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

#ifndef DAEMON_H
#define DAEMON_H

#include <QObject>
#include <QCoreApplication>
#include <QDateTime>
#include <QTimeZone>
#include <QSettings>
#ifdef ICON
	#include <QSystemTrayIcon>
	#include <QIcon>
	#include <QMenu>
#endif
#include <QThread>
#include <QList>
#include <QJsonObject>
#include <QJsonDocument>
#include "constants.h"

class Path;
class UnitManager;
class Network;
class Settings;
class Logger;

/*!
 * \brief The main manager of the DDX
 * 
 * Instantiating this class and calling init() will begin DDX operation.
 * 
 * ## Thread Structure
 * In addition to the Daemon's primary thread, every Path and Beacon gets a
 * thread to itself.
 */
class Daemon : public QObject
{
	friend class Logger;
	Q_OBJECT
public:
	explicit Daemon(QCoreApplication *parent);
	
	~Daemon();
	
	void testPath(const QByteArray &scheme, int log = 0);
	
	void addPath(const QByteArray &name, int log = 0);
	
	UnitManager *getUnitManager();
	
	void releaseUnitManager();
	
	Settings *getSettings() const {return settings;}
	
	/*!
	 * \brief Obtain the current date and time
	 * \return The current time in DDX standard form
	 * 
	 * Because most DDX use cases aren't appropriate places for DST, "DDX time"
	 * refers to the local timezone with DST disabled.  DST can be re-enabled in
	 * settings, but this is generally not recommended.
	 */
	QDateTime getTime() const;
	
	const QTimeZone *getTimezone() const {return &tz;}
	
	/*!
	 * \brief Compare a version string with this application's version
	 * \param testVersion The test version string (must be in the format "4.2")
	 * \return 0 if equivalent, -1 if testVersion is less, 1 if testVersion is more
	 * 
	 * Returns 10 on error.
	 */
	static int versionCompare(QString testVersion);
	
	QStringList args;
	
	Settings *settings;

signals:

public slots:
	
	/*!
	 * \brief Initializes the DDX daemon
	 * 
	 * This function is queued to run as soon as the main event loop starts.
	 * Responsibilities include:
	 * - Loading saved settings (or resetting to defaults)
	 * - Opening log files if necessary
	 * - Checking for other daemon instances
	 * - Testing the integrity of the instrument specification file
	 * - Looking for a GUI instance
	 * - Checking for updates to the DDX or the instrument specification file
	 * - Installing as service
	 * - Try connecting to any existing instrument profiles
	 * 
	 * This function is scheduled to occur immediately after initial event
	 * loop processing in main().
	 * 
	 * ## Planned Future Additions
	 * - Configuring network UPnP with the BRisa framework
	 * - Using bearer management to automatically configure network interfaces
	 */
	void init();
	
	void request(QJsonObject params, QString dest, bool response = false);
	
	void quit(int returnCode=0);
	
private slots:
	
private:
	
	Logger *logger;
	
	bool logReady;
	
	Network *n;
	
	//! Used to direct RPC responses to their requestor
	QHash<int, QString> responseDirector;
	
#ifdef ICON
	QSystemTrayIcon *trayIcon;
	QMenu *trayMenu;
#endif
	
	//! The list of active Paths, including Paths in testing
	QList<Path*> paths;
	
	UnitManager *unitManager;
	
	QTimeZone tz;
	
	int umRefCount;
	
	int nextRequestId;
	
	bool quitting;
	
	void loadDefaultSettings();
	
	void setupService();
	
	void publishLog(QString msg);
	
	void publishAlert(QString msg);
};

#endif // DAEMON_H
