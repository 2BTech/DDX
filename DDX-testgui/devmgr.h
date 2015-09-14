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

#ifndef DEVMGR_H
#define DEVMGR_H

#include <QObject>
#include <QByteArray>
#include <QList>
#include <QHash>
#include <QMutex>
#include <QAtomicInt>
#include <QPlainTextEdit>
#include <QTimer>
#include <QReadWriteLock>
#include <QReadLocker>
#include "ddxrpc.h"
#include "remdev.h"

class MainWindow;

/*!
 * \brief Device manager
 * 
 * The DevMgr class manages DDX-RPC device connections.  It is built to simply handle both inbound
 * and outbound connections.
 * 
 * To listen for inbound connections, simply connect to the deviceRegistered() signal.  See the
 * Network class for understanding how to actually start a TCP connection to devices.
 * 
 * Connecting to a remote device is a bit more complex, but this is a good overview of the process:
 * - Connect to the deviceRegistered() signal
 * - Instantiate a RemDev subclass with an outbound constructor
 * - Wait until deviceRegistered() is emitted
 * - Check that deviceRegistered() is the device you want
 * 
 * \ingroup testgui
 */
class DevMgr : public QObject
{
	Q_OBJECT
public:
	friend class RemDev;
	
	explicit DevMgr(MainWindow *parent = 0);
	
	typedef QHash<QByteArray, RemDev*> DeviceHash;
	
	typedef QList<RemDev*> DeviceList;
	
	~DevMgr();
	
	/*!
	 * \brief Get a reference ID for new devices
	 * \return The integer ID
	 * 
	 * This function is thread-safe.
	 */
	int getRef();
	
	/*!
	 * \brief Close all devices
	 * \param reason The reason to tell all devices
	 * 
	 * This function is thread-safe.
	 */
	void closeAll(int reason = DevShuttingDown);
	
	/*!
	 * \brief Register a request handler
	 * \param handlerObj The object which will handle the request
	 * \param handlerFn The function of \a handlerObj to handle the request
	 * \param method The name of the method
	 * 
	 * This function will replace any existing handler for \a method.  If \a handlerObj
	 * is deleted, the handler will automatically be removed at the next request.
	 * 
	 * TODO discuss how handlers work
	 * 
	 * This function is thread-safe.
	 */
	void addHandler(QObject *handlerObj, const char *handlerFn, const char *method);
	
	/*!
	 * \brief Remove a previously registered handler
	 * \param method The DDX-RPC method to remove
	 * 
	 * This has no effect if no handler for \a method was registered with addHandler().
	 * 
	 * This function is thread-safe.
	 */
	void removeHandler(const char *method) const;
	
	/*!
	 * \brief Set whether a handler is enabled
	 * \param method The DDX-RPC method to set
	 * \param enabled Whether this handler should be enabled
	 * 
	 * This has no effect if no handler for \a method was registered with addHandler().
	 * 
	 * This function is thread-safe.
	 */
	void setHandlerEnabled(const char *method, bool enabled);
	
signals:
	
	void postToLogArea(const QString &msg) const;
	
	/*!
	 * \brief Signal a device is ready for RPC communication or that its connection failed
	 * \param device Pointer to the device, **may be invalid!**
	 * \param ok Whether the device was connected and registered successfully
	 * \param error The connection error string
	 * 
	 * This signal is guaranteed to be emitted at least once for every outbound RemDev instance.
	 * While some errors may trigger it to be sent more than once, it is guaranteed to be sent
	 * only once with \a ok set true.  The \a ok parameter **must** be checked for before
	 * dereferencing \a device.  If \a ok is false, all pointers to the device are now invalid.
	 * Note that this will **not** be emitted for failed inbound connections.
	 */
	void deviceRegistered(RemDev *device, bool ok, const QString &error) const;
	
	/*!
	 * \brief RemDev instances connect to this to enable closeAll()
	 * \param reason A DisconnectReason member
	 * \param fromRemote Should be true if closing on behalf of the remote device
	 */
	void requestClose(int reason, bool fromRemote) const;
	
public slots:
	
private:
	
	struct RequestHandler {
		RequestHandler(QObject *handlerObj, const char *handlerFn) {
			this->handlerObj = handlerObj;
			this->handlerFn = handlerFn;
			enabled = true;
		}
		QPointer<QObject> handlerObj;
		const char *handlerFn;
		bool enabled;
	};
	
	MainWindow *mw;
	
	DeviceList devices;
	
	mutable QMutex dLock;
	
	typedef QHash<QByteArray, RequestHandler> HandlerHash;
	
	mutable HandlerHash handlers;
	
	mutable QReadWriteLock hLock;
	
	QTimer *timeoutPoller;
	
	QAtomicInt refCt;
	
	/*!
	 * \brief RemDev instances call this to indicate they are ready for DDX-RPC communication.
	 * \param dev The device
	 * 
	 * This function must be called only once and markDeviceConnectionFailed() must not be called
	 * after it.
	 * 
	 * This function is thread-safe.
	 */
	void markDeviceRegistered(RemDev *dev);
	
	/*!
	 * \brief RemDev instances call this to indicate that an error ocurred prior to registration.
	 * \param dev The device
	 * \param error The error string
	 * 
	 * This function is thread-safe.
	 */
	void markDeviceConnectionFailed(RemDev *dev, const QString &error);
	
	/*!
	 * \brief dispatchRequest
	 * \param req
	 * \return 
	 * 
	 * This function is thread-safe.
	 */
	bool dispatchRequest(Request *req) const;
	
	/*!
	 * \brief log
	 * \param msg
	 * \param isAlert
	 * 
	 * This function is thread-safe.
	 */
	void log(const QString &msg, bool isAlert = false) const;
	
};

#endif // DEVMGR_H
