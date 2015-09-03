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
	lastInvalidId = 999;
}

TestDev::~TestDev() {
	
}

void TestDev::responseHandler(RemDev::Response *r) {
	validResponses.removeAll(r->id);
	QString str("TD response to %1 (%4): %2 with %3");
	str = str.arg(r->id);
	str = str.arg(r->successful ? "success" : "error");
	// DEBUG
	str = str.arg(QString(serializeValue(*r->mainVal)));
	str = str.arg(QString(r->method));
	log(str);
	delete r;
}

void TestDev::requestHandler(RemDev::Request *r) {
	QString str("TD %1 to %3 with %2");
	if (r->isRequest())
		str = str.arg(QString("request [ID %1]").arg(r->id->GetInt()));
	else
	str = str.arg("notification");
	if (r->params) str = str.arg(QString(serializeValue(*r->params)));
	else str = str.arg("no params");
	str = str.arg(QString(r->method));
	log(str);
	delete r;
}

void TestDev::sub_init() noexcept {
	QTimer *timer = new QTimer(this);
	timer->setTimerType(Qt::CoarseTimer);
	timer->setInterval(1000);
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
	char data[1000];
	
	if (eventCt == 1) {
		log(tr("RD sending three valid requests"));
		validResponses.append(sendRequest(this, "responseHandler", "validMethod"));
		validResponses.append(sendRequest(this, "responseHandler", "validMethod"));
		validResponses.append(sendRequest(this, "responseHandler", "validMethod"));
	}
	else if (eventCt == 2) {
		log(tr("TD emitting successful response"));
		QString out("{\"jsonrpc\":\"2.0\",\"id\":%1,\"result\":true}");
		QByteArray buff = out.arg(validResponses.takeFirst()).toUtf8();
		strcpy(data, buff.constData());
		handleItem(data);
	}
	else if (eventCt == 4) {
		log(tr("TD emitting response with bad member"));
		strcpy(data, "{\"jsonrpc\":\"2.0\",\"id\":213,\"result\":true,\"jsonrpc33\":\"2.33\"}");
		handleItem(data);
	}
	else if (eventCt == 4) {
		log(tr("TD emitting response with string id"));
		strcpy(data, "{\"jsonrpc\":\"2.0\",\"id\":\"213\",\"result\":true}\n");
		handleItem(data);
	}
	else if (eventCt == 5) {
		log(tr("TD emitting response with no id"));
		strcpy(data, "{\"jsonrpc\":\"2.0\",\"result\":true}\n");
		handleItem(data);
	}
	else if (eventCt == 6) {
		log(tr("TD emitting response with two ids"));
		strcpy(data, "{\"jsonrpc\":\"2.0\",\"id\":213,\"id\":4567,\"result\":true}\n");
		handleItem(data);
	}
	else if (eventCt == 7) {
		log(tr("TD emitting response with no mainVal"));
		strcpy(data, "{\"jsonrpc\":\"2.0\",\"id\":213}\n");
		handleItem(data);
	}
	else if (eventCt == 8) {
		log(tr("TD emitting response with result and error"));
		strcpy(data, "{\"jsonrpc\":\"2.0\",\"id\":213,\"result\":true,\"error\":{\"code\":564,\"message\":\"This is an error\"}}\n");
		handleItem(data);
	}
	else if (eventCt == 9) {
		log(tr("TD emitting response with non-object error"));
		strcpy(data, "{\"jsonrpc\":\"2.0\",\"id\":213,\"error\":true}\n");
		handleItem(data);
	}
	else if (eventCt == 10) {
		log(tr("TD emitting response with no error code"));
		strcpy(data, "{\"jsonrpc\":\"2.0\",\"id\":213,\"error\":{\"message\":\"This is an error\"}}\n");
		handleItem(data);
	}
	else if (eventCt == 11) {
		log(tr("TD emitting response with non-int error code"));
		strcpy(data, "{\"jsonrpc\":\"2.0\",\"id\":213,\"error\":{\"code\":18446744073709551615,\"message\":\"This is an error\"}}\n");
		handleItem(data);
	}
	else if (eventCt == 12) {
		log(tr("TD emitting response with no error message"));
		strcpy(data, "{\"jsonrpc\":\"2.0\",\"id\":213,\"error\":{\"code\":564,\"data\":\"This is an error\"}}\n");
		handleItem(data);
	}
	else if (eventCt == 13) {
		log(tr("TD emitting response with non-string error message"));
		strcpy(data, "{\"jsonrpc\":\"2.0\",\"id\":213,\"error\":{\"code\":564,\"message\":null}}\n");
		handleItem(data);
	}
	else if (eventCt == 14) {
		log(tr("TD emitting successful null-id response"));
		strcpy(data, "{\"jsonrpc\":\"2.0\",\"id\":null,\"result\":{\"code\":564,\"message\":null}}\n");
		handleItem(data);
	}
	else if (eventCt == 15) {
		log(tr("TD emitting null-id error"));
		strcpy(data, "{\"jsonrpc\":\"2.0\",\"id\":null,\"error\":{\"code\":564,\"message\":\"This is an error\"}}\n");
		handleItem(data);
	}
	else if (eventCt == 16) {
		log(tr("TD emitting null-id error"));
		strcpy(data, "{\"jsonrpc\":\"2.0\",\"id\":null,\"error\":{\"code\":564,\"message\":\"This is an error\"}}\n");
		handleItem(data);
	}
	else if (eventCt == 17) {
		log(tr("TD emitting unrequested response"));
		QString out("{\"jsonrpc\":\"2.0\",\"id\":%1,\"result\":true}");
		QByteArray buff = out.arg(getInvalidId()).toUtf8();
		strcpy(data, buff.constData());
		handleItem(data);
	}
	else if (eventCt == 18) {
		log(tr("TD emitting valid error"));
		QString out("{\"jsonrpc\":\"2.0\",\"id\":%1,\"error\":{\"code\":564,\"message\":\"This is an error\",\"data\":[49,402,403]}}");
		QByteArray buff = out.arg(validResponses.takeFirst()).toUtf8();
		strcpy(data, buff.constData());
		handleItem(data);
	}
	else if (eventCt == 19) {
		//log(tr("TD closing"));
		close(RemDev::ConnectionTerminated);
		eventCt = 0;
		//log(tr("TD RESETTING-------------------------------------------------"));
	}
}

int TestDev::getInvalidId() {
	do lastInvalidId++;
	while (validResponses.contains(lastInvalidId));
	return lastInvalidId;
}

#ifndef QT_DEBUG
void TestDev::printReqs() const {
	Document doc;
	rapidjson::MemoryPoolAllocator<> &a = doc.GetAllocator();
	doc.SetArray();
	req_id_lock.lock();
	for (RequestHash::ConstIterator it = reqs.constBegin(); it != reqs.constEnd(); ++it) {
		Value v(kArrayType);
		v.PushBack(Value(it.key()), a);
		QByteArray time = QDateTime::fromMSecsSinceEpoch(it->timeout_time).toString("HH:mm:ss.zzz").toUtf8();
		Value s;
		s.SetString(time.constData(), a);
		v.PushBack(s, a);
		s.SetString(rapidjson::StringRef(it->handlerFn));
		v.PushBack(s, a);
		if (it->valid(0)) {
			const char *name = it->handlerObj->metaObject()->className();
			s.SetString(rapidjson::StringRef(name));
			v.PushBack(s, a);
			v.PushBack(Value(rapidjson::kTrueType), a);
		}
		else v.PushBack(Value(rapidjson::kFalseType), a);
		doc.PushBack(v, a);
	}
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);
	log(buffer.GetString());
	req_id_lock.unlock();
}

QByteArray TestDev::serializeValue(const rapidjson::Value &v) {
	Document doc;
	doc.CopyFrom(v, doc.GetAllocator());
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);
	QByteArray array(buffer.GetString());
	Q_ASSERT(buffer.GetSize() == (uint) array.size());
	return array;
}
#endif  // QT_DEBUG
