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
#include "unitmanager.h"
#include "path.h"
#include "modules/genmod.h"

Daemon::Daemon(QCoreApplication *parent) : QObject(parent) {
	// Load command line arguments
	args = parent->arguments();
	// Open stdout stream for logging
	qout = new QTextStream(stdout);
	umRefCount = 0;
}

Daemon::~Daemon() {
	// TODO
}

void Daemon::testPath(QByteArray scheme, int log) {
	scheme.size();
	// TODO
}

void Daemon::addPath(QString name, int log) {
	UnitManager *um = getUnitManager();
	QThread *t = new QThread(this);
	Path *p = new Path(this, name);
	paths.append(p);
	p->moveToThread(t);
	connect(t, &QThread::started, p, &Path::init);
	connect(p, &Path::destroyed, t, &QThread::quit);
	connect(t, &QThread::finished, t, &QThread::deleteLater);
	t->start();
}

void Daemon::quit(int returnCode) {
	// TODO: make this call finishing stuff
	log("Daemon quit slot called");
	qApp->exit(returnCode);
}


void Daemon::log(const QVariant &msg) {
	QString finalMsg = QDateTime::currentDateTime().toString("[yyyy/MM/dd HH:mm:ss.zzz] ");
	finalMsg.append(msg.toString());
#ifdef LOGGING_ENABLE_STDOUT
	*qout << finalMsg << endl;
#endif
}

void Daemon::init() {
	/*! ### Searching for other Daemon instances
	 * This must be done before settings are loaded so as to avoid race
	 * conditions when reading from settings.
	 */
	// TODO
	
	/*! ### Loading Settings
	 * Settings are set to their default values at startup when one of these
	 * conditions is met:
	 * - No settings have been set (determined by searching for the setting
	 *   "SettingsResetOn")
	 * - The daemon is launched with the "-reconfigure" argument
	 * 
	 * Note that the "Default Settings" GUI button simply removes
	 * "SettingsResetOn" and then forces a full application restart.
	 * \sa Daemon::loadDefaultSettings
	 */
	// TODO:  Add a see also to the above comment about the GUI's option which
	// removes "SettingsResetOn"
	settings = new QSettings(parent());
	if ( ! settings->contains("SettingsResetOn")
		 || args.contains("-reconfigure")) loadDefaultSettings();
	else log(QString("Loading settings last reset on ").append(settings->value("SettingsResetOn").toString()));

	// Determine whether log should be saved to file
	if (args.contains("-l") || settings->value("logging/AlwaysLogToFile").toBool()) {
		// TODO
		// Also, consider adding the option to show the console rather than hide it
	}
	
	// Determine whether last instance shut down correctly
	
	// Load and unload the instrument specification file to test it
	
	UnitManager *um = getUnitManager();
	QByteArray testScheme = "{\"n\":\"Test path\",\"d\":\"This is a test path\",\"DDX_author\":\"2B Technologies\",\"DDX_version\":\"0\",\"modules\":[{\"n\":\"Test inlet\",\"t\":\"ExampleInlet\",\"s\":{\"SampleSetting\":\"42\"}},{\"n\":\"Test module 1\",\"t\":\"ExampleModule\"},{\"n\":\"Test module 2\",\"t\":\"ExampleModule\",\"s\":{\"Flow_Rate\":\"12\",\"Analog_Inputs\":{\"items\":[{\"n\":\"Temperature Sensor\",\"t\":\"Voltage_AI\",\"Max_Voltage\":\"3.3\",\"Min_Voltage\":\"0\"},{\"n\":\"Power Meter\",\"t\":\"Current_AI\",\"Max_Current\":\"20\",\"Min_Current\":\"0\"},{\"n\":\"Barometer\",\"t\":\"Voltage_AI\",\"Max_Voltage\":\"2\",\"Min_Voltage\":\"1\"}]}}}]}";
	/*
	 * {
	 *   "n": "Test path",
	 *   "d": "This is a test path",
	 *   "DDX_author": "2B Technologies",
	 *   "DDX_version": "0.0",
	 *   "modules": [
	 *     {
	 *       "n": "Test inlet",
	 *       "t": "ExampleInlet",
	 *       "s": {
	 *         "SampleSetting": "42"
	 *       }
	 *     },
	 *     {
	 *       "n": "Test module 1",
	 *       "t": "ExampleModule"
	 *     },
	 *     {
	 *       "n": "Test module 2",
	 *       "t": "ExampleModule",
	 *       "s": {
	 *         "Flow_Rate": "12",
	 *         "Analog_Inputs": {
	 *           "items": [
	 *             {
	 *               "n": "Temperature Sensor",
	 *               "t": "Voltage_AI",
	 *               "Max_Voltage": "3.3",
	 *               "Min_Voltage": "0"
	 *             },
	 *             {
	 *               "n": "Power Meter",
	 *               "t": "Current_AI",
	 *               "Max_Current": "20",
	 *               "Min_Current": "0"
	 *             },
	 *             {
	 *               "n": "Barometer",
	 *               "t": "Voltage_AI",
	 *               "Max_Voltage": "2",
	 *               "Min_Voltage": "1"
	 *             }
	 *           ]
	 *         }
	 *       }
	 *     }
	 *   ]
	 * }
	 */
	log("Verifying path...");
	log(um->verifyPathScheme(testScheme));
	
	releaseUnitManager();
	
	
	Path *p = new Path(this, "TestPath");
	
	
	// Look for open GUI instance

	// Check for updates 
	// (once a week, also for instrument specifications, even if app is running,
	// figure that out)

	// Set up email notifications?  Twitter uploading?
	log("setupService");
	// Set up as system service (platform-dependent)
	//setupService();
	
	// Try connecting to instruments
	
	log("Starting path");
	p->init();
	log("Ending path");
}

UnitManager* Daemon::getUnitManager() {
	if ( ! unitManager) unitManager = new UnitManager(this);
	umRefCount++;
	return unitManager;
}

void Daemon::releaseUnitManager() {
	if (--umRefCount == 0) {
		delete unitManager;
		unitManager = 0;
	}
}

void Daemon::receiveAlert(QString msg) {
	// TODO
	msg.prepend("a:");
	log(msg);
}

void Daemon::loadDefaultSettings() {
	log("Loading default settings");
	settings->clear();
	settings->setValue("SettingsResetOn",
					   QDateTime::currentDateTime().toString("yyyy/MM/dd HH:mm:ss"));
	
	// Paths
	// Paths must be terminated by a '/'
	// TODO
	settings->setValue("paths/configPath", "/");
	
	// Logging
	settings->setValue("logging/AlwaysLogToFile", false);
	
	// Locale
	// TODO: Remove these
	settings->setValue("locale/DaemonStringsRequired", true);
	settings->setValue("locale/Quit", "Quit");
	settings->setValue("locale/StreamOkay", "OK");
	settings->setValue("locale/StreamError", "Error!");
	
	// Unit Management
	settings->setValue("units/unitFile", "units.json");
	
	// Crash checking
	settings->setValue("crash/LastShutdownSafe", false);
	
	settings->sync();
	// TODO: restart Daemon?????
}


void Daemon::setupService() {
	log("In");
	QIcon icon(":/icons/icon32");
	log("Icon");
	
	trayMenu = new QMenu();
	trayMenu->addAction(tr("Quit"), this, SLOT(quit()));
	
	trayIcon = new QSystemTrayIcon(icon, this);
	trayIcon->setToolTip(APP_NAME_UNTRANSLATABLE);
	trayIcon->setContextMenu(trayMenu);
	trayIcon->show();
}
