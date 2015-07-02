#ifndef MODULE_H
#define MODULE_H

#include <QObject>
#include <QVariant>
#include "data.h"  // Also for DataDef

/*!
 * \brief An element in a Path which can manipulate and respond to data lines
 */
class Module : public QObject
{
	Q_OBJECT
public:
	virtual void init(QStringList settings);
	
	/*!
	 * \brief Update to a new data structure
	 * 
	 * This is called when data flow starts and subsequently when an upstream
	 * Module reconfigures.  The code inside is responsible for updating
	 * updating outgoing column structure and adding accessors to columns for
	 * quick access in process().  This function must be failsafe; it can report
	 * errors but cannot halt the data stream.  _This is a virtual function
	 * which must be reimplimented._
	 * 
	 * ## Modifying Column Structure
	 * While input columns are determined externally, a Module can redefine its
	 * output columns without inflicting any changes upstream.  The following
	 * operations can be done with ease:
	 * - Removing columns: removeColumn() removes a column from the output; the
	 * incoming value is still available.
	 * - Inserting columns: insertColumn() adds a new column to the output which
	 * must be set in process().
	 * - Rearranging columns: Because DataDef is simply a typedef of
	 * QList<Column*>, QList::swap() can be used to safely reorder columns.
	 * - Renaming columns: Columns can be renamed by removing the original,
	 * inserting a new one, and including copy code in process().
	 * 
	 * ### Column Naming Conventions
	 * Because Column names are meant to be globally unique but human-readable
	 * identifiers within paths, searches are case-insensensitive and duplicates
	 * which vary in case are disallowed.  Be wary, however, of the fact that a
	 * column can be removed by one Module and replaced with another of the same
	 * name by a Module downstream without complaint.  Relying on this hack is
	 * not reccommended.
	 * 
	 * ## Fast Buffer Access
	 * Because findColumn() does a slow linear search through the column list,
	 * it is best to only do this search once and then save a pointer directly
	 * to a column's buffer for reading and/or writing during process().  Once
	 * a column has been found, subclasses should store the result of
	 * Column::buffer() to refer to the result in the future.
	 * 
	 * ## Error Handling
	 * See daemon.h for the DDX philosophy on error handling.
	 * 
	 * This function should be designed to handle any possible errors that occur
	 * without interrupting data flow.  Most errors should be caught in
	 * handleReconfigure() rather than process() to prevent from overloading
	 * Beacons during a data stream.  Errors should be reported with alert().
	 */
	virtual void handleReconfigure();
	
	/*!
	 * \brief Handle a data line
	 * 
	 * This function is the core of a Module's work.  This is where line-by-line
	 * processing occurs.  Any code here can make use of buffer pointers saved
	 * by handleReconfigure() to read from and write to columns.  Upon entering
	 * process, column buffers are already loaded with the values from the next
	 * module upstream.  Any changes to these buffers will then be passed to the
	 * next module downstream.  This function must be failsafe; it can report
	 * errors but cannot halt the data stream.  _This is a virtual function
	 * which must be reimplimented._
	 * 
	 * ## Timeouts
	 * [NOT IMPLEMENTED YET] Paths can apply watchdog timers to Modules.
	 * 
	 * ### Workarounds
	 * In combination with the other virtual functions in Module, a Module can
	 * do almost anything desired.  For example, a Module can spawn a separate
	 * thread and push incoming data lines to a queue for processing (albeit
	 * with the downside that resulting information can't be included back in
	 * the original path).  However, if there really is no way to bypass the
	 * timeout.
	 * 
	 * ## Error Handling
	 * See daemon.h for the DDX philosophy on error handling.
	 * 
	 * Errors should be reported with alert().  This function should be designed
	 * to handle any possible errors that occur without interrupting data flow.
	 * Most errors should be caught in handleReconfigure() rather than process()
	 * to prevent from overloading Beacons during a data stream.
	 */
	virtual void process();
	
	/*!
	 * \brief Called instead of process when a Module is being skipped.
	 * 
	 * [SKIP FUNCTIONALITY NOT YET IMPLEMENTED]  Default implementation simply
	 * sets all inserted columns to empty strings.
	 */
	virtual void skip();
	
	virtual void cleanup();
	
	explicit Module(const QString *model, QObject *parent = 0);
	~Module();
	void reconfigure();
	
	void setInputColumnsPtr(const DataDef *c) {inputColumns = c;}
	const DataDef* getOutputColumns() const {return outputColumns;}
	
signals:
	// TODO:  Figure out a way to trigger reconfigures???  I haven't really thought about that yet
	void triggerReconfigure();
	void beacon(QString targets, QVariant msg);
	void sendAlert(QString msg);
	
public slots:
	
private:
	QString name;
	const QString *pathName;
	const DataDef *inputColumns;  // NOT OWNED
	DataDef *newColumns;  // Super and elements owned
	
protected:
	DataDef *outputColumns;  // Super owned, elements owned by newColumns and inputColumns
	
	/*!
	 * \brief Echo a statement to all logging Beacons.
	 * \param msg The message
	 * 
	 * Alerts are tagged with the name of the Path and Module they come from.
	 */
	void alert(QString msg);
	
	/*!
	 * \brief Get a pointer to a specific input Column
	 * \param name The Column's case-insensitive name
	 * \return A pointer to the Column or 0 if it doesn't exist
	 * 
	 * Searches through the input Columns for a Column with the given name.
	 * This search is case-insensitive. See insertColumn() for reasoning.
	 * Because only input columns are searched, inserted columns will not be
	 * found.  They must be managed with the pointer returned from
	 * insertColumn().
	 */
	const Column* findColumn(QString name) const;
	
	/*!
	 * \brief Generate a new Column and add it to the output Columns
	 * \param name A unique, case-insensitive identifier
	 * \param index Position index in the Module's output columns
	 * \return Whether successful
	 * 
	 * Generates a new column buffer, adds it to the Module's output columns,
	 * and adds a reference to the accessor map.
	 * 
	 * __Unsafe outside of reconfigure() or handleReconfigure()!__
	 */
	QString* insertColumn(QString name, int index);
	
	/*!
	 * \brief removeColumn
	 * \param c
	 * 
	 * TODO
	 * 
	 * __Unsafe outside of reconfigure() or handleReconfigure()!__
	 */
	void removeColumn(const Column *c);  // Unsafe outside of handleReconfigure();
};

#endif // MODULE_H
