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

#ifndef REMDEV_H
#define REMDEV_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QTimer>
#include <QTimeZone>
#include <QFlags>
#include <QMutex>
#include <QThread>
#include <QByteArray>
#include <QPointer>
#include "../rapidjson/include/rapidjson/document.h"
#include "../rapidjson/include/rapidjson/stringbuffer.h"
#include "../rapidjson/include/rapidjson/writer.h"
#include "../rapidjson/include/rapidjson/reader.h"
#include "testgui_constants.h"
#include "ddxrpc.h"

class DevMgr;

/*!
 * \brief DDX-RPC remote device base class
 * 
 * ### %Request & %Response Document Privacy
 * The root documents produced by the incoming object handler are hidden by design so as
 * to prevent any mistyping attacks with double members.  While the JSON specification
 * disallows key duplication in objects, RapidJSON does not signal the phenomenon as a
 * parse error for efficiency reasons.  Furthermore, such JSON data may actually be useful
 * for certain cases (such as the DDX-RPC `Settings` type) if the standard is ever changed.
 * However, when unexpected, an attacker can theoretically use duplicates to bypass the
 * type restrictions made prior to Request and Response delivery.  Always use the pointers
 * provided by these two classes rather than using Value::FindMember on the root document.
 * 
 * 
 * ### State Handling
 * This class involves some decently complex state handling rules to manage its operation.
 * This section is useful only to developers of RemDev and subclasses.  Most of the RemDev state
 * is stored in three variables, listed with their state at instantiation:
 * 
 * - #open: false
 * - #registered: false
 * - #state: #InitialState
 * 
 * #open represents the state of the low-level handler (subclass) and loosely indicates whether
 * data can be reliably sent and received by the subclass.  It is modified by connectionReady()
 * and connectionClosed().  It is exposed to subclasses for easy reading but **must not** be
 * modified by them.  connectionReady() must be called only once and only when the subclass is
 * ready for full DDX-RPC communication.  connectionClosed() can be called at any time and will
 * schedule the RemDev instance for deletion.  Note that calling connectionClosed() does **not**
 * guarantee that no more calls to writeItem() will be made!
 * 
 * #registered represents whether the device has been fully registered.  Registration begins only
 * after connectionReady() has been called.  Registration marks a crucial point in a device's
 * lifetime.  Prior to registration, device termination is announced through
 * DevMgr::deviceRegistered(); afterwards it will be announced only through deviceDisconnected().
 * 
 * #state is set with members of #RegistrationState and is used to determine when registration has
 * been completed successfully.
 * 
 * \ingroup testgui
 */
class RemDev : public QObject
{
	Q_OBJECT
public:
#ifndef QT_DEBUG
	friend class TestDev;
#endif
	
	explicit RemDev(DevMgr *dm, bool inbound = 0);
	
	~RemDev();
	
	/*!
	 * \brief Send a new request
	 * \param self The object on which the handler will be called
	 * \param handler The name of the handler function to call
	 * \param method The method name (UTF-8, must live through handling)
	 * \param params Any parameters (0 to omit, will be **nullified, not deleted**)
	 * \param doc Pointer to RapidJSON Document (0 if none, will be **deleted**)
	 * \param timeout Request timeout in msecs (0 to disable timeout, not recommended)
	 * \return The integer ID which will also be in the corresponding Response object
	 * 
	 * See the Response class for information on handling the resulting response.
	 * 
	 * Note that if this is called immediately before termination, it will return -1
	 * and no request will be sent.
	 */
	int sendRequest(QObject *self, const char *handler, const char *method, rapidjson::Value *params = 0,
					rapidjson::Document *doc = 0, qint64 timeout = DEFAULT_REQUEST_TIMEOUT);
	
	/*!
	 * \brief Send a successful response directly to a Request
	 * \param req The Request object (will be **deleted**)
	 * \param result The result (0 -> true, will be **nullified, not deleted**)
	 * 
	 * If \a req is a notification, it will still be deleted but no response will be sent.
	 * 
	 * This is an overloaded function.
	 */
	void sendResponse(Request *req, rapidjson::Value *result = 0);
	
	/*!
	 * \brief Send a successful response
	 * \param id The remote-generated transaction ID (will be **nullified, not deleted**)
	 * \param doc Pointer to RapidJSON Document (0 if none, will be **deleted**)
	 * \param result The result (0 -> true, will be **nullified, not deleted**)
	 */
	void sendResponse(rapidjson::Value &id, rapidjson::Document *doc = 0, rapidjson::Value *result = 0);
	
	/*!
	 * \brief Send an error response directly to a Request
	 * \param req The Request object (will be **deleted**)
	 * \param code The integer error code
	 * \param msg The error message
	 * \param data Pointer to any data (0 to omit, will be **nullified, not deleted**)
	 * 
	 * If \a req is a notification, it will still be deleted but no response will be sent.
	 * 
	 * This is an overloaded function.
	 */
	void sendError(Request *req, int code, const QString &msg, rapidjson::Value *data = 0) noexcept;
	
	/*!
	 * \brief Send an error response directly to a Request by code alone
	 * \param req The Request object (will be **deleted**)
	 * \param code The integer error code
	 * 
	 * Supports:
	 * - `E_JSON_INTERNAL` ("Internal error")
	 * - `E_ACCESS_DENIED` ("Access denied")
	 * - `E_NOT_SUPPORTED` ("Not supported")
	 * - `E_JSON_PARAMS` ("Invalid params")
	 * - `E_JSON_METHOD` ("Method not found")
	 * 
	 * If \a req is a notification, it will still be deleted but no response will be sent.
	 * 
	 * This is an overloaded function.
	 */
	void sendError(Request *req, int code = E_JSON_INTERNAL) noexcept;
	
	/*!
	 * \brief Send an error response
	 * \param id Pointer to remote-generated transaction ID, (0 -> null, will be **nullified, not deleted**)
	 * \param code The integer error code
	 * \param doc Pointer to RapidJSON Document (0 if none, will be **deleted**)
	 * \param msg The error message
	 * \param data Pointer to any data (0 to omit, will be **nullified, not deleted**)
	 */
	void sendError(rapidjson::Value *id, int code, const QString &msg, rapidjson::Value *data = 0,
				   rapidjson::Document *doc = 0) noexcept;
	
	/*!
	 * \brief Send a notification
	 * \param method The method name (UTF8)
	 * \param doc Pointer to RapidJSON Document (0 if none, will be **deleted**)
	 * \param params Any parameters (0 to omit, will be **nullified, not deleted**)
	 */
	void sendNotification(const char *method, rapidjson::Document *doc = 0,
						  rapidjson::Value *params = 0) noexcept;
	
	//! Return whether this connection is ready for RPC communication
	bool valid() const noexcept {return registered;}
	
	bool isInbound() const noexcept {return inbound;}
	
#ifdef QT_DEBUG
	//! Print a list of all current requests (debug builds only)
	void printReqs() const;
#endif
	
public slots:
	
	/*!
	 * \brief Close this connection
	 * \param reason The reason for disconnection
	 * \param fromRemote Should be true if closing on behalf of the remote device
	 * 
	 * _Note:_ this will schedule the called object for deletion.
	 * 
	 * _Warning:_ this function is **not** thread-safe and must only be called from a
	 * thread-local slot or through queued connections.
	 */
	void close(int reason = DevTerminated, bool fromRemote = false) noexcept;
	
	/*!
	 * \brief Poll for operations that have timed out
	 * 
	 * Two types of timeouts are possible with DDX-RPC connections, both of which
	 * are checked by this function.
	 * 
	 * The first is individual request timeouts.
	 * These are mainly meant to protect against attacks in which a connecting client
	 * triggers repeated requests without responding to any, causing the #reqs member
	 * to grow indefinitely.  If a request does not receive a response within its
	 * requester-specified timeout time, an error will be sent to its handler and the
	 * corresponding RequestRef entry will be removed.
	 * 
	 * Registration timeouts will occur when a connection is not successfully
	 * registered within a user-set time interval.  Registration timeouts cannot
	 * be disabled and are meant to inhibit non-DDX connections from sitting
	 * unregistered forever.  Registration timeouts cause disconnection.
	 * 
	 * This function is queued regularly by the DevMgr::timeoutPoller QTimer.
	 * 
	 * _Note:_ this function is thread-safe.
	 */
	void timeoutPoll() noexcept;
	
signals:
	
	void postToLogArea(const QString &msg) const;
	
	/*!
	 * \brief Signalled when this device is disconnected
	 * \param dev The device which disconnected
	 * \param reason A member of DisconnectReason
	 * \param fromRemote Whether the remote device closed the connection
	 * 
	 * _Note:_ The emission of this signal indicates that the object has been scheduled
	 * for deletion.
	 */
	void deviceDisconnected(RemDev *dev, int reason, bool fromRemote) const;
	
private slots:
	
	//! Initializes the device
	void init() noexcept;
	
protected:
	
	DevMgr *dm;  //!< Convenience pointer to DevMgr instance
	
	QByteArray cid;
	
	QByteArray remote_cid;
	
	QByteArray locale;
	
	qint64 connectTime;
	
	bool inbound;
	
	/*! \brief Whether the subclass connection is open for communication
	 * 
	 * _Note:_ Subclasses must NOT set this variable!
	 */
	volatile bool open;
	
	/*!
	 * \brief Handle a single, complete incoming item
	 * \param data The raw data to use (will be deleted)
	 * 
	 * This function is thread-safe.
	 */
	void handleItem(char *data) noexcept;
	
	/*!
	 * \brief Mark the connection as ready
	 * 
	 * Must be called by subclasses as soon as the connection is ready for transmission.
	 * 
	 * _Warning:_ This function is not thread-safe and must be called from a thread-local slot.
	 */
	void connectionReady() noexcept;
	
	/*!
	 * \brief Handle a connection error
	 * \param error A localized error message, ignored if \a normalDisconnection is true
	 * \param normalDisconnection True if this disconnection is not the result of an error
	 * 
	 * This function can be called at any point by the subclass to indicate disconnection.
	 * 
	 * _Note:_ This function will schedule the object for deletion.
	 * 
	 * _Warning:_ This function is not thread-safe and must be called from a thread-local slot.
	 */
	void connectionClosed(const QString &error, bool normalDisconnection = false) noexcept;
	
	/*!
	 * \brief Send a log line tagged with the cid
	 * \param msg The message
	 * \param isAlert Whether it is destined for the user
	 */
	void log(const QString &msg, bool isAlert = false) const noexcept;
	
	/*!
	 * \brief Initialize the subclass
	 * 
	 * Will be called after the device's thread has started.
	 */
	virtual void sub_init() noexcept =0;
	
	/*!
	 * \brief Subclasses must reimplement this to close their connection
	 * 
	 * This function is called by close() and connectionClosed().  It does not have to be thread-safe.
	 */
	virtual void terminate() noexcept =0;
	
	/*!
	 * \brief Write a single RPC item
	 * \param buffer The data to be written (**must** be manually freed)
	 * 
	 * This function should quickly write to a queue and then return.  Note
	 * that this function may be called even after connectionClosed().
	 * 
	 * _Warning:_ This function **must** be made thread-safe!
	 */
	virtual void writeItem(rapidjson::StringBuffer *buffer) noexcept =0;
	
	/*!
	 * \brief Return the subclass connection type string
	 * \return E.g. "TCP" or "Bluetooth"
	 */
	virtual const char *getType() const noexcept =0;
	
private:
	
	//! Maintains handling information about an outgoing RPC request
	struct RequestRef {
		RequestRef(QObject *handlerObj, const char *handlerFn, const char *method, qint64 timeout) {
			time = QDateTime::currentMSecsSinceEpoch();
			if (timeout) timeout_time = time + timeout;
			else timeout_time = 0;
			this->handlerObj = handlerObj;
			this->handlerFn = handlerFn;
			this->method = method;
		}
		RequestRef() {handlerObj = 0;}  // Mark invalid
		
		/*!
		 * \brief Determine request validity
		 * \param checkTime Current time or 0 to disable timeout check
		 * \return Whether the request is still valid
		 * 
		 * Requests can become invalid if their receiver is destroyed or they timeout.
		 */
		bool valid(qint64 checkTime) const {
			if (checkTime && timeout_time)
				if (timeout_time < checkTime) return false;
			return ! handlerObj.isNull();
		}
		// Note: No ID is necessary because it is the key in RequestHash
		QPointer<QObject> handlerObj;
		const char *handlerFn;
		const char *method;
		qint64 time;
		qint64 timeout_time;
	};
	
	//! Used by handleItem()
	enum MainValType {
		ParamsT,
		ResultT,
		ErrorT
	};
	
	enum RegistrationState {
		InitialState = 0x0,
		RegSentFlag = 0x1,
		RegAcceptedFlag = 0x2,
		RemoteRegAcceptedFlag = 0x4,
		RegisteredState = RegSentFlag|RegAcceptedFlag|RemoteRegAcceptedFlag
	};
	
	int_fast8_t state;
	
	//! A hash for maintaing lists of outgoing requests
	typedef QHash<int, RequestRef> RequestHash;
	
	//! Manages all open outgoing requests to direct responses appropriately
	RequestHash reqs;
	
	//! Locks the request hash and lastId variable
	mutable QMutex req_id_lock;
	
	//! The previous integer ID sent out as part of a request
	int lastId;
	
	//! The time at which registration will time out
	qint64 registrationTimeoutTime;
	
	//! Whether the connection has been successfully registered
	bool registered;
	
	/*!
	 * \brief Stringify and deliver a complete JSON document
	 * \param doc Root document (will be **deleted)
	 */
	void sendDocument(rapidjson::Document *doc);
	
	/*!
	 * \brief TODO
	 * \param r The incoming Request (must be deleted)
	 * 
	 * **Warning:** \a r is not guaranteed to be a `register` request!
	 */
	void handleRegRequest(Request *r);
	
	/*!
	 * \brief TODO
	 * \param r The incoming Response (must be deleted)
	 * 
	 * **Warning:** \a r is not guaranteed to be a response to a `register` request!
	 */
	void handleRegResponse(Response *r);
	
	/*!
	 * \brief TODO
	 * \param doc Root document (will be **deleted**)
	 */
	void handleDisconnectNotification(rapidjson::Document *doc);
	
	/*!
	 * \brief Build and deliver a simulated error to an outgoing request
	 * \param id The integer ID of the request
	 * \param req The RequestRef object corresponding to the request
	 * \param code The integer error code
	 * \param msg The error message
	 */
	void simulateError(int id, const RequestRef &req, int code, const QString &msg);
	
	/*!
	 * \brief Log an incoming or simulated error
	 * \param errorVal The error object to log (must be _verified_, see Response)
	 * \param method The method name (will be assumed to be a null error if 0)
	 */
	void logError(const rapidjson::Value *errorVal, const char *method = 0) const;
	
	/*!
	 * \brief Set up an existing RapidJSON document for delivery
	 * \param doc The document to prepare
	 * \param a Convenience reference to \a doc's allocator
	 */
	static inline void prepareDocument(rapidjson::Document *doc, rapidjson::MemoryPoolAllocator<> &a);
};

#endif // REMDEV_H
