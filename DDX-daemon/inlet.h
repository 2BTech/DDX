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
#include <QTextStream>

class Inlet : public QObject
{
	Q_OBJECT
public:
	explicit Inlet(QObject *parent = 0);
	~Inlet();
	bool isPersistent();
	void addStreamOut(QTextStream *stream);
	
signals:
	void msg(QString msg, QString dest);
	void error(QString error);
	void ready();
	void finished();
	
public slots:
	void init();
	//virtual void init() =0;
	
};

#endif // INLET_H
