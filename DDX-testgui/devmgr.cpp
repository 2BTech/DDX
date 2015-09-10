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
	refCt = 0;
	connect(this, &DevMgr::postToLogArea, mw->getLogArea(), &QPlainTextEdit::appendPlainText);
	// Enable invocation of handler functions
	qRegisterMetaType<Response*>("Response*");
	qRegisterMetaType<Request*>("Request*");
	// Set up timeout polling
	timeoutPoller = new QTimer(this);
	timeoutPoller->setTimerType(Qt::CoarseTimer);
	timeoutPoller->setInterval(TIMEOUT_POLL_INTERVAL);
	timeoutPoller->start();
}

DevMgr::~DevMgr() {
	delete timeoutPoller;
}

int DevMgr::getRef() {
	if (refCt.testAndSetAcquire(INT_MAX, 0))
		log(tr("Resetting ref counter"));
	return ++refCt;
}

void DevMgr::closeAll(int reason) {
	emit requestClose(reason, false);
}

void DevMgr::addHandler(QObject *handlerObj, const char *handlerFn, const char *method) {
	QByteArray key(method);
	hLock.lockForWrite();
	handlers.insert(key, RequestHandler(handlerObj, handlerFn));
	hLock.unlock();
	// TODO: remove
	log(tr("Registered handler"));
}

void DevMgr::removeHandler(const char *method) const {
	QByteArray key(method);
	hLock.lockForWrite();
	handlers.remove(key);
	hLock.unlock();
	// TODO: remove
	log(tr("Removed handler"));
}

void DevMgr::setHandlerEnabled(const char *method, bool enabled) {
	QByteArray key(method);
	hLock.lockForWrite();
	HandlerHash::Iterator it = handlers.find(key);
	if (it != handlers.end()) it->enabled = enabled;
	hLock.unlock();
}

void DevMgr::markDeviceRegistered(RemDev *dev) {
	// TODO
	emit deviceRegistered(dev, true, QString());
}

void DevMgr::markDeviceConnectionFailed(RemDev *dev, const QString &error) {
	if ( ! dev->isInbound())
		emit deviceRegistered(dev, false, error);
}

bool DevMgr::dispatchRequest(Request *req) const {
	QByteArray method(req->method);
	
	// Custom dispatching rules here
	
	QReadLocker l(&hLock);
	HandlerHash::ConstIterator it = handlers.find(method);
	if (it == handlers.constEnd()) return false;
	// Ensure the handler object still exists
	if (it->handlerObj.isNull()) {
		l.unlock();
		removeHandler(req->method);  // Remove it if not
		return false;
	}
	if ( ! it->enabled) return false;
	metaObject()->invokeMethod(it->handlerObj, it->handlerFn,
							   Qt::QueuedConnection, Q_ARG(Request*, req));
	return true;
}

void DevMgr::log(const QString &msg, bool isAlert) const {
	(void) isAlert;
	QString out("devmgr: ");
	out.append(msg);
	emit postToLogArea(out);
}

QByteArray serializeJson(const rapidjson::Value &v) {
	rapidjson::Document doc;
	doc.CopyFrom(v, doc.GetAllocator());
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	doc.Accept(writer);
	QByteArray array(buffer.GetString());
	Q_ASSERT(buffer.GetSize() == (uint) array.size());
	return array;
}
