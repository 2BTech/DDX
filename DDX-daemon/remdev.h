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
#include <QJsonObject>
#include <QJsonValue>
#include <QDateTime>
#include <QTimer>
#include <QTimeZone>
#include <QFlags>
#include <QMutex>
#include <QMutexLocker>
#include "constants.h"

class Daemon;
class Logger;
class Settings;

class RemDev : public QObject
{
	Q_OBJECT
public:
	
	// Bool is for whether it was successful; the second value is the contents of the error object if it failed
	// Note that errors will already be logged but not alerted
	typedef int LocalId;
	typedef void (*ResponseHandler)(LocalId, QJsonValue, bool);
	typedef void (*RequestHandler)(QJsonValue, QJsonValue, bool);
	typedef QHash<QByteArray, RemDev*> DeviceHash;
	typedef QList<RemDev*> DeviceList;
	enum ClientRole {
		DaemonRole = 0x1,
		ManagerRole = 0x2,
		VertexRole = 0x4,
		ListenerRole = 0x8
	};
	Q_DECLARE_FLAGS(ClientRoles, ClientRole)
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
	
	explicit RemDev(const QString &type, Daemon *parent);
	
	~RemDev();
	
	int sendRequest(ResponseHandler handler, const QString &method,
					const QJsonObject &params = QJsonObject(), qint64 timeout = DEFAULT_REQUEST_TIMEOUT);
	
	/*!
	 * \brief sendResponse
	 * \param id
	 * \param result
	 * \return 
	 * 
	 * _Warning:_ If the result parameter is of type `QJsonValue::Undefined`,
	 * an error will be sent to the client.  Developers should always ensure that
	 * the result parameter is a valid JSON type.
	 */
	bool sendResponse(LocalId id, const QJsonValue &result = true);
	
	bool sendError(LocalId id, int code, const QString &msg, const QJsonValue &data = QJsonValue::Undefined);
	
	bool sendNotification(const QString &method, const QJsonObject &params = QJsonObject());
	
	void close(DisconnectReason reason = StreamClosed);
	
	bool valid() {return !cid.isNull();}
	
	static const QJsonObject rpc_seed;
	
signals:
	
	void registered() const;
	
	void streamDisconnected(DisconnectReason reason) const;
	
public slots:
	
	void timeoutPoll();
	
protected:
	
	QString name;
	
	Daemon *d;
	
	Logger *lg;
	
	Settings *sg;
	
	QByteArray cid;
	
	QByteArray client_cid;
	
	QByteArray locale;
	
	qint64 connectTime;
	
	bool inbound;
	
	void handleLine(const QByteArray &data);
	
	void log(const QString &msg, bool isAlert = false) const;
	
	QJsonObject newRequest(LocalId id, const QString &method, const QJsonObject &params) const;
	
	QJsonObject newResponse(QJsonValue id, const QJsonValue &result);
	
	QJsonObject newError(LocalId id, int code, const QString &msg, const QJsonValue &data) const;
	
	QJsonObject newNotification(const QString &method, const QJsonObject &params) const;
	
	//void simulateError(LocalId id, const RequestRef *ref, int code);
	
	virtual void terminate(DisconnectReason reason = StreamClosed) =0;
	
	/*!
	 * \brief write
	 * \param data
	 * 
	 * _Warning:_ This function **must** be made thread-safe!
	 */
	virtual void write(const QByteArray &data) =0;
	
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
	
	RequestHash reqs;
	
	//! Locks the request hash and lastId variable
	QMutex req_id_lock;
	
	typedef QHash<QString, RequestHandler> HandlerHash;
	
	HandlerHash reqHandlers;
	
	HandlerHash notifHandlers;
	
	QTimer *timeoutPoller;
	
	LocalId lastId;
	
	qint64 registrationTimeoutTime;
	
	void sendObject(const QJsonObject &obj);
	
	void handleObject(const QJsonObject &obj);
	
	void handleNotification() const;
	
	void handleRequest() const;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(RemDev::ClientRoles)

#endif // REMDEV_H
