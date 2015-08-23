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
#include <rapidjson.h>
#include "constants.h"

class DevMgr;

class RemDev : public QObject
{
	Q_OBJECT
public:
	
	// Bool is for whether it was successful; the second value is the contents of the error object if it failed
	// Note that errors will already be logged but not alerted
	typedef int LocalId;
	typedef void (*ResponseHandler)(int, QJsonValue, bool);
	typedef void (*RequestHandler)(QJsonValue, QJsonValue, bool);
	typedef QHash<QByteArray, RemDev*> DeviceHash;
	typedef QList<RemDev*> DeviceList;
	enum DeviceRole {
		DaemonRole = 0x1,  //!< Can execute paths
		ManagerRole = 0x2,  //!< An interface for a device which executes paths
		VertexRole = 0x4,  //!< A data responder or producer which does not execute paths
		ListenerRole = 0x8,  //!< A destination for loglines and alerts
		GlobalRole = 0x80  //!< A pseudo-role which indicates role-less information
	};
	Q_DECLARE_FLAGS(DeviceRoles, DeviceRole)
	enum DisconnectReason {
		UnknownReason,  //!< Unknown disconnection
		ShuttingDown,  //!< The disconnecting member is shutting down by request
		Restarting,  //!< The disconnecting member is restarting and will be back shortly
		FatalError,  //!< The disconnecting member experienced a fatal error and is shutting down
		ConnectionTerminated,  //!< The connection was explicitly terminated
		RegistrationTimeout,  //!< The connection was alive too long without registering
		BufferOverflow,  //!< The connection sent an object too long to be handled
		StreamClosed,  //!< The stream was closed by its low-level handler
		PasswordInvalid  //!< The passwords sent in response to registration were invalid
	};
	
	explicit RemDev(DevMgr *dm, bool inbound);
	
	~RemDev();
	
	/*!
	 * \brief Send a new request
	 * \param handler
	 * \param method
	 * \param params Any parameters (will be omitted if empty)
	 * \param timeout
	 * \return 
	 */
	//int sendRequest(ResponseHandler handler, const char *method,
	//				const rapidjson::Value *params = 0, qint64 timeout = DEFAULT_REQUEST_TIMEOUT);
	
	/*!
	 * \brief Send a successful response
	 * \param id The remote-generated transaction ID
	 * \param result The result (must **not** be undefined)
	 * \return True on success
	 */
	//bool sendResponse(rapidjson::Value *id = 0, rapidjson::Value *result = rapidjson::Value(rapidjson::kTrueType));
	
	/*!
	 * \brief Send an error response
	 * \param id The remote-generated transaction ID
	 * \param code The integer error code
	 * \param msg The error message
	 * \param data Data to be sent (undefined will be omitted, all other types will be included)
	 * \return True on success
	 */
	//bool sendError(rapidjson::Value *id, int code, const QString &msg, rapidjson::Value *data = 0);
	
	/*!
	 * \brief Send a notification
	 * \param method The method name
	 * \param params Any parameters (will be omitted if empty)
	 * \return True on success
	 */
	//bool sendNotification(const char *method, rapidjson::Value *params = 0);
	
	// TODO
	bool sendRegistration(const QStringList &passwords);
	
	/*!
	 * \brief Close this connection
	 * \param reason The reason for disconnection
	 * \param fromRemote Should be true if closing on behalf of the remote device
	 * 
	 * _Note:_ this will schedule the called object for deletion.
	 */
	void close(DisconnectReason reason = ConnectionTerminated, bool fromRemote = false);
	
	bool valid() const {return registered;}
	
	static const QJsonObject rpc_seed;
	
signals:
	
	void postToLogArea(const QString &msg) const;
	
	void deviceDisconnected(RemDev *dev, DisconnectReason reason, bool fromRemote) const;
	
private slots:
	
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
	void timeoutPoll();
	
	void init();
	
protected:
	DevMgr *dm;  //!< Convenience pointer to Daemon instance
	
	QString cid;
	
	QString client_cid;
	
	QByteArray locale;
	
	qint64 connectTime;
	
	bool inbound;
	
	bool registerAccepted;
	
	/*!
	 * \brief Handle a single, complete incoming item
	 * \param data A mutable copy of the raw data; will be freed automatically
	 */
	void handleItem(char *data);
	
	/*!
	 * \brief Send a log line tagged with the cid
	 * \param msg The message
	 * \param isAlert Whether it is destined for the user
	 */
	void log(const QByteArray &msg) const;
	
	/*!
	 * \brief Build a request object
	 * \param id The locally-generated integer ID (must **not** be 0)
	 * \param method The method name 
	 * \param params Any parameters (will be omitted if empty)
	 * \return The request object
	 */
	QJsonObject newRequest(LocalId id, const QString &method, const QJsonObject &params) const;
	
	/*!
	 * \brief Build a response object
	 * \param id The remote-generated ID
	 * \param result The params value (must **not** be null or undefined)
	 * \return The response object
	 */
	QJsonObject newResponse(QJsonValue id, const QJsonValue &result);
	
	/*!
	 * \brief Build an error object
	 * \param id The remote-generated ID (undefined IDs will be converted to null)
	 * \param code The integer error code
	 * \param msg The error message
	 * \param data Data to be sent (undefined will be omitted, all other types will be included)
	 * \return The error object
	 */
	QJsonObject newError(QJsonValue id, int code, const QString &msg, const QJsonValue &data) const;
	
	/*!
	 * \brief Build a notification object
	 * \param method The method name
	 * \param params Any parameters (will be omitted if empty)
	 * \return The notification object
	 */
	QJsonObject newNotification(const QString &method, const QJsonObject &params) const;
	
	//void simulateError(LocalId id, const RequestRef *ref, int code);
	
	virtual void sub_init() {}
	
	virtual void terminate(DisconnectReason reason, bool fromRemote) =0;
	
	/*!
	 * \brief write
	 * \param data
	 * 
	 * _Warning:_ This function **must** be made thread-safe!
	 */
	virtual void write(const char *data) =0;
	
private:
	
	struct RequestRef {
		RequestRef(ResponseHandler handler, qint64 timeout) {
			time = QDateTime::currentMSecsSinceEpoch();
			if (timeout) timeout_time = time + timeout;
			else timeout_time = 0;
			this->handler = handler;
		}
		// Note: No id is necessary because it is the key in RequestHash
		ResponseHandler handler;
		qint64 time;
		qint64 timeout_time;
	};
	
	//! A hash for maintaing lists of outgoing requests
	typedef QHash<LocalId, RequestRef> RequestHash;
	
	//! Manages all open outgoing requests to direct responses appropriately
	RequestHash reqs;
	
	//! Locks the request hash and lastId variable
	QMutex req_id_lock;
	
	typedef QHash<QString, RequestHandler> HandlerHash;
	
	HandlerHash reqHandlers;
	
	HandlerHash notifHandlers;
	
	//! Polls for timeouts; see timeoutPoll()
	QTimer *timeoutPoller;
	
	LocalId lastId;
	
	qint64 registrationTimeoutTime;
	
	bool registered;
	
	void sendObject(const QJsonObject &obj);
	
	void handleObject(const QJsonObject &obj);
	
	void handleRequest(const QJsonObject &obj);
	
	void handleNotification(const QJsonObject &obj);
	
	void handleRegistration(const QJsonObject &obj);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(RemDev::DeviceRoles)

#endif // REMDEV_H
