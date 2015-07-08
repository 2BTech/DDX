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

#ifndef PATH_H
#define PATH_H

#include <QObject>
#include <QString>
#include <QList>
#include "module.h"

class Module;
class Inlet;

/*!
 * \brief A complete string of consecutive Modules which handles data lines
 * 
 * ## Testing %Path Configurations
 * Paths can be built by an external configuration wizard in "test mode".  In
 * test mode, alerts are redirected to a text stream, then Module::init() is
 * called on every Module followed by the initial reconfigure.  This allows
 * users to test the structure of their configurations before a Path goes live.
 * The Module::init() and Module::handleReconfigure() functions should catch as
 * many errors as possible.
 */
class Path : public QObject
{
	Q_OBJECT
public:
	
	explicit Path(bool test = false, QObject *parent = 0);
	
	/*!
	 * \brief Retreive a Module pointer by name
	 * \param name The name of the Module
	 * \return Pointer to a Module, or 0 if none found 
	 */
	Module* findModule(QString name);
	
	QString getName() {return name;}
	
	~Path();
	
signals:
	
public slots:
	void init();
	
private:
	QList<Module*> *modules;
	QString name;
	bool inTestMode;
	bool isRunning;
	
	QString getDefaultModuleName();
};

#endif // PATH_H
