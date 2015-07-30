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

#include "exampleinlet.h"

ExampleInlet::ExampleInlet(Path *parent, const QString name) : Inlet(parent, name) {
	std::seed_seq ss({210, QTime::currentTime().msec()});
	rg = std::mt19937(ss);
	ct = 0;
}

ExampleInlet::~ExampleInlet() {
	qDeleteAll(timers);
}

void ExampleInlet::init(const QJsonObject settings) {
	if (settings["Fail_on_init"].toBool())
		terminate("Simulating a failure as requested by settings");
	chance = settings["Reconfigure_chance"].toInt(10);
	QVariantList tl = settings["items"].toArray().toVariantList();
	foreach (const QVariant &timerEntry, tl) {
		QJsonObject te = timerEntry.toJsonObject();
		QTimer *t = new QTimer(this);
		t->setInterval(te["Interval"].toInt(10000));
		connect(t, &QTimer::timeout, this, &ExampleInlet::trigger);
		timers.append(t);
	}
	ctColumn = insertColumn("Index", 0);
	randColumn = insertColumn("Random", 1);
	QString *dummy = insertColumn("Dummy", 2);
	*dummy = QString("this is a column");
}

void ExampleInlet::start() {
	for (int i = 0; i < timers.size(); i++)
		timers.at(i)->start();
}

void ExampleInlet::stop() {
	for (int i = 0; i < timers.size(); i++)
		timers.at(i)->stop();
}

QJsonObject ExampleInlet::publishSettings() const {
	alert("ExampleInlet::publishSettings()");
	QByteArray t("{\"Reconfigure_chance\":{\"t\":\"N\",\"d\":\"Probability of reconfigure on every chance\","
				 "\"default\":10},\"Fail_on_init\":{\"t\":\"B\",\"d\":\"Should the module fail in init()?\","
				 "\"default\":false},\"Timers\":{\"t\":\"C\",\"d\":\"Make as many timers as desirable\","
				 "\"Timer\":{\"t\":\"I\",\"d\":\"A timer which generates a fake data line after every interval\","
				 "\"Interval\":{\"t\":\"N\",\"d\":\"The interval, in milliseconds\",\"default\":10}}}}");
	QJsonDocument d = QJsonDocument::fromJson(t);
	return d.object();
}

QJsonObject ExampleInlet::publishActions() const {
	
}

void ExampleInlet::cleanup() {
	
}

void ExampleInlet::trigger() {
	// TODO:  reconfigure if chance works
	*ctColumn = QString::number(ct++);
	*randColumn = QString::number(rg());
}
