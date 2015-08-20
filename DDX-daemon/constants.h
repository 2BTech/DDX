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

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QtGlobal>

// GENERAL PURPOSE INDICATIVES
#define VERSION_FULL_TEXT "0.0"
#define APP_NAME_SHORT "DDX"
#define APP_NAME "2B DDX"
#define APP_AUTHOR "2B"
#define APP_AUTHOR_FULL "2B Technologies"
#define APP_AUTHOR_DOMAIN "twobtech.com"

// DEBUGGING
//! Comment to disable error checks that *shouldn't* be necessary in production
#ifdef QT_DEBUG
#define ENABLE_CAUTIOUS_ERROR_CHECKING
#endif
#ifdef ENABLE_CAUTIOUS_ERROR_CHECKING
	//! Whether to check for rigorously check for JSON parsing errors in schemes which should come from the DDX itself
	#define PATH_PARSING_CHECKS
	#define CAUTIOUS_CHECKS
	// #define LIST_SETTINGS_STARTUP
#endif
#define KEEP_UNITMANAGER

// RPC
#define DEFAULT_REQUEST_TIMEOUT 60000  //!< Time until a request times out in msecs (can be overridden)
#define TIMEOUT_POLL_INTERVAL 5000 //!< Timeout polling frequency (in msecs)
#define MAX_TRANSACTION_SIZE 536870000 //!< Maximum transaction size in bytes

// NETWORK
#define ENABLE_SSL

// LOGGING
// Comment to disable timestamping on stdout
#define LOG_STREAM_TIMESTAMP "yyyy-MM-dd HH:mm:ss.zzz"

// SETTINGS
#define SETTINGS_INCLUDE_DESCRIPTIONS
// SETTINGS GROUPS
#define SG_RPC "rpc"
#define SG_PATHS "paths"
#define SG_GUI "gui"
#define SG_TIME "time"
#define SG_NETWORK "network"

// DIRECTORIES
#define ddx_path(path_macro) (SG_PATHS "/" path_macro)
#define D_INSTALL "install"
#define D_LOGS "logs"
#define D_DATA "data"
#define D_CONFIG "config"

// PLATFORM SPECIALIZATION

// BUFFERING
#define MAX_SOCKET_BUFFER_SIZE		104857600  // 104857600 = 100mb

// MISCELLANEOUS
#define VERSION_COMPARE_FAILED 10

// ERROR CODES
/* Note:  Error codes are automatically generated from RemoteManagementProtocol.md
 * with "(-?[0-9]+)\|([^|\r\n]+)\|(E_[A-Z0-9_]+)" -> "#define $3 $1 //!< $2" */
#define E_ACCESS_DENIED -32000 //!< Access denied: Client's roles are not sufficient for the request
#define E_PARAMETER_OBJECT -32001 //!< Parameters not a JSON object
#define E_NOT_SUPPORTED -32002 //!< Not supported
#define E_RPC_GENERAL -32003 //!< An error occurred
#define E_TYPE_MISMATCH -32004 //!< Params contain invalid type
#define E_REQUEST_TIMEOUT -32005 //!< Request timed out
#define E_ENCRYPTION_REQUIRED -32006 //!< Encryption required
#define E_METHOD_RESPONSE_INVALID -32007 //!< Method gave invalid response
#define E_NETWORK_DISABLED 500 //!< Server does not implement network communication (for future use)
#define E_VERSION_FORBIDDEN 501 //!< Server is not compatible with the client version
#define E_NO_EXTERNAL_CONNECTIONS 502 //!< Server does not allow external connections
#define E_ADDRESS_FORBIDDEN 503 //!< Address forbidden
#define E_CLIENT_TYPE_FORBIDDEN 504 //!< A specified client role is explicitly forbidden
#define E_VERSION_UNREADABLE 505 //!< Version unreadable
#define E_PASSWORD_INVALID 506 //!< Password invalid
#define E_PATH_NONEXISTENT 200 //!< Path does not exist
#define E_SETTING_NONEXISTENT 120 //!< Setting does not exist
#define E_SETTING_CONVERT 121 //!< Setting could not be converted to target type
#define E_SETTING_DECODE 122 //!< Setting could not be base-64 or JSON decoded
#define E_SETTING_DENIED 123 //!< Setting outside of requesting module
#define E_SETTING_SAVEREQUIRED 124 //!< Setting reset requests must be saved
#define E_SETTINGS_VERSION 10 //!< DDX settings are for higher version or corrupted
#define E_TCP_SERVER_FAILED 20 //!< A TCP server could not be established

#endif // CONSTANTS_H
