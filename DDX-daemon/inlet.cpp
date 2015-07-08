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

#include "inlet.h"

Inlet::Inlet(const QString name, Path *parent) : Module(name, parent)
{
	// TODO
}

Inlet::~Inlet()
{
	// TODO
}

void Inlet::init(QJsonObject settings) {
	// TODO
	settings.size();
}

void Inlet::initialReconfigure() {
	// TODO
}

bool Inlet::isSynchronous() const {
	// TODO
	return false;
}

bool Inlet::isFinite() const {
	// TODO
	return false;
}
