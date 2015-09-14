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

/*!
 * \file shared_constants.h
 * Constants shared between various DDX implementations
 */

#ifndef SHARED_CONSTANTS_H
#define SHARED_CONSTANTS_H

// RPC
#define DEFAULT_REQUEST_TIMEOUT 60000  //!< Time until a request times out in msecs (can be overridden)
//#define TIMEOUT_POLL_INTERVAL 9000 //!< Timeout polling frequency (in msecs)
#define TIMEOUT_POLL_INTERVAL 1000 //!< Timeout polling frequency (in msecs)
#define MAX_TRANSACTION_SIZE 536870000 //!< Maximum transaction size in bytes
#define REMDEV_THREADS

// NETWORK
#define ENABLE_SSL
//#define NETWORK_THREAD

// ERROR CODES
/* Note:  Error codes are automatically generated from RemoteManagementProtocol.md
 * with "(-?[0-9]+)\|([^|\r\n]+)\|(E_[A-Z0-9_]+)[^\r\n]*" -> "#define $3 $1 //!< $2" */
#define E_ACCESS_DENIED -32000 //!< Access denied
#define E_INVALID_RESPONSE -32001 //!< Invalid response
#define E_NOT_SUPPORTED -32002 //!< Not supported
#define E_NO_BATCH -32003 //!< Batch not supported
#define E_REQUEST_TIMEOUT -32004 //!< Request timed out
#define E_ENCRYPTION_REQUIRED -32005 //!< Encryption required
#define E_DEVICE_DISCONNECTED -32006 //!< Device disconnected
#define E_NETWORK_DISABLED 500 //!< Server does not implement network communication (for future use)
#define E_VERSION_FORBIDDEN 501 //!< Server is not compatible with the client version
#define E_NO_EXTERNAL_CONNECTIONS 502 //!< Server does not allow external connections
#define E_ADDRESS_FORBIDDEN 503 //!< Address forbidden
#define E_CLIENT_TYPE_FORBIDDEN 504 //!< A specified client role is explicitly forbidden
#define E_VERSION_UNREADABLE 505 //!< Version unreadable
#define E_PASSWORD_INVALID 506 //!< Password invalid
#define E_TARGET_INVALID 507 //!< Target invalid
#define E_PATH_NONEXISTENT 200 //!< Path does not exist
#define E_SETTING_NONEXISTENT 120 //!< Setting does not exist
#define E_SETTING_CONVERT 121 //!< Setting could not be converted to target type
#define E_SETTING_DECODE 122 //!< Setting could not be base-64 or JSON decoded
#define E_SETTING_DENIED 123 //!< Setting outside of requesting module
#define E_SETTING_SAVEREQUIRED 124 //!< Setting reset requests must be saved
#define E_SETTINGS_VERSION 10 //!< DDX settings are for higher version or corrupted
#define E_TCP_SERVER_FAILED 20 //!< A TCP server could not be established

// JSON ERROR CODES
#define E_JSON_PARSE -32700 //!< Invalid JSON or parse error
#define E_JSON_REQUEST -32600 //!< Invalid request
#define E_JSON_METHOD -32601 //!< Method not found
#define E_JSON_PARAMS -32602 //!< Invalid params
#define E_JSON_INTERNAL -32603 //!< Internal error

#endif // SHARED_CONSTANTS_H
