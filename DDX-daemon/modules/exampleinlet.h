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

#ifndef EXAMPLEINLET_H
#define EXAMPLEINLET_H

#include <QObject>
#include <QList>
#include <QTime>
#include <QTimer>
#include <QVariant>
#include <random>
#include "inlet.h"

class Path;

/*!
 * \brief Example inlet class
 * 
 * \ingroup modules
 */
class ExampleInlet final : public Inlet
{
	Q_OBJECT  // Required
public:
	ExampleInlet(Path *parent, const QByteArray &name);
	~ExampleInlet();  // Required
	void init(const QJsonObject settings) override;  // Required
	void start() override;  // Required
	void stop() override;  // Required
	QJsonObject publishSettings() const override;  // Optional
	QJsonObject publishActions() const override;  // Optional
	void cleanup() override;  // Required
	
	
private slots:
	void trigger();
	
private:
	std::mt19937 rg;
	bool allowReconfigure;
	QList<QTimer*> timers;
	int chance;
	bool failOnInit;
	int ct, ct2;
	QByteArray *ctColumn, *randColumn, *inColumn;
};

#endif // EXAMPLEINLET_H
