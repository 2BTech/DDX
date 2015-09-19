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

#ifndef EXAMPLEMODULE_H
#define EXAMPLEMODULE_H

#include <QObject>
#include "module.h"

class Path;

/*!
 * \brief Example module class
 * 
 * \ingroup modules
 */
class ExampleModule final : public Module
{
	Q_OBJECT  // Required
public:
	using Module::Module;  // Required
	~ExampleModule();  // Required
	void init(rapidjson::Value &config) override;  // Required
	void process() override;  // Required
	rapidjson::Value publishSettings(rapidjson::MemoryPoolAllocator<> &a) const override;  // Optional
	rapidjson::Value publishActions(rapidjson::MemoryPoolAllocator<> &a) const override;  // Optional
	void cleanup() override;  // Required
	void handleReconfigure() override;  // Required
private:
	QString echo;
};

#endif // EXAMPLEMODULE_H
