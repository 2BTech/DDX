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

#ifndef DAEMON_H
#define DAEMON_H

#include <QObject>
#include <QTextStream>  // For outputting to stdout
#include <QDateTime>
#include "../DDX-gui/constants.h"

class Daemon : public QObject
{
	Q_OBJECT
public:
	explicit Daemon(QObject *parent = 0);
	~Daemon();

signals:

public slots:
	void init();
	void log(const QVariant &msg);  // Print a low-info log message
	// TODO: void report(const QVariant &msg);  // Send a message which should be saved to disk or inserted into the data log somehow
	// TODO: void notify(const QVariant &msg);  // Pump out a desktop notification and/or email notification (See snorenotify)

private:
	QTextStream *qout;  // stdout wrapper
};

#endif // DAEMON_H
