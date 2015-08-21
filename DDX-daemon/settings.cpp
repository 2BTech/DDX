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

#include "settings.h"
#include "daemon.h"
#include "logger.h"

Settings::Settings(Daemon *parent) : QObject(parent) {
	// Initialize
	lg = Logger::get();
	systemSettings = new QSettings(APP_AUTHOR_FULL, APP_NAME_SHORT, this);
	QList<SetEnt> loaded = registerSettings();
	foreach (const SetEnt &set, loaded) {
#ifdef LIST_SETTINGS_STARTUP
		lg->log(QString("Found setting: ").append(set.key));
#endif
		Q_ASSERT(((QMetaType::Type) set.defVal.type()) == set.type);
		s.insert(set.key, Setting(set.defVal, set.type));
	}
	// Check whether a full reset is required
	if ( ! systemSettings->contains("Version")
		|| parent->args.contains("-reconfigure", Qt::CaseInsensitive)) {
		resetAll();
		return;
	}
	int vc = Daemon::versionCompare(systemSettings->value("Version").toString());
	if (vc > 0) {
		lg->log
			(tr("Settings are for higher version or corrupted. Run latest version "
				"or launch daemon with '-reconfigure' option to reset all settings."), true);
		// TODO:  Quit here
		parent->quit(E_SETTINGS_VERSION);
		return;
	}
	lg->log(tr("Reloading settings last reset on %1")
		.arg(s.value("SettingsResetOn").v.toDateTime().toString("yyyy/MM/dd HH:mm:ss")));
	if (vc < 0) lg->log
		(tr("Settings are for a previous version of the DDX; some expected "
			"functionality may not work. Defaults for the current version will "
			"be used where applicable. Reconfigure from the GUI menu or by "
			"running the daemon with the '-reconfigure' option to update settings."));
	for (SettingsHash::iterator it = s.begin(); it != s.end(); ++it) {
		if (systemSettings->contains(it.key())) {
			QVariant value = systemSettings->value(it.key());
			if ( ! value.convert(it->t)) {
				lg->log(tr("Saved setting '%1' is %2, not %3; keeping default")
					.arg(it.key(), QMetaType::typeName(value.type()), QMetaType::typeName(it->t)));
				continue;
			}
			it->v = value;
		}
		// Todo:  Live-load command-line arguments ('-s"key":"value"')
	}
}

Settings::~Settings() {
	QWriteLocker l(&lock);
	delete systemSettings;  // Auto-syncs
}

QVariant Settings::v(const QByteArray &key, const QByteArray &group) const {
	QByteArray k = getKey(key, group);
	QReadLocker l(&lock);
	Q_ASSERT_X(s.contains(k), "Settings::verifySettingExists", k);
	return s.value(k).v;
}

QVariant Settings::getDefault(const QByteArray &key, const QByteArray &group) const {
	QByteArray k = getKey(key, group);
	QReadLocker l(&lock);
	Q_ASSERT(s.contains(k));
	return s.value(k).d;
}

bool Settings::set(const QByteArray &key, const QVariant &val,
				   const QByteArray &group, bool save) {
	QByteArray k = getKey(key, group);
	QWriteLocker l(&lock);
	Q_ASSERT(s.contains(k));
	SettingsHash::iterator it = s.find(k);
	if ( ! val.canConvert(it->t)) return false;
	it->v = val;
	it->v.convert(it->t);
	if (save) systemSettings->setValue(k, it->v);
	// TODO:  If hold and save, only change the stored setting, not the live one
	return true;
}

QVariant Settings::reset(const QByteArray &key, const QByteArray &group) {
	QByteArray k = getKey(key, group);
	QWriteLocker l(&lock);
	Q_ASSERT(s.contains(k));
	SettingsHash::iterator it = s.find(k);
	it->v = it->d;
	systemSettings->setValue(k, it->v);
	return it->d;
}

void Settings::resetAll() {
	lg->log(tr("Resetting all settings"), true);
	QWriteLocker l(&lock);
	systemSettings->clear();
	SettingsHash::iterator it;
	for (it = s.begin(); it != s.end(); ++it) {
		it->v = it->d;
		systemSettings->setValue(it.key(), it->d);
	}
	systemSettings->setValue("SettingsResetOn", QDateTime::currentDateTime());
	systemSettings->sync();
}

void Settings::saveAll() {
	QWriteLocker l(&lock);
	SettingsHash::iterator it;
	for (it = s.begin(); it != s.end(); ++it) {
		systemSettings->setValue(it.key(), it->v);
	}
	systemSettings->sync();
}

QList<Settings::SetEnt> Settings::registerSettings() const {
	SettingsFactory b;
	
	b.add("SettingsResetOn", tr("The date of last full settings reset"),
		  QDateTime::currentDateTime(), QMetaType::QDateTime);
	b.add("Version", tr("The DDX version these settings correspond to"),
		  VERSION_FULL_TEXT, QMetaType::QString);
	b.add("LastShutdownSafe", tr("Whether the previous shutdown was completed normally"),
		  false, QMetaType::Bool);
	
	b.enterGroup(SG_RPC);
	b.add("RegistrationPeriod", tr("Registration timeout period in seconds"),
		  10, QMetaType::Int);
	b.add("GlobalPassword", tr("A password which must be known to all devices"),
		  QByteArray(""), QMetaType::QByteArray);
	b.add("DaemonPassword", tr("A password which must be known to all managers"),
		  QByteArray(""), QMetaType::QByteArray);
	b.add("ManagerPassword", tr("A password which must be known to all managers"),
		  QByteArray(""), QMetaType::QByteArray);
	b.add("VertexPassword", tr("A password which must be known to all managers"),
		  QByteArray(""), QMetaType::QByteArray);
	b.add("ListenerPassword", tr("A password which must be known to all managers"),
		  QByteArray(""), QMetaType::QByteArray);
	
	b.enterGroup(SG_PATHS);
	// TODO
	/*b.add(D_INSTALL, tr("Where the DDX executables and libraries are"),
		  "tbd", QMetaType::QString);
	b.add(D_LOGS,
	b.add(D_DATA,
	b.add(D_CONFIG,
	*/
	
	b.enterGroup(SG_GUI);
	b.add("OpenOnStart", tr("Whether the GUI should be opened when the daemon starts"),
		  true, QMetaType::Bool);
	b.add("Executable", tr("The path to the GUI's executable"),
		  "tbd", QMetaType::QString);
	b.add("Arguments", tr("The GUI's process arguments"),
		  "", QMetaType::QString);
	b.add("KeepAlive", tr("Whether the GUI should be reopened if it crashes"),
		  true, QMetaType::Bool);
	
	b.enterGroup(SG_TIME);
	b.add("Timezone", tr("The local timezone"),
		  QTimeZone::systemTimeZoneId(), QMetaType::QByteArray);
	b.add("IgnoreDST", tr("Whether to ignore daylight savings time"),
		  true, QMetaType::Bool);
	b.add("ForceUTC", tr("Whether to force the use of UTC"),
		  false, QMetaType::Bool);
	
	b.enterGroup(SG_NETWORK);
	b.add("GUIPort", tr("The network port used by the GUI to manage the daemon"),
		  4388, QMetaType::Int);
	b.add("UseIPv6Localhost", tr("Whether to use IPv6 localhost rather than IPv4"),
		  false, QMetaType::Bool);
	b.add("AllowExternal", tr("Whether to allow foreign devices to connect"),
		  false, QMetaType::Bool);
	b.add("RequireSslExternal", tr("Whether to require SSL encryption on external connections"),
		  true, QMetaType::Bool);
	b.add("RequireSslLocal", tr("Whether to require SSL encryption on local connections"),
		  false, QMetaType::Bool);
	
	return b.list;
}

inline QByteArray Settings::getKey(const QByteArray &subKey, const QByteArray &group) const {
	if (group.isNull()) return subKey;
	QByteArray key(group);
	return key.append("/").append(subKey);
}
