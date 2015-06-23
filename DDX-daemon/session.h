#ifndef SESSION_H
#define SESSION_H

#include <QObject>

class Session : public QObject
{
	Q_OBJECT
public:
	explicit Session(QObject *parent = 0);
	~Session();
	
signals:
	
public slots:
};

#endif // SESSION_H
