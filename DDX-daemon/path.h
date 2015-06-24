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
#include <QList>
#include <QThread>
// TODO:  Replace with derivatives
#include "inlet.h"
#include "outlet.h"

class Path : public QObject
{
	Q_OBJECT
public:
	explicit Path(QObject *parent = 0);
	~Path();
	
signals:
	void threadDelta(int delta);
	
public slots:
	void init();
	
private:
	// TODO: Remove testing stuff
	Inlet *in;
	Outlet *out;
	QList<QThread*> *threads;
};

#endif // PATH_H
