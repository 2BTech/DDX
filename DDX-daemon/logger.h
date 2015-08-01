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
#include <QPointer>

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
	 * global function, the Logger class is simplest to implement as a
	 * singleton.  For this reason, no public constructor is given.  This should
	 * be how components gain access to the logging system.
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
	
	/*!
	 * \brief Send a high-level message to the user
	 * \param msg The message
	 * 
	 * Convenience wrapper around log().  This funciton is thread-safe.
	 */
	void alert(const QString &msg) {log(msg, true);}
	
signals:
	
public slots:
	
private:
	
	struct Entry {
		Entry(const QString &msg, bool isAlert) {
			this->msg = msg;
			this->isAlert = isAlert;
		}
		QString msg;
		bool isAlert;
	};
	
	static QMutex constructionLock;
	
	QPointer<Daemon> daemon;
	
	//! stdout wrapper used for logging
	QTextStream *sout;
	
	//! stderr wrapper used for logging
	QTextStream *serr;
	
	QQueue<Entry> q;
	
	QMutex qLock;
	
	Logger();
	
	void handleMsg(QtMsgType t, const QMessageLogContext &c, const QString &m);
	friend void globalHandleMessage(QtMsgType t, const QMessageLogContext &c, const QString &m);
	
	void process();
	
	
};

#endif // LOGGER_H
