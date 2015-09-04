#ifndef NETCONSERVER_H
#define NETCONSERVER_H

#include <QObject>
#include <QTcpServer>
#include "network.h"

class NetConServer : public QTcpServer
{
	Q_OBJECT
public:
	friend class Network;
	
	~NetConServer();
	
private:
	NetConServer(Network *parent);
	
	void incomingConnection(qintptr socketDescriptor) override;
	
};

#endif // NETCONSERVER_H
