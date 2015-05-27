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

#include "daemon.h"

Daemon::Daemon(QObject *parent) : QObject(parent) {
	// Load command line arguments
	args = ((QCoreApplication*) parent)->arguments();
	
	// Open stdout stream for logging
	qout = new QTextStream(stdout);
}


Daemon::~Daemon() {

}


void Daemon::log(const QVariant &msg) {
	QString finalMsg = QDateTime::currentDateTime().toString("[yyyy/MM/dd HH:mm:ss.zzz] ");
	finalMsg.append(msg.toString());
#ifdef LOGGING_ENABLE_STDOUT
	*qout << finalMsg << endl;
#endif
}

/*!
 * \brief Daemon::init
 * Initializes the daemon.  This includes:
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
void Daemon::init() {
	// Load settings
	settings = new QSettings(parent());
	if ( ! settings->contains("SettingsResetOn")
		 || args.contains("-reconfigure")) loadDefaultSettings();
	else log(QString("Loading settings last reset on ").append(settings->value("SettingsResetOn").toString()));

	// Determine whether log should be saved to file
	if (args.contains("-l") || settings->value("logging/AlwaysLogToFile").toBool()) {
		// TODO
		// Also, consider adding the option to show the console rather than hide it
	}
	
	// Check for other daemon instances

	// Load and unload the instrument specification file to test it

	// Look for open GUI instance

	// Check for updates 
	// (once a week, also for instrument specifications, even if app is running,
	// figure that out)

	// Set up email notifications?  Twitter uploading?
	
	// Set up as system service (platform-dependent)

	// Try connecting to instruments
}


void Daemon::loadDefaultSettings() {
	log("Loading default settings");
	settings->setValue("SettingsResetOn",
					   QDateTime::currentDateTime().toString("yyyy/MM/dd HH:mm:ss"));
	
	// Logging
	settings->setValue("logging/AlwaysLogToFile", false);
	
	settings->sync();
	// TODO: Emit a "settings changed" signal
}
