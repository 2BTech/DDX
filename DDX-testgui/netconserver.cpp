#include "netconserver.h"

NetConServer::NetConServer() : QTcpServer(parent) {
	
}

NetConServer::~NetConServer() {
	
}

void incomingConnection(qintptr socketDescriptor) {
	(void) socketDescriptor;
}
