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
	QWriteLocker l(&lock);
	
	enumerateSettings();
	
	systemSettings = new QSettings(APP_AUTHOR_FULL, APP_NAME_SHORT, this);
	
}

Settings::~Settings() {
	QWriteLocker l(&lock);
	delete systemSettings;  // Auto-syncs
}

QVariant Settings::value(const QString &key, const QString &category) const {
	QString k = getKey(key, category);
	QReadLocker l(&lock);
	Q_ASSERT(s.contains(k));
	return s.value(k).v;
}

QVariant Settings::getDefault(const QString &key, const QString &category) const {
	QString k = getKey(key, category);
	QReadLocker l(&lock);
	Q_ASSERT(s.contains(k));
	return s.value(k).d;
}

bool Settings::set(const QString &key, const QVariant &val, const QString &category, bool save) {
	QString k = getKey(key, category);
	QWriteLocker l(&lock);
	Q_ASSERT(s.contains(k));
	QHash<QString, Setting>::iterator it = s.find(k);
	if ( ! val.canConvert(it->t)) return false;
	it->v = val;
	it->v.convert(it->t);
	if (save) systemSettings->setValue(k, it->v);
	return true;
}

void Settings::reset(const QString &key, const QString &category) {
	QString k = getKey(key, category);
	QWriteLocker l(&lock);
	Q_ASSERT(s.contains(k));
	QHash<QString, Setting>::iterator it = s.find(k);
	it->v = it->d;
	systemSettings->setValue(k, it->v);
}

void Settings::resetAll() {
	QWriteLocker l(&lock);
	QHash<QString, Setting>::iterator it;
	for (it = s.begin(); it != s.end(); ++it) {
		it->v = it->d;
		systemSettings->setValue(it.key(), it->d);
	}
	systemSettings->sync();
}

QList<Settings::SetEnt> Settings::enumerateSettings() const {
	Settings::SettingsBuilder b;
	
	
	
	return b.list;
}

inline QString Settings::getKey(const QString &subKey, const QString &cat) const {
	if (cat.isNull()) return subKey;
	QString key(subKey);
	return key.append("/").append(cat);
}
