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
	delete sout;
	delete serr;
}

void Logger::log(const QString &msg, bool isAlert) {
	LogEntry e(msg, isAlert);
	// Immediately echo to output streams
	QTextStream *outStream = isAlert ? serr : sout;
	sLock.lock();
	// Note: using msg is faster than e.msg because QTextStream::operator<<
	// converts QByteArrays to QString anyway
#ifdef LOG_STREAM_TIMESTAMP
	*outStream << "[" << e.time.toString(LOG_STREAM_TIMESTAMP) << "] " << msg << endl;
#else
	*outStream << msg << endl;
#endif
	sLock.unlock();
	// Queue for dispatch
	qLock.lock();
	q.enqueue(e);
	qLock.unlock();
	// TODO:  Notify???
}

void Logger::handleMsg(QtMsgType t, const QMessageLogContext &c, const QString &m) {
#ifdef QT_DEBUG
	QString msg;
	if (c.file && !m.contains("ASSERT")) {
		msg = "Qt %1:%2 [%3]: ";
		msg = msg.arg(c.file, QString::number(c.line), c.function);
	}
	msg.append(m);
#else
	QString msg("DDX: ");
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
