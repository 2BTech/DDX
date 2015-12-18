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
#include "rapidjson_using.h"

ExampleInlet::ExampleInlet(Path *parent, const QByteArray &name) : Inlet(parent, name) {
	std::seed_seq ss({210, QTime::currentTime().msec(), 34});
	rg = std::mt19937(ss);
	ct = ct2 = 0;
	inColumn = 0;
}

ExampleInlet::~ExampleInlet() {
	qDeleteAll(timers);
}

void ExampleInlet::init(rapidjson::Value &config) {
	// TODO: Rapidjson
	/*if (config["Fail_on_init"].toBool())
		terminate("Simulating a failure as requested by settings");
	chance = config["Reconfigure_chance"].toInt(10);
	QVariantList tl = config["items"].toArray().toVariantList();
	foreach (const QVariant &timerEntry, tl) {
		QJsonObject te = timerEntry.toJsonObject();
		QTimer *t = new QTimer(this);
		t->setInterval(te["Interval"].toInt(10000));
		connect(t, &QTimer::timeout, this, &ExampleInlet::trigger);
		timers.append(t);
	}
	ctColumn = insertColumn("Index", 0);
	randColumn = insertColumn("Random", 1);
	QByteArray *dummyPtr = insertColumn("Dummy", 2);
	QByteArray dummy = config["Dummy_string"].toString().toUtf8();
	*dummyPtr = dummy;*/
}

void ExampleInlet::start() {
	for (int i = 0; i < timers.size(); i++)
		timers.at(i)->start();
}

void ExampleInlet::stop() {
	for (int i = 0; i < timers.size(); i++)
		timers.at(i)->stop();
}

rapidjson::Value ExampleInlet::publishSettings(rapidjson::MemoryPoolAllocator<> &a) const {
	alert("ExampleInlet::publishSettings()");
	// TODO:  Rapidjson
	/*QByteArray t("{\"Reconfigure_chance\":{\"t\":\"int\",\"d\":\"Probability of reconfigure on every chance\","
				 "\"default\":10},\"Fail_on_init\":{\"t\":\"bool\",\"d\":\"Should the module fail in init()?\","
				 "\"default\":false},\"Dummy_string\":{\"t\":\"string\",\"d\":\"The dummy string to insert into a column\","
				 "\"default\":\"this is a column\"},\"Timers\":{\"t\":\"cat\",\"d\":\"Make as many timers as desirable\","
				 "\"Timer\":{\"t\":\"item\",\"d\":\"A timer which generates a fake data line after every interval\","
				 "\"Interval\":{\"t\":\"int\",\"d\":\"The interval, in milliseconds\",\"default\":10}}}}");
	QJsonDocument d = QJsonDocument::fromJson(t);
	return d.object();*/
}

rapidjson::Value ExampleInlet::publishActions(rapidjson::MemoryPoolAllocator<> &a) const {
	// TODO
	return Value(rapidjson::kNullType);
}

void ExampleInlet::cleanup() {
	
}

void ExampleInlet::trigger() {
	int n = abs((int) rg()) % 100;
	if (n <= chance) {
		if (outputColumns.size() == 3) {
			inColumn = &insertColumn("Inserted",2)->c;
			ct2 = 0;
		}
		else {
			removeColumn(findColumn("Inserted"));
			inColumn = 0;
		}
		path->reconfigure();
	}
	*ctColumn = QByteArray::number(++ct);
    *randColumn = QByteArray::number((int)rg());
	if (inColumn)
		*inColumn = QString("Inserted %1 lines ago").arg(ct2++).toUtf8();
	process();
}
