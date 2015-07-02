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
#include <QCoreApplication>  // For loading command line arguments
#include <QDateTime>
#include <QSettings>
#include <QSystemTrayIcon>	// Tray icon
#include <QIcon>
#include <QMenu>			// Tray icon
#include "../DDX-gui/constants.h"
#include "path.h"

/*!
 * \brief The main manager of the DDX
 * 
 * Instantiating this class and calling init() will begin DDX operation.
 */
class Daemon : public QObject
{
	Q_OBJECT
public:
	explicit Daemon(QObject *parent = 0);
	~Daemon();
	
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
	 * This function is scheduled to occur immediately after initial event
	 * loop processing in main().
	 */
	void init();
	
	void log(const QVariant &msg);  // Print a low-info log message
	
	void quit(int returnCode=0);
	// TODO: void report(const QVariant &msg);  // Send a message which should be saved to disk or inserted into the data log somehow
	// TODO: void notify(const QVariant &msg);  // Pump out a desktop notification and/or email notification (See snorenotify)

private:
	void loadDefaultSettings();
	void setupService();
	QTextStream *qout;  // stdout wrapper
	QSystemTrayIcon *trayIcon;
	QMenu *trayMenu;
	
	// TODO: Remove
	Path *testpath;
};


/* TODO: Add comments in this file on the error-handling philosophy.
 * # Error Handling
	 * There are a handful of solutions for managing errors without interrupting
	 * data flow.  Noncritical Modules can simply pass data through untouched.
	 * Critical modules whose actions can be postponed can insert a column with
	 * the name `DDX:<modulename>:Passed` and set it to "ok" when successful and
	 * anything besides that when unsuccessful, especially reasons why and data
	 * which may be necessary to carry out the process later.  
 *
 */
#endif // DAEMON_H
