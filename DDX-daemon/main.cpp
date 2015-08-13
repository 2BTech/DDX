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

#include <QCoreApplication>
#include "constants.h"
#include "daemon.h"
#include "logger.h"

#if (QT_VERSION < QT_VERSION_CHECK(5, 4, 0))
#error Qt version 5.4 or greater required
#endif

/*!
 * \brief main
 * \param argc argument count
 * \param argv argument vector
 * \return exit code
 * \sa Daemon::Daemon()
 * 
 * The main function instantiates QCoreApplication and then the Daemon class,
 * which manages all functions of the daemon.  It then begins the event loop.
 */
int main(int argc, char *argv[])
{
	QCoreApplication::setOrganizationName(APP_AUTHOR_FULL);
	QCoreApplication::setOrganizationDomain(APP_AUTHOR_DOMAIN);
	QCoreApplication::setApplicationName(APP_NAME_SHORT);
	QCoreApplication::setApplicationVersion(VERSION_FULL_TEXT);
	qInstallMessageHandler(&globalHandleMessage);
	
	QCoreApplication a(argc, argv);

	Logger::get()->log(APP_NAME " " VERSION_FULL_TEXT);

	Daemon daemon(&a);

	return a.exec();
}
