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
#include <QDateTime>
#include <QString>
#include <QTextStream>
#include <QQueue>
#include <QMutex>
#include <QMutexLocker>
#include <QPointer>
#include <QJsonObject>
#include "constants.h"

class Daemon;

//! For use by Qt's internal messages only
void globalHandleMessage(QtMsgType t, const QMessageLogContext &c, const QString &m);

/*!
 * \brief Manages global logging
 * 
 * The Logger class works tightly with the Daemon class to manage logging.  The
 * remote-operated nature of many DDX installations means that all logging must
 * potentially be duplicated to various locations.  The Logger class helps to
 * abstract all logging and alerting functionality with regards to other DDX
 * daemon components.
 * 
 * _Note:_ With the exception of process(), all functions in this class are
 * thread-safe.
 */
class Logger : public QObject
{
	friend class Daemon;
	Q_OBJECT
public:
	
	/*!
	 * \brief Retrieve the Logger instance
	 * \return The application's Logger
	 * 
	 * Because Qt's internal logging functions require logging to be tied to a
	 * global function, the Logger class is implemented as a singleton.  No
	 * public constructor is given.  This should be how components gain access
	 * to the logging system.
	 * 
	 * This function is thread-safe.
	 */
	static Logger* get();
	
	~Logger();
	
	/*!
	 * \brief Write a log message to the log systems
	 * \param msg The message to be logged
	 * \param isAlert Whether it is destined for the user
	 * 
	 * Publishes a log message to any listening targets.  Output to stdout and
	 * stderr happens immediately.  Messages then enter a queue to be output to
	 * designated targets.  Note that no messages will be sent until all startup
	 * targets either initialize or critically fail.  Since alerts are mainly
	 * used to indicate errors of some sort, the isAlert argument also
	 * determines whether the message is sent to stdout or stderr.
	 * 
	 * This function is thread-safe.
	 */
	void log(const QString &msg, bool isAlert = false);
	
signals:
	
public slots:
	
private:
	
	struct LogEntry {
		LogEntry(const QString &msg, bool isAlert) {
			time = QDateTime::currentDateTimeUtc();
			this->msg = msg;
			this->isAlert = isAlert;
		}
		QJsonObject toNetwork() {
			QJsonObject o;
			o.insert("Message", msg);
			o.insert("Time", time.toString(Qt::ISODate));
			o.insert("IsAlert", isAlert);
			return o;
		}
		QString msg;  //!< The message
		bool isAlert;  //!< Whether this is destined for the user
		QDateTime time;  //!< The time this log event was recorded (in UTC)
	};
	
	//! Locks Logger::Logger to prevent racing the singleton
	static QMutex constructionLock;
	
	//! Daemon handle (set by Daemon::Daemon)
	QPointer<Daemon> daemon;
	
	//! stdout wrapper used for logging
	QTextStream *sout;
	
	//! stderr wrapper used for logging
	QTextStream *serr;
	
	//! Output stream lock
	QMutex sLock;
	
	//! The log queue
	QQueue<LogEntry> q;
	
	//! Locks the log queue
	QMutex qLock;
	
	//! Private, thread-safe constructor for singleton instantiation
	Logger();
	
	//! For use by Qt's internal messages only
	void handleMsg(QtMsgType t, const QMessageLogContext &c, const QString &m);
	
	friend void globalHandleMessage(QtMsgType t, const QMessageLogContext &c, const QString &m);
};

#endif // LOGGER_H
