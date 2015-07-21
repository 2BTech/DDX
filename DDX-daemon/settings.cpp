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
	
}

Settings::~Settings() {
	
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
	return s.value(k).v;
}

bool Settings::set(const QString &key, const QVariant &val, const QString &category) {
	QString k = getKey(key, category);
	QWriteLocker l(&lock);
	Q_ASSERT(s.contains(k));
	QHash<QString, Setting>::iterator setting = s.find(k);
	if ( ! val.canConvert(setting->t))
		return false;
	setting->v = val;
	return true;
}

void Settings::reset(const QString &key, const QString &category) {
	QString k = getKey(key, category);
	QWriteLocker l(&lock);
	Q_ASSERT(s.contains(k));
	QHash<QString, Setting>::iterator setting = s.find(k);
	setting->v = setting->d;
}

void Settings::resetAll() {
	QWriteLocker l(&lock);
	QHash<QString, Setting>::iterator it;
	for (it = s.begin(); it != s.end(); ++it)
		it->v = it->d;
}

QJsonObject Settings::listAllSettings() const {
	
}

inline QString Settings::getKey(const QString &subKey, const QString &cat) const {
	QString key(subKey);
	if ( ! cat.isNull()) key.append("/").append(cat);
	return key;
}
