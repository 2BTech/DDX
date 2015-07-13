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
}

Daemon::~Daemon() {
	// TODO
}

void Daemon::addPath(QString name, QByteArray scheme) {
	
	QThread *t = new QThread(this);
	Path *p = new Path(this, name);
	paths->append(p);
	p->moveToThread(t);
	connect(t, &QThread::started, p, &Path::init);
	connect(p, &Path::finished, t, &QThread::quit);
	connect(t, &QThread::finished, t, &QThread::deleteLater);
	t->start();
	
	
	
	/*-	t2 = new QThread(this);		
	-	qDebug("Initiating Inlet");		
	-	in = new Inlet();		
	-	in->moveToThread(t1);		
	-	in->init();		
	-	qDebug("Initiating Outlet");		
	-	out = new Outlet();		
	-	out->moveToThread(t2);		
	-	out->init();		
	-			
	-	connect(t2, SIGNAL(started()), out, SLOT(run()));		
	-	connect(out, SIGNAL(finished()), t2, SLOT(quit()));		
	-	connect(out, SIGNAL(finished()), out, SLOT(deleteLater()));		
	-	connect(t2, SIGNAL(finished()), t2, SLOT(deleteLater()));		
	-	t2->start();
		*/
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
	/*!
	 * ### Loading Settings
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
	
	// Check for other daemon instances
	
	// Load and unload the instrument specification file to test it
	um = new UnitManager(this);
	Path *p = new Path(this, "testPATH");
	GenMod *gm = new GenMod(p, QString("test"));
	
	
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
	trayMenu->addAction(settings->value("locale/Quit").toString(), this, SLOT(quit()));
	
	trayIcon = new QSystemTrayIcon(icon, this);
	trayIcon->setToolTip(APP_NAME_UNTRANSLATABLE);
	trayIcon->setContextMenu(trayMenu);
	trayIcon->show();
}
