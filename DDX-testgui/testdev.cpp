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

#include "testdev.h"
#include "devmgr.h"

#define RAPIDJSON_IO
#include "rapidjson_using.h"

TestDev::TestDev(DevMgr *dm, bool inbound) : RemDev(dm, inbound) {
	eventCt = 0;
}

TestDev::~TestDev() {
	
}

void TestDev::responseHandler(RemDev::Response *r) const {
	QString str("TD response to %1: %2 with %3");
	str.arg(r->id);
	str.arg(r->successful ? "success" : "error");
	str.arg(QString(serializeValue(*r->mainVal)));
	log(str);
	delete r;
}

void TestDev::sub_init() noexcept {
	QTimer *timer = new QTimer(this);
	timer->setTimerType(Qt::CoarseTimer);
	timer->setInterval(1500);
	connect(timer, &QTimer::timeout, this, &TestDev::timeout);
	timer->start();  // Start immediately for registration timeout
}

void TestDev::terminate(DisconnectReason reason, bool fromRemote) noexcept {
	(void) reason;
	(void) fromRemote;
}

void TestDev::writeItem(rapidjson::StringBuffer *buffer) noexcept {
	QString out(tr("RD wrote: "));
	out.append(buffer->GetString());
	delete buffer;
	log(out);
}

void TestDev::timeout() {
	eventCt++;
	char *data = new char[1000];
	
	if (eventCt == 1) {
		log(tr("TD emitting bad data"));
		strcpy(data, "this means nothing\n");
		handleItem(data);
	}
	else if (eventCt == 2) {
		log(tr("TD emitting a valid request"));
		strcpy(data, "{\"jsonrpc\":\"2.0\",\"method\":\"register\"}\n");
		handleItem(data);
	}
	else if (eventCt == 3) {
		delete data;
		log(tr("RD sending a valid request"));
		sendRequest(this, "responseHandler", "method-name");
	}
	else if (eventCt == 4) {
		delete data;
		log(tr("RD sending a valid request with params"));
		Document *doc = new Document;
		Value v(kObjectType);
		v.SetObject();
		v.AddMember("Onething", Value(3829), doc->GetAllocator());
		v.AddMember("Twothing", Value(rapidjson::kTrueType), doc->GetAllocator());
		v.AddMember("final", Value((long long) 3892837592836592835), doc->GetAllocator());
		sendRequest(this, "responseHandler", "method-name", doc, &v);
	}
	else if (eventCt == 5) {
		delete data;
		log(tr("RD sending a valid notification"));
		sendNotification("notif.method");
	}
	else if (eventCt == 6) {
		delete data;
		log(tr("RD sending a valid notification with params"));
		Document *doc = new Document;
		Value v(kObjectType);
		v.SetObject();
		v.AddMember("Onething", Value(3829), doc->GetAllocator());
		v.AddMember("Twothing", Value(rapidjson::kTrueType), doc->GetAllocator());
		v.AddMember("final", Value((long long) 3892837592836592835), doc->GetAllocator());
		sendNotification("notif.methodPARAMS", doc, &v);
	}
	else if (eventCt == 10) {
		delete data;
		log(tr("TD closing"));
		close(RemDev::ConnectionTerminated);
		eventCt = 0;
		//log(tr("TD RESETTING-------------------------------------------------"));
	}
	else delete data;
}
