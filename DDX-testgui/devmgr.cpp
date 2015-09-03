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

#include "devmgr.h"
#include "mainwindow.h"

DevMgr::DevMgr(MainWindow *parent) : QObject(parent) {
	mw = parent;
	unregCt = 0;
	closing = false;
	// Enable invocation of handler functions
	qRegisterMetaType<RemDev::Response*>("RemDev::Response*");
	qRegisterMetaType<RemDev::Request*>("RemDev::Request*");
	// Set up timeout polling
	timeoutPoller = new QTimer(this);
	timeoutPoller->setTimerType(Qt::CoarseTimer);
	timeoutPoller->setInterval(TIMEOUT_POLL_INTERVAL);
	timeoutPoller->start();
}

DevMgr::~DevMgr() {
	delete timeoutPoller;
}

void DevMgr::closeAll(RemDev::DisconnectReason reason) {
	dLock.lock();
	closing = true;
	for (int i = 0; i < devices.size(); i++) {
		devices.at(i)->close(reason);
	}
	devices.clear();
	closing = false;
	dLock.unlock();
	closing = false;
}

void DevMgr::addHandler(QObject *handlerObj, const char *handlerFn, const char *method) {
	QByteArray key(method);
	RequestHandler entry(handlerObj, handlerFn);
	hLock.lockForWrite();
	handlers.insert(key, entry);
	hLock.unlock();
}

void DevMgr::removeHandler(const char *method) const {
	QByteArray key(method);
	hLock.lockForWrite();
	handlers.remove(key);
	hLock.unlock();
}

void DevMgr::setHandlerEnabled(const char *method, bool enabled) {
	QByteArray key(method);
	hLock.lockForWrite();
	HandlerHash::Iterator it = handlers.find(key);
	if (it != handlers.end()) it->enabled = enabled;
	hLock.unlock();
}

QByteArray DevMgr::addDevice(RemDev *dev) {
	connect(dev, &RemDev::postToLogArea, mw->getLogArea(), &QPlainTextEdit::appendPlainText);
	connect(timeoutPoller, &QTimer::timeout, dev, &RemDev::timeoutPoll);
	dLock.lock();
	devices.append(dev);
	dLock.unlock();
	QString name = tr("Unknown%1").arg(++unregCt);
	return name.toUtf8();
}

void DevMgr::removeDevice(RemDev *dev) {
	if (closing) return;
	dLock.lock();
	devices.removeOne(dev);
	dLock.unlock();
}

bool DevMgr::dispatchRequest(RemDev::Request *req) const {
	QByteArray method(req->method);
	
	// Custom dispatching rules here
	
	hLock.lockForRead();
	HandlerHash::ConstIterator it = handlers.find(method);
	if (it == handlers.constEnd()) {
		hLock.unlock();
		return false;
	}
	// Ensure the handler object still exists
	if (it->handlerObj.isNull()) {
		hLock.unlock();
		removeHandler(req->method);  // Remove it if not
		return false;
	}
	if ( ! it->enabled) {
		hLock.unlock();
		return false;
	}
	metaObject()->invokeMethod(it->handlerObj, it->handlerFn,
							   Qt::QueuedConnection, Q_ARG(RemDev::Request*, req));
	hLock.unlock();
	return true;
}
