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

#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QMessageLogContext>
#include <QString>
#include <QTextStream>
#include <QQueue>
#include <QMutex>
#include <QMutexLocker>

class Daemon;

void globalHandleMessage(QtMsgType t, const QMessageLogContext &c, const QString &m);

class Logger : public QObject
{
	friend class Daemon;
	Q_OBJECT
public:
	
	static Logger* get();
	
	~Logger();
	
	void handleMsg(QtMsgType t, const QMessageLogContext &c, const QString &m);
	
signals:
	
public slots:
	
private:
	
	//! stdout wrapper used for logging
	QTextStream *sout;
	
	//! stderr wrapper used for logging
	QTextStream *serr;
	
	QQueue<QString> q;
	
	QMutex qLock;
	
	Logger();
	
	void process();
	
	
};

#endif // LOGGER_H
