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

#include "unitmanager.h"
#include "module.h"
#include "daemon.h"
#include "path.h"
// TODO: Remove
#include "modules/genmod.h"

UnitManager::UnitManager(Daemon *parent) : QObject(parent)
{
	modules = new QHash<QString, QMetaObject>;
	registerModules();
	beacons = new QHash<QString, QMetaObject>;
	changed = false;
	QString unitsFileName = parent->settings->value("paths/configPath").toString();
	unitsFileName.append(parent->settings->value("units/unitFile").toString());
}

UnitManager::~UnitManager()
{
	// TODO: iterate through these?
	// Does delete work on pointers????
	delete modules;
	delete beacons;
}

QString UnitManager::verifyPathScheme(const QByteArray scheme) {
	// Check basic scheme parsing requirements
	if (scheme.isEmpty())
		return tr("Scheme is empty");
	QJsonParseError pe;
	QJsonDocument schemeDoc = QJsonDocument::fromJson(scheme, &pe);
	if (pe.error != QJsonParseError::NoError)
		return tr("Scheme JSON failed to parse, reported: '%1'").arg(pe.errorString());
	if ( ! schemeDoc.isObject())
		return tr("Scheme is not a JSON object");
	
	// Check requirements of the scheme's top-level components
	QJsonObject schemeObj = schemeDoc.object();
	if ( !schemeObj.contains("n") || !schemeObj.contains("modules"))
		return tr("Scheme is missing name or module list");
	if ( ! schemeObj.value("n").toString().isEmpty())
		return tr("Scheme name element is invalid or empty");
	if ( ! schemeObj.value("modules").isArray())
		return tr("Scheme module list is not a JSON array");
	if (schemeObj.contains("d"))  // Descriptions are optional
		if ( ! schemeObj.value("d").isString())  // Descriptions can be empty
			return tr("Scheme contains a description which is not a string");
	if (schemeObj.contains("DDX_author"))  // Optional
		if ( ! schemeObj.value("DDX_author").toString().isEmpty())
			return tr("Scheme contains a DDX_author which is not a string or empty");
	if (schemeObj.contains("DDX_version"))
		if ( ! schemeObj.value("DDX_version").toString().isEmpty())
			return tr("Scheme contains a DDX_version which is not a string or empty");
	QJsonArray modArray = schemeDoc.array();
	if (modArray.size() < 1)
		return tr("Scheme does not contain any modules");
	
	// Loop through modules and check each one
	QJsonArray::const_iterator i;
	QStringList modNameList;
	Module *moduleInstance;
	bool firstElement = true;
	for (i = modArray.constBegin(); i != modArray.constEnd(); i++) {
		if ( ! i->isObject())
			return tr("Scheme contains a module which is not a JSON object");
		QJsonObject obj = i->toObject();
		if ( !obj.contains("n") || !obj.contains("t"))
			return tr("Scheme contains a module which is missing a name or type");
		if (obj.contains("s"))  // Settings are optional
			if ( ! obj.value("s").isObject())
				return tr("Scheme contains a module whose settings element is not a JSON object");
		QString n = obj.value("n").toString();
		if (n.isEmpty())
			return tr("Scheme contains a module whose name is not a string or empty");
		if (modNameList.contains(n, Qt::CaseInsensitive))
			return tr("Scheme contains multiple modules named '%1'").arg(n);
		modNameList.append(n);
		QString t = obj.value("t").toString();
		if (t.isEmpty())
			return tr("Scheme contains a module whose type is not a string or empty");
		if ( ! moduleExists(t))
			return tr("Scheme requests module of type '%1', which does not exist").arg(t);
		
		// Check that the Module is an inlet; requires instantiation :'(
		moduleInstance = constructModule(t, 0, n);
		if ( ! moduleInstance->inherits("Inlet") && firstElement) {
			delete moduleInstance;
			return tr("Scheme's first module is not an inlet");
		}
		if (moduleInstance->inherits("Inlet") && ! firstElement) {
			delete moduleInstance;
			return tr("Scheme contains multiple inlet modules");
		}
		delete moduleInstance;
		firstElement = false;
	}
	return QString();
}

bool UnitManager::moduleExists(const QString type) const {
	return modules->contains(type);
}

Module* UnitManager::constructModule(const QString type, Path *parent, const QString name) const {
	return (Module*) modules->value(type).newInstance(Q_ARG(Path*, parent),
													  Q_ARG(QString, name));
}


void UnitManager::registerModules() {
	// List all Modules here (1 of 2)
	// modules->insert("ExampleModule", ExampleModule::staticMetaObject);
	modules->insert("GenMod", GenMod::staticMetaObject);
}

QJsonObject UnitManager::getModuleList() const {
	QJsonObject l;
	
	// List all Modules here (2 of 2)
	// l.insert("ExampleModule", tr("Example description"));
	l.insert("GenMod", tr("General modifications (TODO)"));
	
	return l;
}
