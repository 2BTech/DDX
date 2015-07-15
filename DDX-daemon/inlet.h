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

#ifndef INLET_H
#define INLET_H

#include <QObject>
#include "constants.h"
#include "module.h"

/*!
 * \brief A Path's first Module, responsible for producing data lines
 */
class Inlet : public Module
{
	Q_OBJECT
public:
	
	/*!
	 * \brief Configure the Inlet for operation
	 * \param settings The JSON settings tree
	 * 
	 * TODO
	 */
	virtual void init(QJsonObject settings);
	
	/*!
	 * \brief Determines whether the stream is synchronous or asynchronous
	 * \return True if synchronous, false if asynchronous
	 */
	virtual bool isSynchronous() const;
	
	/*!
	 * \brief Determines whether the stream is finite or continuous
	 * \return True if finite, false if continuous
	 */
	virtual bool isFinite() const;
	
	virtual void start();
	
	virtual void stop();
	
	explicit Inlet(Path *parent, const QString name);
	
	~Inlet();
	
private:
#ifdef CAUTIOUS_ALERTS
	void handleReconfigure() {alert("DDX bug: handleReconfigure() called on an inlet");}
	void process() {alert("DDX bug: process() called on an inlet");}
#else
	void handleReconfigure() {}
	void process() {}
#endif
};

#endif // INLET_H
