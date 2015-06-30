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

#ifndef DATA_H
#define DATA_H

#include <QObject>
#include <QList>
#include <QString>

/*!
 * \brief The Column struct
 * Stores the contents of and metadata about a column as it resides in a stream.
 */
struct Column {
	QString c;  //!< The column's actual data buffer
	QString n; //!< The name and main identifier of the column as reported by its parent
	const QObject *p;  //!< A pointer to the column's parent, either a Module or Inlet
	
	Column(QString name, const QObject *parent) {
		QString c;
		QString n(name);
		p = parent;
	}
	
	QString* buffer() {return &c;}
};

/*!
 * \brief DataDef
 * An ordered representation of Columns, which can model the format of data at
 * any point in the stream.
 */
typedef QList<Column*> DataDef;

#endif // DATA_H
