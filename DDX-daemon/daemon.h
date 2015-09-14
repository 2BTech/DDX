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
#include <QThread>
#include <QList>
#include <QTimer>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMutex>
#include "daemon_constants.h"

class Path;
class PathManager;
class Network;
class Settings;
class Logger;
class RemDev;

//! \defgroup daemon Daemon
//! \defgroup modules Daemon modules

/*!
 * \brief The main manager of the DDX
 * 
 * Instantiating this class will begin DDX operation.
 * 
 * ## Thread Structure
 * In addition to the Daemon's primary thread, every Path and Beacon gets a
 * thread to itself.
 * 
 * ## Utility Timers
 * The Daemon class manages a set of utility timers to trigger periodic maintenance
 * actions.  Each of these timers will first fire about five minutes into DDX execution;
 * from then on, they will only fire around their specified interval.  They are useful for
 * actions such as cache clearing, device time setting, and triggering events at a distant
 * time.  These timers are neither precise nor accurate and should not be used as such.
 * 
 * \ingroup daemon
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
	
	/*!
	 * \brief Add a remote device to the master device list
	 * \param dev Pointer to the device
	 * \return The device's initial, unregistered name
	 */
	QString addDevice(RemDev *dev);
	
	void registerDevice(RemDev *dev);
	
	void removeDevice(RemDev *dev);
	
	PathManager *getUnitManager();
	
	void releaseUnitManager();
	
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
	
	Settings *getSettings() const {return sg;}
	
	int countRemoteDevices() const {return devices.size();}
	
	/*!
	 * \brief Compare a version string with this application's version
	 * \param versionA Version A (must be in the format "4.2")
	 * \param versionB Version B (must be in the format "4.2")
	 * \param ignoreMinor Whether to ignore the second number
	 * \return 0 if equivalent, 1 if A is greater, -1 if A is lesser
	 * 
	 * Returns VERSION_COMPARE_FAILED on error.  \a versionB defaults to this DDX version.
	 */
	static int versionCompare(QString versionA, QString versionB = VERSION_FULL_TEXT, bool ignoreMinor = false);
	
	QStringList args;

signals:
	
	//! Emitted approximately every two minutes
	void twoMinuteTimer() const;
	
	//! Emitted approximately every five minutes
	void fiveMinuteTimer() const;
	
	//! Emitted approximately every fifteen minutes
	void fifteenMinuteTimer() const;
	
	//! Emitted approximately every hour
	void hourlyTimer() const;
	
	//! Emitted approximately every day
	void dailyTimer() const;
	
	//! Emitted approximately every week
	void weeklyTimer() const;
	
	//! Emitted approximately every month
	void monthlyTimer() const;

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
	
	void quit(int returnCode=0);
	
private slots:
	
	/*!
	 * \brief Check all utility timers for timeout
	 * 
	 * This slot is called every UTILITY_INTERVAL for the duration of Daemon execution.
	 * It manages the sending of timeout signals.
	 * 
	 * To determine if a timer has timed out, it subtracts the current time from the timer's
	 * timeout time.  If the result is negative (meaning the timeout has already passed) or if
	 * it is positive and less than half of the UTILITY_INTERVAL, the corresponding timeout
	 * signal is sent and the timer is reset.
	 */
	void utilityTimeout();
	
private:
	
	//! Convenience pointer to Logger instance
	Logger *lg;
	
	//! Master pointer to Settings instance; must be manually freed
	Settings *sg;
	
	bool logReady;
	
	//! Master pointer to Network instance; must be manually freed
	Network *n;
	
	// //! The list of active Paths
	// QList<Path*> paths;
	
	/*! A list of all connected #devices; used mainly for garbage collection
	 * 
	 * Must be manually freed.  Note that devices in this list are not guaranteed
	 * to have unique cids!
	 */
	QList<RemDev*> devices;
	
	//! #devices lock
	QMutex dLock;
	
	//! Master pointer to PathManager instance; must be manually freed
	PathManager *unitManager;
	
	//! DDX time timezone, used by #getTime
	QTimeZone tz;
	
	//! Utility timer
	QTimer *utilityTimer;
	
	//! The time at which the two minute timer times out
	qint64 twoMinuteTimeout;
	
	//! The time at which the five minute timer times out
	qint64 fiveMinuteTimeout;
	
	//! The time at which the fifteen minute timer times out
	qint64 fifteenMinuteTimeout;
	
	//! The time at which the hourly timer times out
	qint64 hourlyTimeout;
	
	//! The time at which the daily timer times out
	qint64 dailyTimeout;
	
	//! The time at which the weekly timer times out
	qint64 weeklyTimeout;
	
	//! The time at which the monthly timer times out
	qint64 monthlyTimeout;
	
	bool quitting;
	
	void publishLog(QString msg);
	
	void publishAlert(QString msg);
};

#endif // DAEMON_H
