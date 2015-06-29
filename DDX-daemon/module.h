#ifndef MODULE_H
#define MODULE_H

#include <QObject>
#include "data.h"  // Also for DataDef

class Module : public QObject
{
	Q_OBJECT
public:
	virtual void process();
	virtual bool handleReconfigure();
	
	explicit Module(const QString *model, QObject *parent = 0);
	~Module();
	const DataDef* reconfigure();
	
	const DataDef* getInputColumns() const {return inputColumns;}
	const DataDef* getOutputColumns() const {return outputColumns;}
	
signals:
	
public slots:
	
private:
	const DataDef *inputColumns;  // NOT OWNED
	DataDef *outputColumns;  // Owned
	ColumnRefMap* savedColumns;
	
protected:
	inline void saveColumn(Column* c);
	void insertColumn();  // Unsafe outside of reconfigure();
	void removeColumn();  // Unsafe outside of reconfigure();

};

#endif // MODULE_H
