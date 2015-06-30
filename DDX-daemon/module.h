#ifndef MODULE_H
#define MODULE_H

#include <QObject>
#include <QVariant>
#include "data.h"  // Also for DataDef

class Module : public QObject
{
	Q_OBJECT
public:
	/*!
	 * \brief handleReconfigure
	 * \return Whether successful
	 * This is called when data flow starts and subsequently when an upstream
	 * Module reconfigures.  The code inside is responsible for updating
	 * updating outgoing column structure and adding accessors to columns for
	 * quick access in process().  This function must be failsafe; it can report
	 * errors but cannot halt the data stream.
	 * 
	 * ## Modifying Column Structure
	 * While input columns are determined externally, a Module can redefine its
	 * output columns without inflicting any changes upstream.  The following
	 * operations can be done with ease:
	 * - Removing columns: removeColumn() removes a column from the output; the
	 * incoming value is still available.
	 * - Inserting columns: insertColumn() adds a new column to the output and
	 * automatically adds an entry in the accessor map.
	 * 
	 * ### Column Naming Conventions
	 * Because Column names are meant to be globally unique but human-readable
	 * identifiers within paths, searches are case-insensensitive and duplicates
	 * which vary in case are disallowed.
	 * \sa findColumn() insertColumn() removeColumn() addAccessor()
	 * - Inserting, removing, or rearranging outgoing columns
	 * - Adding accessors to columns for quick access in process()
	 * _This is a virtual function which must be reimplemented._
	 * Rearragement of columns can be done directly on
	 * \sa findColumn() insertColumn() removeColumn() addAccessor() alert()
	 */
	virtual void handleReconfigure();
	virtual void process();
	
	/*!
	 * \brief skip
	 * Called instead of process() whenever this Module is skipped.  Default
	 * implementation simply sets all new columns to empty strings.
	 */
	virtual void skip();
	
	explicit Module(const QString *model, QObject *parent = 0);
	~Module();
	void reconfigure();
	
	void setInputColumnsPtr(const DataDef *c) {inputColumns = c;}
	const DataDef* getOutputColumns() const {return outputColumns;}
	
signals:
	void triggerReconfigure();
	void beacon(QString targets, QVariant msg);
	void sendAlert(QString msg);
	
public slots:
	
private:
	QString name;
	const QString *pathName;
	const DataDef *inputColumns;  // NOT OWNED
	DataDef *newColumns;  // Super and elements owned
	ColumnRefMap* accessMap;
	
protected:
	DataDef *outputColumns;  // Super owned, elements owned by newColumns and inputColumns
	
	/*!
	 * \brief alert
	 * \param msg The message
	 * Echoes a statement to all logging Beacons.
	 */
	void alert(QString msg);
	
	/*!
	 * \brief findColumn
	 * \param name
	 * \return pointer to the Column
	 * Searches through the input columns for a column with the given name.
	 * This search is case-insensitive. See insertColumn() for reasoning.
	 * Returns null if the column could not be found.
	 */
	const Column* findColumn(QString name) const;
	
	/*!
	 * \brief insertColumn
	 * \param name A unique identifier
	 * \param index Position index in the Module's output columns
	 * \return Whether successful
	 * Generates a new column buffer, adds it to the Module's output columns,
	 * and adds a reference to the accessor map.  
	 */
	bool insertColumn(QString name, int index);  // Unsafe outside of handleReconfigure();
	void removeColumn(const Column *c);  // Unsafe outside of handleReconfigure();
	void addAccessor(const Column *c);  // Unsafe outside of handleReconfigure();
	inline bool addAccessor(QString name);  // Unsafe outside of handleReconfigure();
};

#endif // MODULE_H
