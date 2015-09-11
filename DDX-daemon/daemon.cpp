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
#include "pathmanager.h"
#include "path.h"
#include "network.h"
#include "modules/genmod.h"
#include "settings.h"
#include "logger.h"

Daemon::Daemon(QCoreApplication *parent) : QObject(parent) {
	logReady = false;
	lg = Logger::get();
	lg->d = this;
	// Load command line arguments
	args = parent->arguments();
	// Initialize other variables
	sg = new Settings(this);
	unitManager = 0;
	quitting = false;
	utilityTimer = new QTimer(this);
	// TODO: Make this work with new syntax
	connect(parent, SIGNAL(aboutToQuit()), this, SLOT(quit()));
	QMetaObject::invokeMethod(this, "init", Qt::QueuedConnection);
}

Daemon::~Daemon() {
	// TODO
	if (unitManager) delete unitManager;
	delete n;
}

void Daemon::init() {
	
	// Ensure the current version constant is correctly formatted
	Q_ASSERT(versionCompare(QString("4.2")) != VERSION_COMPARE_FAILED);
	
	if (QString::compare(qVersion(), QT_VERSION_STR) != 0) lg->log
		(tr("The daemon was compiled to use Qt %1, but it is running on a system "
			"with Qt %2. It will continue to run, but there may be unexpected "
			"side effects. If problems occur, install the correct version of "
			"Qt.").arg(QT_VERSION_STR, qVersion()), true);
	
	// Do locale stuff here?  At least before timezone stuff
	
	// Determine timezone for DDX time
	{
		QTimeZone utc = QTimeZone(0);
		if (sg->v("ForceUTC", SG_TIME).toBool()) tz = utc;
		else {
			QByteArray requestedTzId = sg->v("Timezone", SG_TIME).toByteArray();
			if (requestedTzId != QTimeZone::systemTimeZoneId())
				lg->log(tr("System timezone '%1' does not match requested timezone '%2'; "
							   "using requested")
							.arg(QString(QTimeZone::systemTimeZoneId()), QString(requestedTzId)));
			
			QTimeZone requestedTz = QTimeZone(requestedTzId);
			if (sg->v("IgnoreDST", SG_TIME).toBool()) {
				int tzOffset = requestedTz.standardTimeOffset(QDateTime::currentDateTime());
				tz = QTimeZone(tzOffset);
			}
			else {
				if (requestedTz.hasDaylightTime()) lg->log
					(tr("Note: the DDX is not ignoring DST. Time changes will not be reported "
						"and may cause undefined behavior."));
				tz = requestedTz;
			}
			if ( ! (requestedTz.isValid() && tz.isValid())) {
				lg->log(tr("The timezone could not be established"));
				tz = utc;
			}
		}
	}
	lg->log(tr("Using timezone %1").arg(
		tz.displayName(QTimeZone::GenericTime, QTimeZone::DefaultName)));
	lg->log(tr("Current DDX time %1").arg(getTime().toString(Qt::ISODate)));

	//! ### Network Manager Initialization
	lg->log("STARTING");
	n = new Network(this);
	lg->log("finished");
	lg->log(QString("Queue length: %1").arg(lg->q.size()));
	
	// Determine whether log should be saved to file
	//if (args.contains("-l") || settings->value("logging/AlwaysLogToFile").toBool()) {
		// TODO
		// Also, consider adding the option to show the console rather than hide it
	//}
	
	// Determine whether last instance shut down correctly
	
	// Load and unload the instrument specification file to test it
	
	
	QByteArray testScheme =
			"{\"n\":\"Test path\",\"d\":\"This is a test path\",\"DDX_author\":\"2B Technologies\","
			"\"DDX_version\":\"0\",\"modules\":[{\"n\":\"Test inlet\",\"t\":\"ExampleInlet\",\"s\":"
			"{\"SampleSetting\":\"42\"}},{\"n\":\"Test module 1\",\"t\":\"ExampleModule\"},{\"n\":"
			"\"Test module 2\",\"t\":\"ExampleModule\",\"s\":{\"Flow_Rate\":\"12\",\"Analog_Inputs\":"
			"{\"items\":[{\"n\":\"Temperature Sensor\",\"t\":\"Voltage_AI\",\"Max_Voltage\":\"3.3\","
			"\"Min_Voltage\":\"0\"},{\"n\":\"Power Meter\",\"t\":\"Current_AI\",\"Max_Current\":\"20\","
			"\"Min_Current\":\"0\"},{\"n\":\"Barometer\",\"t\":\"Voltage_AI\",\"Max_Voltage\":\"2\","
			"\"Min_Voltage\":\"1\"}]}}}]}";
	testScheme.size();
	
	// Trigger all utility timers in five minutes
	twoMinuteTimeout = fiveMinuteTimeout = fifteenMinuteTimeout
			= hourlyTimeout = dailyTimeout = weeklyTimeout = monthlyTimeout
			= QDateTime::currentMSecsSinceEpoch() + 300000;
	utilityTimer->setTimerType(Qt::VeryCoarseTimer);
	utilityTimer->setInterval(UTILITY_INTERVAL);
	connect(utilityTimer, &QTimer::timeout, this, &Daemon::utilityTimeout);
	utilityTimer->start();
}

void Daemon::quit(int returnCode) {
	if (quitting) return;  // Prevent recursion
	quitting = true;
	// TODO: make this call finishing stuff
	n->shutdown();  // May take a while
	if (returnCode) lg->log(tr("Terminating (%1)").arg(returnCode));
	else lg->log(tr("Quitting"));
	qApp->exit(returnCode);
}

void Daemon::utilityTimeout() {
#define UTILITY_TIMEOUT_PERIOD ceil(UTILITY_INTERVAL/2)
	qint64 time = QDateTime::currentMSecsSinceEpoch();
	if ((twoMinuteTimeout - time) < UTILITY_TIMEOUT_PERIOD) {
		emit twoMinuteTimer();
		twoMinuteTimeout = time + 120000;
	}
	if ((fiveMinuteTimeout - time) < UTILITY_TIMEOUT_PERIOD) {
		emit fiveMinuteTimer();
		fiveMinuteTimeout = time + 300000;
	}
	if ((fifteenMinuteTimeout - time) < UTILITY_TIMEOUT_PERIOD) {
		emit fifteenMinuteTimer();
		fifteenMinuteTimeout = time + 900000;
	}
	if ((hourlyTimeout - time) < UTILITY_TIMEOUT_PERIOD) {
		emit hourlyTimer();
		hourlyTimeout = time + 3.6e+6;
	}
	if ((dailyTimeout - time) < UTILITY_TIMEOUT_PERIOD) {
		emit dailyTimer();
		dailyTimeout = time + 8.64e+7;
	}
	if ((weeklyTimeout - time) < UTILITY_TIMEOUT_PERIOD) {
		emit weeklyTimer();
		weeklyTimeout = time + 6.048e+8;
	}
	if ((monthlyTimeout - time) < UTILITY_TIMEOUT_PERIOD) {
		emit monthlyTimer();
		monthlyTimeout = time + 2.63e+9;
	}
}

QString Daemon::addDevice(RemDev *dev) {
	dLock.lock();
	devices.append(dev);
	int ct = devices.size();
	dLock.unlock();
	return tr("UnregisteredDevice%1").arg(ct);
}

void Daemon::registerDevice(RemDev *dev) {
	
}

void Daemon::removeDevice(RemDev *dev) {
	dLock.lock();
	devices.removeAll(dev);
	dLock.unlock();
	// TODO
}

void Daemon::testPath(const QByteArray &scheme, int log) {
	// TODO
	scheme.size();
	log += 2;
}

void Daemon::addPath(const QByteArray &name, int log) {
	/*this->lg->log("fail");
	PathManager *um = getUnitManager();
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
	// TODO*/
}

PathManager* Daemon::getUnitManager() {
	if ( ! unitManager) unitManager = new PathManager(this);
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

QDateTime Daemon::getTime() const {
	return QDateTime::currentDateTimeUtc().toTimeZone(tz);
}

int Daemon::versionCompare(QString versionA, QString versionB, bool ignoreMinor) {
	QStringList aList = versionA.split('.');
	QStringList bList = versionB.split('.');
	if (aList.size() != 2 || bList.size() != 2) return VERSION_COMPARE_FAILED;
	int a, b;
	bool ok;
	a = aList[0].toInt(&ok);
	if ( ! ok) return VERSION_COMPARE_FAILED;
	b = bList[0].toInt(&ok);
	if ( ! ok) return VERSION_COMPARE_FAILED;
	if (a > b) return 1;
	if (a < b) return -1;
	if (ignoreMinor) return 0;
	a = aList[1].toInt(&ok);
	if ( ! ok) return VERSION_COMPARE_FAILED;
	b = bList[1].toInt(&ok);
	if ( ! ok) return VERSION_COMPARE_FAILED;
	if (a > b) return 1;
	if (a < b) return -1;
	return 0;
}
