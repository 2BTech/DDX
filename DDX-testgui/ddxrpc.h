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

#ifndef DDXRPC_H
#define DDXRPC_H

#include <QByteArray>
#include "../rapidjson/include/rapidjson/document.h"
#include "../rapidjson/include/rapidjson/stringbuffer.h"
#include "../rapidjson/include/rapidjson/writer.h"
#include "../rapidjson/include/rapidjson/reader.h"

class RemDev;

enum DevRoles {
	DevDaemonRole = 0x1,  //!< Can execute paths
	DevManagerRole = 0x2,  //!< An interface for a device which executes paths
	DevVertexRole = 0x4,  //!< A data responder or producer which does not execute paths
	DevListenerRole = 0x8,  //!< A destination for loglines and alerts
	DevGlobalRole = 0x80  //!< A pseudo-role which indicates role-less information
};

//! Enumerates various disconnection reasons
enum DevDisconnectReason {
	DevUnknownDisconnect,  //!< Unknown disconnection
	DevShuttingDown,  //!< The disconnecting member is shutting down by request
	DevRestarting,  //!< The disconnecting member is restarting and will be back shortly
	DevFatalError,  //!< The disconnecting member experienced a fatal error and is shutting down
	DevTerminated,  //!< The connection was explicitly terminated
	DevRegistrationTimeout,  //!< The connection was alive too long without registering
	DevStreamClosed,  //!< The stream was closed by its low-level handler
	DevEncryptionRequired  //!< Encryption is required on this connection
};

/*!
 * \brief Represents an incoming RPC request or notification
 * 
 * Note that the JSON data will also be deleted when the Response is deleted, so if a copy
 * of the #mainVal is required, that copy must be built with one of RapidJSON's deep copy
 * operations, such as Value::CopyFrom.  Handlers **must** eventually delete the Request
 * object they are passed.
 * 
 * ## Responding to Requests
 * Incoming DDX-RPC requests and notifications are passed to their handlers with the
 * Request class.  This class and RemDev provide a convenient API for using RapidJSON
 * to produce and send a corresponding response or error response.  The simplest way to
 * respond to requests is using the overloads of RemDev::sendRequest and  RemDev::sendError
 * which take in a pointer to a Request.  If you need a reference to a RapidJSON allocator
 * when building supplemental information (as is often necessary), #alloc will return the
 * same allocator used to produce the final response object.
 * 
 */
class Request {
public:
	friend class RemDev;
	friend class TestDev; // TODO: Remove
	
	~Request() {
		delete doc;
		if (outDoc) delete outDoc;
	}
	
	/*!
	 * \brief Retrieve the allocator for outgoing RapidJSON 
	 * \return Reference to the allocator
	 */
	rapidjson::MemoryPoolAllocator<> &alloc() {
		if ( ! outDoc) outDoc = new rapidjson::Document;
		return outDoc->GetAllocator();
	}
	
	/*!
	 * \brief Whether this is a notification or request
	 * \return True if notification
	 */
	bool isRequest() const {
		return (bool) id;
	}
	
	//! Method name
	const char *method;
	
	//! The contents of the params element (may be 0, otherwise guaranteed to be an object)
	rapidjson::Value *params;
	
	//! The device which sent the request
	RemDev *device;
	
private:
	//! Internal constructor for RemDev
	Request(const char *method, rapidjson::Value *params, rapidjson::Document *doc,
			RemDev *device, rapidjson::Value *id) {
		this->method = method;
		this->params = params;
		this->device = device;
		this->id = id;
		this->doc = doc;
		this->outDoc = 0;
	}
	
	//! ID value (no type checking, will be 0 if this is a notification)
	rapidjson::Value *id;
	
	//! Root document pointer
	rapidjson::Document *doc;
	
	//! Response document for use with the direct-response versions of RemDev::sendRequest and RemDev::sendError
	rapidjson::Document *outDoc;
};
Q_DECLARE_METATYPE(Request*);

/*!
 * \brief Represents a response to an RPC request
 * 
 * Note that the JSON data will also be deleted when the Response is deleted, so if a copy
 * of the #mainVal is required, that copy must be built with one of RapidJSON's deep copy
 * operations, such as Value::CopyFrom.
 * 
 * Response handlers must have the prototype "void fn(Response *)" and will be called with
 * queued connections (and thus will be executed by the receiving event loop). They **must**
 * eventually delete the Response they are passed.  The also must either be declared as
 * slots or with the `Q_INVOKABLE` macro.
 * 
 * The only guarantee made if #ok is true is that #mainVal is set.  If #ok is
 * false, #mainVal is guaranteed to be a _verified_ error object.  This means that it has a
 * "code" member for which IsInt() returns true and a "message" member for which IsString()
 * returns true; no guarantees are made towards a possible "data" member.
 */
class Response {
public:
	friend class RemDev;
	
	~Response() {
		delete doc;
	}
	
	//! True if the response was a response, false if there was an error
	bool ok;
	
	//! The integer ID returned by the corresponding sendRequest call
	int id;
	
	//! The contents of the "response" element on success or "error" on error
	rapidjson::Value *mainVal;
	
	//!The method name which was passed to sendRequest
	const char *method;
	
	//! The device which received the response
	RemDev *device;
	
private:
	//! Internal constructor for RemDev
	Response(bool ok, int id, const char *method, rapidjson::Document *doc,
			 RemDev *device, rapidjson::Value *mainVal = 0) {
		this->ok = ok;
		this->id = id;
		this->method = method;
		this->mainVal = mainVal;
		this->device = device;
		this->doc = doc;
	}
	
	//! Root document pointer (may be equivalent to #mainVal)
	rapidjson::Document *doc;
};
Q_DECLARE_METATYPE(Response*);

/*!
 * \brief Serialize a RapidJSON Value to its JSON text
 * \param v The value to serialize
 * \return A null-terminated version of the JSON
 */
QByteArray serializeJson(const rapidjson::Value &v);
// Note: defined in devmgr.cpp

#endif // DDXRPC_H

