#ifndef OUTLET_H
#define OUTLET_H

#include <QObject>

class Outlet : public QObject
{
	Q_OBJECT
public:
	explicit Outlet(QObject *parent = 0);
	~Outlet();
	
signals:
	
public slots:
};

#endif // OUTLET_H
