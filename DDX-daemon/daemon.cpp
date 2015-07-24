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
#include "network.h"
#include "modules/genmod.h"
#include "settings.h"
#include "logger.h"

Daemon::Daemon(QCoreApplication *parent) : QObject(parent) {
	logReady = false;
	logger = Logger::get();
	logger->daemon = this;
	// Load command line arguments
	args = parent->arguments();
	// Initialize other variables
	settings = new Settings(this);
	//n = new Network(this);
	umRefCount = 0;
	unitManager = 0;
	nextRequestId = 1;
	quitting = false;
	// TODO: Make this work with new syntax
	connect(parent, SIGNAL(aboutToQuit()), this, SLOT(quit()));
}

Daemon::~Daemon() {
	// TODO
	if (unitManager) delete unitManager;
	delete n;
}

void Daemon::init() {
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
	//settings = new QSettings(parent());
	/*if ( ! settings->contains("SettingsResetOn")
		 || args.contains("-reconfigure")) loadDefaultSettings();
	else log(tr("Loading settings last reset on ").append(settings->value("SettingsResetOn").toString()));*/

	//! ### Network Manager Initialization
	logger->log("STARTING");
	n = new Network(this);
	logger->log("finished");
	logger->log(QString("Queue length: %1").arg(logger->q.size()));
	
	// Determine whether log should be saved to file
	//if (args.contains("-l") || settings->value("logging/AlwaysLogToFile").toBool()) {
		// TODO
		// Also, consider adding the option to show the console rather than hide it
	//}
	
	// Determine whether last instance shut down correctly
	
	// Load and unload the instrument specification file to test it
	
	
	QByteArray testScheme = "{\"n\":\"Test path\",\"d\":\"This is a test path\",\"DDX_author\":\"2B Technologies\",\"DDX_version\":\"0\",\"modules\":[{\"n\":\"Test inlet\",\"t\":\"ExampleInlet\",\"s\":{\"SampleSetting\":\"42\"}},{\"n\":\"Test module 1\",\"t\":\"ExampleModule\"},{\"n\":\"Test module 2\",\"t\":\"ExampleModule\",\"s\":{\"Flow_Rate\":\"12\",\"Analog_Inputs\":{\"items\":[{\"n\":\"Temperature Sensor\",\"t\":\"Voltage_AI\",\"Max_Voltage\":\"3.3\",\"Min_Voltage\":\"0\"},{\"n\":\"Power Meter\",\"t\":\"Current_AI\",\"Max_Current\":\"20\",\"Min_Current\":\"0\"},{\"n\":\"Barometer\",\"t\":\"Voltage_AI\",\"Max_Voltage\":\"2\",\"Min_Voltage\":\"1\"}]}}}]}";
	testScheme.size();
	
	
	// Set up as system service (platform-dependent)
	//setupService();
	
}

void Daemon::testPath(QByteArray scheme, int log) {
	// TODO
	scheme.size();
	log += 2;
}

void Daemon::addPath(QString name, int log) {
	this->logger->log("fail");
	UnitManager *um = getUnitManager();
	QByteArray scheme = um->getPathScheme(name);
	// TODO add error checking for scheme not found
	QThread *t = new QThread(this);
	Path *p = new Path(this, name, scheme);
	paths.append(p);
	p->moveToThread(t);
	connect(t, &QThread::started, p, &Path::init);
	connect(p, &Path::destroyed, t, &QThread::quit);
	connect(t, &QThread::finished, t, &QThread::deleteLater);
	t->start();
	log +=2;
	// TODO
}

void Daemon::request(QJsonObject params, QString dest, bool response) {
	if (response) {
		/* Get request ID so that responses can be recorded
		 * While this function is technically called from multiple threads, each
		 * call is queued, so Daemon data elements should be safe to access. */
		int id = nextRequestId++;
		if (nextRequestId == INT_MAX) {
			logger->log("RPC ID value maxed; resetting.  May cause undefined behavior.");
			nextRequestId = 100;  // Some random relatively low value
			/* TODO:  In future versions, it may be better to trigger and daemon
			 * restart when this happens. */
		}
		id+=2;
	}
	else {}
	params.size();
	dest.size();
}

void Daemon::quit(int returnCode) {
	if (quitting) return;  // Prevent recursion
	quitting = true;
	// TODO: make this call finishing stuff
	n->shutdown();  // May take a while
	if (returnCode) logger->log(tr("Terminating (%1)").arg(returnCode));
	else logger->log(tr("Quitting"));
	qApp->exit(returnCode);
}

UnitManager* Daemon::getUnitManager() {
	if ( ! unitManager) unitManager = new UnitManager(this);
#ifndef KEEP_UNITMANAGER
	logger->log("Unit manager requested");
	umRefCount++;
#endif
	return unitManager;
}

void Daemon::releaseUnitManager() {
#ifndef KEEP_UNITMANAGER
	log ("Unit manager released");
	if ((--umRefCount < 1) && unitManager) {
		log ("DELETING UNITMANAGER");
		delete unitManager;
		log ("    ok");
	}
	if (umRefCount < 0) umRefCount = 0;
#endif
}

int Daemon::versionCompare(QString testVersion) {
	const QString currentVersion = QString::fromLatin1(VERSION_FULL_TEXT);
	QStringList current = currentVersion.split('.');
	QStringList test = testVersion.split(QChar('.'));
	if (current.size() != 2 || test.size() != 2)
		return 10;
	int cMajor, cMinor, tMajor, tMinor;
	bool ok;
	cMajor = current[0].toInt(&ok);
	if ( ! ok) return 10;
	cMinor = current[1].toInt(&ok);
	if ( ! ok) return 10;
	tMajor = test[0].toInt(&ok);
	if ( ! ok) return 10;
	tMinor = test[1].toInt(&ok);
	if ( ! ok) return 10;
	if (cMajor > tMajor) return -1;
	if (cMajor < tMajor) return 1;
	if (cMinor > tMinor) return -1;
	if (cMinor < tMinor) return 1;
	return 0;
}

void Daemon::loadDefaultSettings() {
	logger->log("Loading default settings");
	/*settings->clear();
	settings->setValue("SettingsResetOn",
					   QDateTime::currentDateTime().toString("yyyy/MM/dd HH:mm:ss"));
	
	// Paths
	// Paths must be terminated by a '/'
	// TODO
	settings->setValue("paths/configPath", "/");
	
	// Logging
	settings->setValue("logging/AlwaysLogToFile", false);
	
	settings->setValue("network/GUIPort", 4388);
	settings->setValue("network/AllowExternalManagement", false);
	
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
	
	settings->sync();*/
	// TODO: restart Daemon?????
}

void Daemon::setupService() {
#ifdef ICON
	log("In");
	QIcon icon(":/icons/icon32");
	log("Icon");
	
	trayMenu = new QMenu();
	trayMenu->addAction(tr("Quit"), this, SLOT(quit()));
	
	trayIcon = new QSystemTrayIcon(icon, this);
	trayIcon->setToolTip(APP_NAME_UNTRANSLATABLE);
	trayIcon->setContextMenu(trayMenu);
	trayIcon->show();
#endif
}
