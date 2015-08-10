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
#include <QByteArray>
#include <QMetaType>
#include <QVariant>
#include <QJsonObject>
#include <QSettings>
#include <QReadWriteLock>
#include <QReadLocker>
#include <QWriteLocker>
#include <QGlobalStatic>
#include <QTimeZone>
#include <QStandardPaths>

class Daemon;
class Logger;

class Settings : public QObject
{
	Q_OBJECT
public:
	explicit Settings(Daemon *parent);
	
	~Settings();
	
	QVariant v(const QByteArray &key, const QByteArray &group = "") const;
	
	bool set(const QByteArray &key, const QVariant &val,
			 const QByteArray &group ="", bool save = true);
	
	void reset(const QByteArray &key, const QByteArray &group = "");
	
	QVariant getDefault(const QByteArray &key, const QByteArray &group = "") const;
	
	void resetAll();
	
	void saveAll();
	
signals:
	
public slots:
	
private:
	
	//! A simplified, long-term setting descriptor
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
	
	//! A bulky, temporary setting descriptor
	struct SetEnt {
		SetEnt(const QByteArray key, const QString desc,
			   const QVariant defaultVal, QMetaType::Type type) {
			this->key = key;
			this->desc = desc;
			this->defVal = defaultVal;
			this->type = type;
		}
		QByteArray key;
		QString desc;
		QVariant defVal;
		QMetaType::Type type;
	};
	
	//! Simplifies the registration of settings in enumerateSettings
	struct SettingsFactory {
		SettingsFactory(){}
		void add(QByteArray key, QString desc,
				 QVariant defaultVal, QMetaType::Type type) {
			SetEnt se(key, desc, defaultVal, type);
			se.key.prepend(currentGroup);
			list.append(se);
		}
		void enterGroup(QByteArray group) {
			if (group.isEmpty()) currentGroup.clear();
			else currentGroup = group.append("/");
		}
		QByteArray currentGroup;
		QList<SetEnt> list;
	};
	
	//! Logger handle
	Logger *logger;
	
	//! Maintains persistent storage of settings
	QSettings *systemSettings;
	
	//! The master settings map
	QHash<QByteArray, Setting> s;
	
	//! Locks the master settings map
	mutable QReadWriteLock lock;
	
	/*!
	 * \brief Register and list application-wide settings
	 * \return A list of settings
	 * 
	 * This function publishes a list of all the application-wide settings
	 * available.  Developers must register any settings they require in this
	 * function to use the DDX settings management system.  This function
	 * utilizes the SettingsFactory struct to list settings under the
	 * appropriate group.  Look at existing entries to understand usage.
	 * 
	 * _Note:_ In debug builds, the default value of every setting is tested
	 * against its type specification via an assert in the Settings constructor.
	 * If this assert is failing, define `LIST_SETTINGS_STARTUP` to show which
	 * entry causes the assertion fail.
	 */
	QList<SetEnt> registerSettings() const;
	
	//! Builds a hash key from a setting name and group
	inline QByteArray getKey(const QByteArray &subKey, const QByteArray &group) const;
	
};

#endif // SETTINGS_H
