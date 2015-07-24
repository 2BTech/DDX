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

#include "logger.h"
#include "daemon.h"

void globalHandleMessage(QtMsgType t, const QMessageLogContext &c, const QString &m) {
	Logger::get()->handleMsg(t, c, m);
}

//! Used to ensure that Logger construction is thread-safe
QMutex loggerConstructionLock;

Logger* Logger::get() {
	// TODO:  Check if this is safe
	QMutexLocker l(&loggerConstructionLock);
	static Logger instance;
	return &instance;
}

Logger::Logger() : QObject(qApp)
{
	sout = new QTextStream(stdout);
	serr = new QTextStream(stderr);
}

Logger::~Logger()
{
	QMutexLocker l(&qLock);
	delete sout;
	delete serr;
}

void Logger::log(const QString &msg, bool isAlert) {
#ifdef LOGGING_INCLUDE_TIMESTAMP
	QString echo(QDateTime::currentDateTime().toString("[yyyy/MM/dd HH:mm:ss.zzz] "));
	echo.append(msg);
#else
	QString echo(msg);
#endif
	// Immediately echo to output streams
	if (isAlert) *serr << echo << endl;
	else *sout << echo << endl;
	//
	if ( ! daemon || ! daemon->logReady) {
		QMutexLocker l(&qLock);
		q.enqueue(Entry(echo, isAlert));
		return;
	}
	
}

void Logger::handleMsg(QtMsgType t, const QMessageLogContext &c, const QString &m) {
#ifdef QT_DEBUG
	QString msg("Qt %1:%2 [%3]: ");
	msg = msg.arg(c.file, QString::number(c.line), c.function);
	msg.append(m);
#else
	QString msg("DDX bug: ");
	msg.append(m);
#endif
	
#if (QT_VERSION < QT_VERSION_CHECK(5, 5, 0))
	bool alert = (t != QtDebugMsg);
#else
	bool alert = ! (t == QtDebugMsg || t == QtInfoMsg);
#endif
	
	log(msg, alert);
	// TODO:  See if Fatal needs us to quit for it
}

void Logger::process() {
	
}
