#ifndef INLET_H
#define INLET_H

#include <QObject>

class Inlet : public QObject
{
	Q_OBJECT
public:
	explicit Inlet(QObject *parent = 0);
	~Inlet();
	
signals:
	
public slots:
};

#endif // INLET_H
