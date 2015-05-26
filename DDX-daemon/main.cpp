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
#include <QTextStream>  // Temporary debug console output
#include <QTimer>  // For calling init() after exec()
#include "../DDX-gui/constants.h"
#include "daemon.h"


int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	QCoreApplication::setOrganizationName("2B Technologies");
	QCoreApplication::setOrganizationDomain("twobtech.com");
	QCoreApplication::setApplicationName("DDX");
	QCoreApplication::setApplicationVersion(VERSION_FULL_TEXT);

	Daemon daemon(&a);

	daemon.log("2B DDX " VERSION_FULL_TEXT);

	// Begin execution
	QTimer::singleShot(0, &daemon, &Daemon::init);
	return a.exec();
}
