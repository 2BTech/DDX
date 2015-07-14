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
#include <QTextStream>  // For outputting to stdout
#include <QCoreApplication>
#include <QDateTime>
#include <QSettings>
#include <QSystemTrayIcon>	// Tray icon
#include <QIcon>
#include <QMenu>			// Tray icon
#include <QThread>
#include <QList>
#include "constants.h"

class Path;
class UnitManager;

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
	Q_OBJECT
public:
	explicit Daemon(QCoreApplication *parent);
	
	~Daemon();
	
	void addPath(QString name);
	
	UnitManager *getUnitManager() {return um;}
	
	QStringList args;
	
	QSettings *settings;

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
	 */
	void init();
	
	void receiveAlert(QString msg);
	
	void log(const QVariant &msg);  // Print a low-info log message
	
	void quit(int returnCode=0);

private:
	
	QTextStream *qout;  // stdout wrapper
	
	QSystemTrayIcon *trayIcon;
	
	QMenu *trayMenu;
	
	QList<Path*> *paths;
	
	UnitManager *um;
	
	void loadDefaultSettings();
	
	void setupService();
};

#endif // DAEMON_H
