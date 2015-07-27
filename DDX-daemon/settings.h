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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QMetaType>
#include <QVariant>
#include <QJsonObject>
#include <QSettings>
#include <QReadWriteLock>
#include <QReadLocker>
#include <QWriteLocker>
#include <QGlobalStatic>
#include <QTimeZone>

class Daemon;
class Logger;

class Settings : public QObject
{
	Q_OBJECT
public:
	explicit Settings(Daemon *parent);
	
	~Settings();
	
	QVariant v(const QString &key, const QString &group = QString()) const;
	
	bool set(const QString &key, const QVariant &val,
			 const QString &group = QString(), bool save = true);
	
	void reset(const QString &key, const QString &group = QString());
	
	QVariant getDefault(const QString &key, const QString &group = QString()) const;
	
	void resetAll();
	
signals:
	
public slots:
	
private:
	
	struct Setting {
		Setting(const QVariant defaultValue,
				QMetaType::Type type) {
			v = defaultValue;
			d = defaultValue;
			t = type;
		}
		Setting(){}
		QVariant v;
		QVariant d;
		QMetaType::Type t;
	};
	
	struct SetEnt {
		SetEnt(const QString key, const QString desc,
			   const QVariant defaultVal, QMetaType::Type type) {
			this->key = key;
			this->desc = desc;
			this->defVal = defaultVal;
			this->type = type;
		}
		QString key;
		QString desc;
		QVariant defVal;
		QMetaType::Type type;
	};
	
	struct SettingsBuilder {
		SettingsBuilder(){}
		void add(QString key, QString desc,
				 QVariant defaultVal, QMetaType::Type type) {
			SetEnt se(key, desc, defaultVal, type);
			se.key.prepend(currentGroup);
			list.append(se);
		}
		void enterGroup(QString group) {
			if (group.isNull()) currentGroup.clear();
			else currentGroup = group.append("/");
		}
		QString currentGroup;
		QList<SetEnt> list;
	};
	
	Logger *logger;
	
	QSettings *systemSettings;
	
	QHash<QString, Setting> s;
	
	mutable QReadWriteLock lock;
	
	QList<SetEnt> enumerateSettings() const;
	
	inline QString getKey(const QString &subKey, const QString &group) const;
	
};

#endif // SETTINGS_H
