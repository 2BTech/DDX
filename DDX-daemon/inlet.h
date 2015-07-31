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
#include "path.h"
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
	 * See Module::init() for general initiation information.  Inlets function
	 * exactly the same but with one additional requirement.  Inlet::init()
	 * _must_ also prepare a best-guess column structure with
	 * insertColumn() before returning.
	 */
	virtual void init(QJsonObject settings);
	
	/*!
	 * \brief Determines whether the stream is synchronous or asynchronous
	 * \return True if synchronous, false if asynchronous
	 */
	bool isSynchronous() const;
	
	/*!
	 * \brief Determines whether the stream is finite or continuous
	 * \return True if finite, false if continuous
	 */
	bool isFinite() const;
	
	/*!
	 * \brief Flag Inlet for starting
	 * 
	 * This function should return before the first call to process().  This
	 * function is called with plenty of stack overhead, so it's best to start
	 * any data collection systems and then use signals and slots to handle
	 * individual data lines once control is yielded back to the event loop.
	 */
	virtual void start();
	
	virtual void stop();
	
	explicit Inlet(Path *parent, const QByteArray &name);
	
	~Inlet();
	
private:
	bool streamIsSynchronous;
	bool streamIsFinite;
	
#ifdef CAUTIOUS_ALERTS
	void handleReconfigure() final {alert("DDX bug: handleReconfigure() called on an inlet");}
	void process() final {alert("DDX bug: process() called on an inlet");}
#else
	void handleReconfigure() final {}
	void process() final {}
#endif
};

#endif // INLET_H
