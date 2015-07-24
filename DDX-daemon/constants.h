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

// GENERAL PURPOSE INDICATIVES
#define VERSION_FULL_TEXT "0.0"
#define APP_NAME_SHORT "DDX"
#define APP_NAME "2B DDX"
#define APP_AUTHOR "2B"
#define APP_AUTHOR_FULL "2B Technologies"
#define APP_AUTHOR_DOMAIN "twobtech.com"

// DEBUGGING
//! Comment to disable error checks that *shouldn't* be necessary in production
#define ENABLE_CAUTIOUS_ERROR_CHECKING
#ifdef ENABLE_CAUTIOUS_ERROR_CHECKING
	//! Whether to check for rigorously check for JSON parsing errors in code which should come from the DDX itself
	#define PATH_PARSING_CHECKS
	#define CAUTIOUS_CHECKS
#endif
#define KEEP_UNITMANAGER

// LOGGING
#ifdef ENABLE_DEBUG
#define LOGGING_ENABLE_PRINT_ALL  // Comment to disable printing of all but errors
#endif
#define LOGGING_ENABLE_STDOUT  // Comment to disable printing of log messages to stdout


// PLATFORM SPECIALIZATION


// BUFFERING
#define MAX_SOCKET_BUFFER_SIZE		104857600  // 104857600 = 100mb
#define DEFAULT_SLOW_BUFFER_SIZE	1048576  // 1048576 = 1mb
#define DEFAULT_FAST_BUFFER_SIZE	104857600  // 104857600 = 100mb


// ERROR CODES
#define E_QT_FATAL				1
#define E_QT_CRITICAL			2
#define E_SETTINGS_VERSION		10
#define E_TCP_SERVER_FAILED		20

// LOGGING
#define LOGGING_INCLUDE_TIMESTAMP

// Log function aliases
#define debug(msg) (qDebug((msg).toLatin1()))
// TODO:  Update to use qInfo (qt5.5+)
#define info(msg) (qDebug((msg).toLatin1()))
#define warning(msg) (qWarning((msg).toLatin1()))
#define critical(msg) (qCritical((msg).toLatin1()))
#define fatal(msg) (qFatal((msg).toLatin1()))

#endif // CONSTANTS_H
