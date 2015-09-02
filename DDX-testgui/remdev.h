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
#include <QMutexLocker>
#include <QThread>
#include <QByteArray>
#include <QPointer>
#include "../rapidjson/include/rapidjson/document.h"
#include "../rapidjson/include/rapidjson/stringbuffer.h"
#include "../rapidjson/include/rapidjson/writer.h"
#include "../rapidjson/include/rapidjson/reader.h"
#include "constants.h"

class DevMgr;

class RemDev : public QObject
{
	Q_OBJECT
public:
#ifndef QT_DEBUG
	friend class TestDev;
#endif
	
	typedef int LocalId;
	
	enum DeviceRoles {
		DaemonRole = 0x1,  //!< Can execute paths
		ManagerRole = 0x2,  //!< An interface for a device which executes paths
		VertexRole = 0x4,  //!< A data responder or producer which does not execute paths
		ListenerRole = 0x8,  //!< A destination for loglines and alerts
		GlobalRole = 0x80  //!< A pseudo-role which indicates role-less information
	};
	
	enum DisconnectReason {
		UnknownReason,  //!< Unknown disconnection
		ShuttingDown,  //!< The disconnecting member is shutting down by request
		Restarting,  //!< The disconnecting member is restarting and will be back shortly
		FatalError,  //!< The disconnecting member experienced a fatal error and is shutting down
		ConnectionTerminated,  //!< The connection was explicitly terminated
		RegistrationTimeout,  //!< The connection was alive too long without registering
		BufferOverflow,  //!< The connection sent an object too long to be handled
		StreamClosed  //!< The stream was closed by its low-level handler
	};
	
	/*!
	 * \brief Response class
	 * 
	 * This class represents a response to a request.  Note that the JSON data they contain
	 * will also be deleted when the Response is deleted, so if a copy of the #response_data
	 * is required, that copy must be built with one of RapidJSON's deep copy operations, such
	 * as Value::CopyFrom.
	 * 
	 * Response handlers must have the prototype "void fn(Response *)" and will be called with
	 * queued connections (and thus will be executed by the receiving event loop). They **must**
	 * eventually delete the Response they are passed.  The also must either be declared as
	 * slots or with the `Q_INVOKABLE` macro.
	 * 
	 * If #successful is false, #mainVal is guaranteed to be a "verified" error object.  This
	 * means that it has a "code" member for which IsInt() returns true and a "message" member for
	 * which IsString() returns true.  The only guarantee made if #successful is true is that
	 * #mainVal is set.
	 */
	class Response {
	public:
		Response(bool successful, int id, const char *method, rapidjson::Document *doc,
				 rapidjson::Value *mainVal = 0) {
			this->successful = successful;
			this->id = id;
			this->method = method;
			this->mainVal = mainVal;
			this->doc = doc;
		}
		~Response() {
			delete doc;
		}
		
		//! True if the response was a response, false if there was an error
		bool successful;
		
		//! The integer ID returned by the corresponding sendRequest call
		int id;
		
		//! The contents of the "response" element on success or "error" on error
		rapidjson::Value *mainVal;
		
		//!The method name which was passed to sendRequest
		const char *method;
		
	private:
		//! Root document pointer (may be equivalent to #mainVal)
		rapidjson::Document *doc;
	};
	
	/*class RequestHandler {
	public:
		RequestHandler(QObject *handlerObj, const char *handlerFn, const char *method,
					   HandleFlag handleFlags, DeviceRoles requiredRoles) {
			
			this->handlerObj = handlerObj;
			this->handlerFn = handleFn;
			this->handleFlags = handleFlags;
			this->reqRoles = requiredRoles;
		}
	private:
		QPointer<QObject> handlerObj;
		const char *handlerFn;
		HandleFlag handleFlags;
		DeviceRoles reqRoles;
	};
	friend class RequestHandler;*/
	
	class Request {
	public:
		friend class RemDev;
		Request(const char *method, rapidjson::Value *params, rapidjson::Document *doc,
				rapidjson::Value *id) {
			this->method = method;
			this->params = params;
			this->id = id;
			this->doc = doc;
			this->outDoc = 0;
		}
		~Request() {
			delete doc;
			if (outDoc) delete outDoc;
		}
		
		/*!
		 * \brief Retrieve the allocator for outgoing RapidJSON 
		 * \return Reference to the allocator
		 */
		rapidjson::MemoryPoolAllocator<> &a() {
			if ( ! outDoc) outDoc = new rapidjson::Document;
			return outDoc->GetAllocator();
		}
		
		/*!
		 * \brief Whether this is a notification or request
		 * \return True if notification
		 */
		bool isNotif() const {
			return ! id;
		}
		
		//! Method name
		const char *method;
		
		//! The contents of the params element (may be 0)
		rapidjson::Value *params;
		
	private:
		
		//! ID value (will be 0 if this is a notification)
		rapidjson::Value *id;
		
		//! Root document pointer
		rapidjson::Document *doc;
		
		//! Response document for use with the direct-response versions of RemDev::sendRequest and RemDev::sendError
		rapidjson::Document *outDoc;
	};
	
	explicit RemDev(DevMgr *dm, bool inbound);
	
	~RemDev();
	
	/*!
	 * \brief Send a new request
	 * \param self The object on which the handler will be called
	 * \param handler The name of the handler function to call
	 * \param method The method name (UTF-8, must live through handling)
	 * \param doc Pointer to RapidJSON Document (0 if none, will be **deleted**)
	 * \param params Any parameters (0 to omit, will be **nullified, not deleted**)
	 * \param timeout Request timeout in msecs
	 * \return The integer ID which will also be in the corresponding Response object
	 * 
	 * Note that if this is called immediately before termination, it will return -1
	 * and no request will be sent.
	 */
	int sendRequest(QObject *self, const char *handler, const char *method, rapidjson::Document *doc = 0,
					rapidjson::Value *params = 0, qint64 timeout = DEFAULT_REQUEST_TIMEOUT);
	
	/*!
	 * \brief Send a successful response
	 * \param id The remote-generated transaction ID (will be **nullified, not deleted**)
	 * \param doc Pointer to RapidJSON Document (0 if none, will be **deleted**)
	 * \param result The result (0 -> true, will be **nullified, not deleted**)
	 */
	void sendResponse(rapidjson::Value &id, rapidjson::Document *doc = 0, rapidjson::Value *result = 0);
	
	/*!
	 * \brief Send a successful response directly to a Request
	 * \param req The Request object (will be **deleted**)
	 * \param result The result (0 -> true, will be **nullified, not deleted**)
	 * 
	 * If #req is a notification, #req will still be deleted but no response will be sent.
	 */
	void sendResponse(Request *req, rapidjson::Value *result = 0);
	
	/*!
	 * \brief Send an error response
	 * \param id Pointer to remote-generated transaction ID, (0 -> null, will be **nullified, not deleted**)
	 * \param code The integer error code
	 * \param msg The error message
	 * \param doc Pointer to RapidJSON Document (0 if none, will be **deleted**)
	 * \param data Pointer to any data (0 to omit, will be **nullified, not deleted**)
	 */
	void sendError(rapidjson::Value *id, int code, const QString &msg, rapidjson::Document *doc = 0,
				   rapidjson::Value *data = 0) noexcept;
	
	/*!
	 * \brief Send an error response directly to a Request
	 * \param req The Request object (will be **deleted**)
	 * \param code The integer error code
	 * \param msg The error message
	 * \param data Pointer to any data (0 to omit, will be **nullified, not deleted**)
	 * 
	 * If #req is a notification, #req will still be deleted but no response will be sent.
	 */
	void sendError(Request *req, int code, const QString &msg, rapidjson::Value *data = 0) noexcept;
	
	/*!
	 * \brief Send an error response directly to a Request by code alone
	 * \param req The Request object (will be **deleted**)
	 * \param code The integer error code
	 * 
	 * Supports:
	 * - E_JSON_INTERNAL ("Internal error")
	 * - E_ACCESS_DENIED ("Access denied")
	 * - E_NOT_SUPPORTED ("Not supported")
	 * - E_JSON_PARAMS ("Invalid params")
	 * 
	 * If #req is a notification, #req will still be deleted but no response will be sent.
	 */
	void sendError(Request *req, int code = E_JSON_INTERNAL) noexcept;
	
	/*!
	 * \brief Send a notification
	 * \param method The method name (UTF8)
	 * \param doc Pointer to RapidJSON Document (0 if none, will be **deleted**)
	 * \param params Any parameters (0 to omit, will be **nullified, not deleted**)
	 */
	void sendNotification(const char *method, rapidjson::Document *doc = 0,
						  rapidjson::Value *params = 0) noexcept;
	
	// TODO
	bool sendRegistration(const QStringList &passwords) noexcept;
	
	bool valid() const noexcept {return registered;}
	
#ifdef QT_DEBUG
	void printReqs() const;
	static QByteArray serializeValue(const rapidjson::Value &v);
#endif
	
public slots:
	
	/*!
	 * \brief Close this connection
	 * \param reason The reason for disconnection
	 * \param fromRemote Should be true if closing on behalf of the remote device
	 * 
	 * _Note:_ this will schedule the called object for deletion.
	 */
	void close(DisconnectReason reason = ConnectionTerminated, bool fromRemote = false) noexcept;
	
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
	 * This function is called regularly by the #timeoutPoller QTimer.  This timer
	 * is automatically started and stopped as needed.
	 */
	void timeoutPoll() noexcept;
	
signals:
	
	void postToLogArea(const QString &msg) const;
	
	void deviceDisconnected(RemDev *dev, DisconnectReason reason, bool fromRemote) const;
	
private slots:
	
	void init() noexcept;
	
protected:
	
	enum RegistrationState {
		UnregisteredState = 0x0,
		RegSentFlag = 0x1,
		RegAcceptedFlag = 0x2,
		RemoteRegAcceptedFlag = 0x4,
		RegisteredState = RegSentFlag|RegAcceptedFlag|RemoteRegAcceptedFlag
	};
	
	DevMgr *dm;  //!< Convenience pointer to DevMgr instance
	
	QByteArray cid;
	
	QByteArray remote_cid;
	
	QByteArray locale;
	
	qint64 connectTime;
	
	bool inbound;
	
	RegistrationState regState;
	
	bool closed;
	
	/*!
	 * \brief Handle a single, complete incoming item
	 * \param data The raw data to use
	 */
	void handleItem(const char *data) noexcept;
	
	/*!
	 * \brief Send a log line tagged with the cid
	 * \param msg The message
	 * \param isAlert Whether it is destined for the user
	 * 
	 * This is an overload which converts the QString to QByteArray.
	 */
	void log(const QString &msg, bool isAlert = false) const noexcept;
	
	virtual void sub_init() noexcept {}
	
	virtual void terminate(DisconnectReason reason, bool fromRemote) noexcept =0;
	
	/*!
	 * \brief Write a single RPC item
	 * \param data The data to be written (**must** be manually freed)
	 * 
	 * This function should quickly write to a buffer and then return.
	 * 
	 * _Warning:_ This function **must** be made thread-safe!
	 */
	virtual void writeItem(rapidjson::StringBuffer *buffer) noexcept =0;
	
	virtual const char *getType() const noexcept =0;
	
private:
	
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
		// Note: No id is necessary because it is the key in RequestHash
		QPointer<QObject> handlerObj;
		const char *handlerFn;
		const char *method;
		qint64 time;
		qint64 timeout_time;
	};
	
	//! A hash for maintaing lists of outgoing requests
	typedef QHash<LocalId, RequestRef> RequestHash;
	
	//! Manages all open outgoing requests to direct responses appropriately
	RequestHash reqs;
	
	/*typedef QHash<QByteArray, RequestHandler> HandlerHash;
	
	HandlerHash handlers;
	
	QReadWriteMutex hLock;*/
	
	//! Locks the request hash and lastId variable
	mutable QMutex req_id_lock;
	
	int pollerRefCount;
	
	LocalId lastId;
	
	qint64 registrationTimeoutTime;
	
	bool registered;
	
	void sendDocument(rapidjson::Document *doc);
	
	void handleRequest_Notif(rapidjson::Document *doc);
	
	void dispatchRequest_Notif();
	
	void handleResponse(rapidjson::Document *doc);
	
	void handleRegistration(rapidjson::Document *doc);
	
	void handleDisconnect(rapidjson::Document *doc);
	
	void simulateError(int id, const RequestRef &req, int code, const QString &msg);
	
	/*!
	 * \brief Log an incoming or simulated error
	 * \param errorVal The error object to log (must be verified, see Response)
	 * \param method The method name (will be assumed to be a null error if 0)
	 */
	void logError(const rapidjson::Value *errorVal, const char *method = 0) const;
	
	static inline void prepareDocument(rapidjson::Document *doc, rapidjson::MemoryPoolAllocator<> &a);
};

Q_DECLARE_METATYPE(RemDev::Response*)
Q_DECLARE_METATYPE(RemDev::Request*)

#endif // REMDEV_H
