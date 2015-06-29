#ifndef MODULE_H
#define MODULE_H

#include <QObject>
#include "data.h"  // Also for DataDef

class Module : public QObject
{
	Q_OBJECT
public:
	virtual bool handleReconfigure();
	virtual void process();
	virtual void skip();
	
	explicit Module(const QString *model, QObject *parent = 0);
	~Module();
	const DataDef* reconfigure();
	
	const DataDef* getInputColumns() const {return inputColumns;}
	const DataDef* getOutputColumns() const {return outputColumns;}
	
signals:
	
public slots:
	
private:
	const DataDef *inputColumns;  // NOT OWNED
	DataDef *outputColumns;  // Super owned, elements owned by newColumns and inputColumns
	DataDef *newColumns;  // Super and elements owned
	ColumnRefMap* accessMap;
	
protected:
	const Column* findColumn(QString name) const;
	
	void insertColumn(int index);  // Unsafe outside of handleReconfigure();
	void removeColumn(Column *c);  // Unsafe outside of handleReconfigure();
	void addAccessor(Column *c);  // Unsafe outside of handleReconfigure();
	inline bool addAccessor(QString n);  // Unsafe outside of handleReconfigure();
};

#endif // MODULE_H
