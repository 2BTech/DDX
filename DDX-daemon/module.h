#ifndef MODULE_H
#define MODULE_H

#include <QObject>

class Module : public QObject
{
	Q_OBJECT
public:
	explicit Module(const QString *def, QObject *parent = 0);
	~Module();
	
signals:
	
public slots:
};

#endif // MODULE_H
