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
#define APP_NAME_UNTRANSLATABLE_SHORT "DDX"
#define APP_NAME_UNTRANSLATABLE "2B DDX"
#define APP_AUTHOR "2B"
#define APP_AUTHOR_FULL "2B Technologies"
#define APP_AUTHOR_DOMAIN "twobtech.com"

// DEBUGGING
//! Comment to disable error checks that *shouldn't* be necessary in production
#define CAUTIOUS_ERROR_CHECKING
#ifdef CAUTIOUS_ERROR_CHECKING
	//! Whether to check for rigorously check for JSON parsing errors in code which should come from the DDX itself
	#define PATH_PARSING_CHECKS
	#define CAUTIOUS_ALERTS
#endif

// LOGGING
#ifdef ENABLE_DEBUG
#define LOGGING_ENABLE_PRINT_ALL  // Comment to disable printing of all but errors
#endif
#define LOGGING_ENABLE_STDOUT  // Comment to disable printing of log messages to stdout


// PLATFORM SPECIALIZATION


// BEACON REMOVAL
// #define BEACONS


// PROCESSING
#define POSTPROCESS_EVENT_HANDLING_TIMEOUT	200  // Timeout on call to QEventLoop::processEvents() after every Path::process()


// BUFFERING
#define DEFAULT_SLOW_BUFFER_SIZE	1048576  // 1048576 = 1mb
#define DEFAULT_FAST_BUFFER_SIZE	104857600  // 104857600 = 100mb


#endif // CONSTANTS_H
