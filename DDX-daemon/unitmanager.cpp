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
#include "settings.h"
// TODO: Remove
#include "modules/genmod.h"

UnitManager::UnitManager(Daemon *parent) : QObject(parent)
{
	registerModules();
	schemeFileNeedsRewriting = false;
	//QString schemeFileName = settings->value("paths/configPath").toString();
	//schemeFileName.append(settings->value("units/unitFile").toString());
	// TODO: load paths
}

UnitManager::~UnitManager()
{
	// TODO: iterate through modules
}

QByteArray UnitManager::getPathScheme(QString name) const {
	// TODO
	name.size();
	return QByteArray();
}

QString UnitManager::verifyPathScheme(const QByteArray scheme) const {
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
	if (schemeObj.value("n").toString().isEmpty())
		return tr("Scheme name element is invalid or empty");
	if (schemeObj.contains("d"))  // Descriptions are optional
		if ( ! schemeObj.value("d").isString())  // Descriptions can be empty
			return tr("Scheme contains a description which is not a string");
	if (schemeObj.contains("DDX_author"))  // Optional
		if (schemeObj.value("DDX_author").toString().isEmpty())
			return tr("Scheme contains a DDX_author which is empty or not a string");
	if (schemeObj.contains("DDX_version"))  // Optional
		if (schemeObj.value("DDX_version").toString().isEmpty())
			return tr("Scheme contains a DDX_version which is empty or not a string");
	if (schemeObj.contains("auto_start"))  // Optional
		if ( ! schemeObj.value("auto_start").isBool())
			return tr("Scheme contains an auto_start element which is not a boolean");
	if ( ! schemeObj.value("modules").isArray())
		return tr("Scheme module list is not a JSON array");
	QJsonArray modArray = schemeObj.value("modules").toArray();
	if (modArray.size() < 1)
		return tr("Scheme does not contain any modules");
	
	// Loop through modules and check each one
	QJsonArray::const_iterator i;
	QStringList modNameList;
	Module *moduleInstance;
	bool firstElement = true;
	for (i = modArray.constBegin(); i != modArray.constEnd(); ++i) {
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
			return tr("Scheme contains a module whose name is empty or not a string");
		if (modNameList.contains(n, Qt::CaseInsensitive))
			return tr("Scheme contains multiple modules named '%1'").arg(n);
		modNameList.append(n);
		QString t = obj.value("t").toString();
		if (t.isEmpty())
			return tr("Scheme contains a module whose type is empty or not a string");
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
	// TODO:  This function is largely untested!
}

QString UnitManager::addPath(const QByteArray scheme, bool save) {
	QString error = verifyPathScheme(scheme);
	if ( ! error.isEmpty()) return error;
	// TODO
	if (save) ((Daemon*) parent())->alert("DDX bug: path saving not implemented yet (UnitManager::addPath())");
	return QString();
}

bool UnitManager::moduleExists(const QString type) const {
	return modules.contains(type);
}

Module* UnitManager::constructModule(const QString type, Path *parent, const QString name) const {
	return (Module*) modules.value(type).newInstance(Q_ARG(Path*, parent),
													 Q_ARG(QString, name));
}

#include "modules/module_register.cpp"
