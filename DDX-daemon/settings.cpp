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

Settings::Settings(Daemon *parent) : QObject(parent) {
	//QWriteLocker l(&lock);
	// Initialize
	systemSettings = new QSettings(APP_AUTHOR_FULL, APP_NAME_SHORT, this);
	QList<SetEnt> loaded = enumerateSettings();
	foreach (const SetEnt &set, loaded)
		s.insert(set.key, Setting(set.defVal, set.type));
	// Check existing settings
	if ( ! systemSettings->contains("Version")) resetAll();
	/*if (systemSettings->value("Version") > VERSION_FULL_TEXT) {
		parent->log(tr("Settings are for higher version"));
		parent->quit(E_SETTINGS_VERSION);
		return;
	}
	if (systemSettings->value("Version") < VERSION_FULL_TEXT) {
		// TODO
	}
	else for (QHash<QString, Setting>::iterator it = s.begin(); it != s.end(); ++it) {
		if (systemSettings->contains(it.key())) {
			Q_ASSERT(can cast);
		}
	}*/
}

Settings::~Settings() {
	QWriteLocker l(&lock);
	delete systemSettings;  // Auto-syncs
}

QVariant Settings::value(const QString &key, const QString &group) const {
	QString k = getKey(key, group);
	QReadLocker l(&lock);
	Q_ASSERT(s.contains(k));
	return s.value(k).v;
}

QVariant Settings::getDefault(const QString &key, const QString &group) const {
	QString k = getKey(key, group);
	QReadLocker l(&lock);
	Q_ASSERT(s.contains(k));
	return s.value(k).d;
}

bool Settings::set(const QString &key, const QVariant &val, const QString &group,
				   bool save, bool hold) {
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
	QWriteLocker l(&lock);
	systemSettings->clear();
	QHash<QString, Setting>::iterator it;
	for (it = s.begin(); it != s.end(); ++it) {
		it->v = it->d;
		systemSettings->setValue(it.key(), it->d);
	}
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
	
	b.enterGroup("network");
	b.add("GUIPort", tr("The network port used by the GUI to manage the daemon"),
		  4388, QMetaType::Int);
	b.add("UseIPv6Localhost", tr("Whether to use IPv6 localhost rather than IPv4"),
		  false, QMetaType::Bool);
	b.add("AllowExternalGUI", tr("Whether to allow non-local GUI to manage the daemon"),
		  false, QMetaType::Bool);
	
	return b.list;
}

inline QString Settings::getKey(const QString &subKey, const QString &group) const {
	if (group.isNull()) return subKey;
	QString key(subKey);
	return key.append("/").append(group);
}
