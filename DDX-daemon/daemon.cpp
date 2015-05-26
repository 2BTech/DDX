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

#include "daemon.h"

Daemon::Daemon(QObject *parent) : QObject(parent) {
	// Open stdout stream for logging
#ifdef LOGGING_ENABLE_STDOUT
	qout = new QTextStream(stdout);
#endif
	// TODO: Save to file if flag is true (see init)
}

Daemon::~Daemon() {

}

void Daemon::log(const QVariant &msg) {
	QString finalMsg = QDateTime::currentDateTime().toString("[yyyy/MM/dd HH:mm:ss.zzz] ");
	finalMsg.append(msg.toString());
#ifdef LOGGING_ENABLE_STDOUT
	*qout << finalMsg << endl;
#endif
}

void Daemon::init() {
	// TODO
	log("[Insert initialization code here]");

	// Determine whether log should be saved to file (look for a flag in the command parser)

	// Load preferences

	// Check for other daemon instances

	// Look for open GUI instance

	// Check for updates (once a week, even if app is running - figure that out)

	// Set up email notifications?  Twitter uploading?

	// Try connecting to instruments
}
