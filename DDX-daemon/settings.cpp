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
	logger = Logger::get();
	systemSettings = new QSettings(APP_AUTHOR_FULL, APP_NAME_SHORT, this);
	QList<SetEnt> loaded = enumerateSettings();
	foreach (const SetEnt &set, loaded) {
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
		logger->alert(tr("Settings are for higher version or corrupted. "
					   "Run latest version or launch daemon with '-reconfigure' "
					   "option to reset all settings."));
		// TODO:  Quit here
		parent->quit(E_SETTINGS_VERSION);
		return;
	}
	logger->log(tr("Reloading settings last reset on %1")
		.arg(s.value("SettingsResetOn").v.toDateTime().toString("yyyy/MM/dd HH:mm:ss")));
	if (vc < 0) logger->log
		(tr("Settings are for a previous version of the DDX; some expected "
			"functionality may not work. Defaults for the current version will "
			"be used where applicable. Reconfigure from the GUI menu or by "
			"running the daemon with the '-reconfigure' option to update settings."));
	for (QHash<QString,Setting>::iterator it = s.begin(); it != s.end(); ++it) {
		if (systemSettings->contains(it.key())) {
			QVariant value = systemSettings->value(it.key());
			if ( ! value.canConvert(it->t)) {
				logger->log(tr("Saved setting '%1' is %2, not %3; keeping default")
					.arg(it.key(), QMetaType::typeName(value.type()), QMetaType::typeName(it->t)));
				continue;
			}
			it->v = value;
		}
		// Todo:  Live-load command-line arguments ("-s'key':'value'")
	}
}

Settings::~Settings() {
	QWriteLocker l(&lock);
	delete systemSettings;  // Auto-syncs
}

QVariant Settings::v(const QString &key, const QString &group) const {
	QString k = getKey(key, group);
	QReadLocker l(&lock);
	Q_ASSERT_X(s.contains(k), "Settings::verifySettingExists", k.toLatin1());
	return s.value(k).v;
}

QVariant Settings::getDefault(const QString &key, const QString &group) const {
	QString k = getKey(key, group);
	QReadLocker l(&lock);
	Q_ASSERT(s.contains(k));
	return s.value(k).d;
}

bool Settings::set(const QString &key, const QVariant &val,
				   const QString &group, bool save) {
	QString k = getKey(key, group);
	QWriteLocker l(&lock);
	Q_ASSERT(s.contains(k));
	QHash<QString, Setting>::iterator it = s.find(k);
	if ( ! val.canConvert(it->t)) return false;
	it->v = val;
	it->v.convert(it->t);
	if (save) systemSettings->setValue(k, it->v);
	// TODO:  If hold and save, only change the stored setting, not the live one
	return true;
}

void Settings::reset(const QString &key, const QString &group) {
	QString k = getKey(key, group);
	QWriteLocker l(&lock);
	Q_ASSERT(s.contains(k));
	QHash<QString, Setting>::iterator it = s.find(k);
	it->v = it->d;
	systemSettings->setValue(k, it->v);
}

void Settings::resetAll() {
	logger->log(tr("Resetting all settings"));
	QWriteLocker l(&lock);
	systemSettings->clear();
	QHash<QString, Setting>::iterator it;
	for (it = s.begin(); it != s.end(); ++it) {
		it->v = it->d;
		systemSettings->setValue(it.key(), it->d);
	}
	systemSettings->setValue("SettingsResetOn", QDateTime::currentDateTime());
	systemSettings->sync();
}

QList<Settings::SetEnt> Settings::enumerateSettings() const {
	SettingsBuilder b;
	
	b.add("SettingsResetOn", tr("The date of last full settings reset"),
		  QDateTime::currentDateTime(), QMetaType::QDateTime);
	b.add("Version", tr("The DDX version these settings correspond to"),
		  VERSION_FULL_TEXT, QMetaType::QString);
	b.add("LastShutdownSafe", tr("Whether the previous shutdown was completed normally"),
		  false, QMetaType::Bool);
	
	b.enterGroup(SG_TIME);
	b.add("Timezone", tr("The local timezone"),
		  QTimeZone::systemTimeZoneId(), QMetaType::QString);
	b.add("IgnoreDST", tr("Whether to ignore daylight savings time"),
		  true, QMetaType::Bool);
	b.add("ForceUTC", tr("Whether to force the use of UTC"),
		  false, QMetaType::Bool);
	
	b.enterGroup(SG_NETWORK);
	b.add("GUIPort", tr("The network port used by the GUI to manage the daemon"),
		  4388, QMetaType::Int);
	b.add("UseIPv6Localhost", tr("Whether to use IPv6 localhost rather than IPv4"),
		  false, QMetaType::Bool);
	b.add("AllowExternalManagement", tr("Whether to allow non-local GUI to manage the daemon"),
		  false, QMetaType::Bool);
	
	return b.list;
}

inline QString Settings::getKey(const QString &subKey, const QString &group) const {
	if (group.isNull()) return subKey;
	QString key(group);
	return key.append("/").append(subKey);
}
