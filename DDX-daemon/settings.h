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

class Daemon;

class Settings : public QObject
{
	Q_OBJECT
public:
	explicit Settings(Daemon *parent);
	
	~Settings();
	
	QVariant value(const QString &key, const QString &category = QString()) const;
	
	QVariant getDefault(const QString &key, const QString &category = QString()) const;
	
	bool set(const QString &key, const QVariant &val, const QString &category = QString(), bool save = true);
	
	void reset(const QString &key, const QString &category = QString());
	
	void resetAll();
	
signals:
	
public slots:
	
private:
	
	struct Setting {
		Setting(const QVariant value, const QVariant defaultValue,
				QMetaType::Type type) {
			v = value;
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
		void add(const QString key, const QString desc,
				 const QVariant defaultVal, QMetaType::Type type) {
			SetEnt se(key, desc, defaultVal, type);
			if (currentCat.isNull()) list.append(se);
			se.key.prepend(currentCat);
			list.append(se);
		}
		void enterCategory(QString &category) {
			if (category.isNull()) currentCat.clear();
			else currentCat = category.append("/");
		}
		QString currentCat;
		QList<SetEnt> list;
	};
	
	QSettings *systemSettings;
	
	QHash<QString, Setting> s;
	
	mutable QReadWriteLock lock;
	
	QList<SetEnt> enumerateSettings() const;
	
	inline QString getKey(const QString &subKey, const QString &cat) const;
	
};

#endif // SETTINGS_H
