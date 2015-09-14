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

#include "../shared_constants.h"

/*!
 * \file daemon_constants.h
 * Global constants and macros for DDX daemons
 * 
 * \ingroup daemon
 */

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
	#define LIST_SETTINGS_STARTUP
#endif
#define KEEP_UNITMANAGER

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
#if INT_MAX<60000
#	define UTILITY_INTERVAL INT_MAX
#else
#	define UTILITY_INTERVAL 60000
#endif

#endif // CONSTANTS_H
