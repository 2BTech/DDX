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
#include <QJsonObject>

class Module;
class Inlet;
class Daemon;

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
 * 
 * ## Threading Information
 * Every Path runs in its own thread.  Modules written to communicate across
 * Paths must take this into account.  Furthermore, Beacons and some inlets
 * will also be run in their own threads.
 */
class Path : public QObject
{
	Q_OBJECT
public:
	
	explicit Path(Daemon *parent, const QByteArray model);
	
	~Path();
	
	/*!
	 * \brief Retreive a Module pointer by name via slow linear search
	 * \param name The name of the Module
	 * \return Pointer to a Module, or 0 if none found 
	 * 
	 * Note that this is a case-insensitive search.
	 */
	Module* findModule(QString name) const;
	
	QJsonObject publishSettings() const;
	
	QJsonObject publishActions() const;
	
	/*!
	 * \brief Echo a statement to all logging Beacons.
	 * \param msg The message
	 * \param m The Module from which it comes; ignore when writing Path alerts
	 * 
	 * Alerts are tagged with the name of the Path they come from.
	 */
	void alert(const QString msg, const Module *m = 0) const;
	
	QString getName() const {return name;}
	
signals:
	void isReady() const;
	
	void isRunning() const;
	
	void finished();
	
	void sendAlert(const QString msg) const;
	
public slots:
	void init();
	void start();
	void stop();
	
private:
	QJsonObject model;
	QList<Module*> *modules;
	Inlet *inlet;
	QString name;
	bool running;
	bool ready;
	
	QString getDefaultModuleName(const QString type);
};

#endif // PATH_H
