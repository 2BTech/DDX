/******************************************************************************
 *                         DATA DISPLAY APPLICATION X                         *
 *                            2B TECHNOLOGIES, INC.                           *
 *                                                                            *
 * The DDX is free software: you can redistribute it and/or modify it under   *
 * the terms of the GNU General Public License as published by the Free       *
 * Software Foundation, either version 3 of the License, or (at your option)  *
 * any later version.  The DDX is distributed in the hope that it will be     *
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty of     *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General  *
 * Public License for more details.  You should have received a copy of the   *
 * GNU General Public License along with the DDX.  If not, see                *
 * <http://www.gnu.org/licenses/>.                                            *
 *                                                                            *
 *  For more information about the DDX, check out the 2B website or GitHub:   *
 *       <http://twobtech.com/DDX>       <https://github.com/2BTech/DDX>      *
 ******************************************************************************/

#ifndef MODULE_H
#define MODULE_H

#include <QObject>
#include <QJsonObject>
#include <QStringList>
#include "data.h"
#include "path.h"

class Path;

/*!
 * \brief An element in a Path which can manipulate and respond to data lines
 * 
 * ## %Module Execution Flow Overview
 * Module operation is managed by four virtual functions:
 * - init() handles any initialization that needs to happen prior to anything
 * else.  It is called exactly once and only after construction and before the
 * first call to handleReconfigure().
 * - handleReconfigure() alerts the Module that a new data format is available.
 * It can be called multiple times and will be called at least once prior to
 * calling process() and cleanup().
 * - process() operates on a data line.  It can be called multiple times between
 * calls to handleReconfigure.
 * - cleanup() is called immediately before destruction.
 * 
 * ## Modifying %Column Structure
 * While input columns are determined externally, a Module can redefine its
 * output columns without inflicting any changes upstream.  The following
 * operations can be done with ease within handleReconfigure():
 * - Removing columns: removeColumn() removes a column from the output; the
 * incoming value is still available.
 * - Inserting columns: insertColumn() adds a new column to the output which
 * must be set in process().
 * - Rearranging columns: Because ::DataDef is simply a typedef of
 * QList<Column*>, QList::swap() can be used to safely reorder columns.
 * - Renaming columns: Columns can be renamed by removing the original,
 * inserting a new one, and including copy code in process().
 * 
 * ### %Column Naming Conventions
 * Because Column names are meant to be globally unique but human-readable
 * identifiers within paths, searches are case-insensensitive and duplicates
 * which vary in case are disallowed.  Be wary, however, of the fact that a
 * column can be removed by one Module and replaced with another of the same
 * name by a Module downstream without complaint.  Relying on this hack is
 * not reccommended.
 * 
 * ## %Module Settings
 * Modules can publish a tree of settings which must be configured by someone
 * prior to use.  Settings can be reported with publishSettings(), which records
 * what settings this Module allows to be set.  Configured settings are passed
 * into init().  See Path for information on how settings can be live-tested.
 * 
 * ## Error Handling
 * The DDX philosophy on error handling is to report everything but never fail.
 * All functions in this class and most others are forcibly failsafe in the
 * sense that they do not have a way of halting execution.  This is done on
 * purpose so as to prevent data loss because of software problems.  Care should
 * be taken to ensure that code is bug-free by catching all possible problems,
 * reporting them with alert(), and gracefully handling them.  Misconfigured
 * settings should be reported in init() in the hopes they will be caught in
 * Path tests.  Bad data structure should be reported and any necessary state
 * saved in handleReconfigure() so that data can pass through process() without
 * causing a crash.  process() should be able to deal with incorrect or missing
 * data safely.  Inlets should buffer asynchronous data.
 * 
 * ## Module-Module Communication
 * Modules can communicate with each other with the use of Path::getModule() and
 * Column#p, although the former is recommended over the latter.
 * 
 * ## %Module Registration
 * Modules must be properly registered before you can use them.  Registration
 * instructions are documented with UnitManager.
 */
class Module : public QObject
{
	Q_OBJECT
public:
	
	/*!
	 * \brief Configure the Module for operation
	 * \param settings The JSON settings tree
	 * 
	 * This function can be reimplemented to offer setup space for a Module.  It
	 * is guaranteed to be called exactly once and prior to the first call to
	 * handleReconfigure().  _This is a virtual function which must be
	 * reimplemented._
	 * 
	 * ### Error Handling
	 * See the Module class documentation for general information on error
	 * handling.  This function should be designed to handle any possible
	 * errors that occur without interrupting data flow.  Catch as many errors
	 * as possible here.  Errors should be reported with alert().
	 */
	virtual void init(const QJsonObject settings);
	
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
	 * which must be reimplemented._
	 * 
	 * ### Error Handling
	 * See the Module class documentation for general information on error
	 * handling.  This function should be designed to handle any possible
	 * errors that occur without interrupting data flow.  Most errors should be
	 * caught in handleReconfigure() rather than process() to prevent from
	 * overloading Beacons during a data stream.  Errors should be reported with
	 * alert().
	 */
	virtual void process();
	
	/*!
	 * \brief Return a JSON tree of settings for this Module
	 * \return The settings tree
	 * 
	 * ### Settings Tree Format
	 * The settings tree is a JSON object with a key of "A", "I", or "C"
	 * depending on the type of element.  All elements, regardless of type, are
	 * required to have an "n" string, which is the unique name of the element.
	 * When displayed in the GUI, underscores in names will be replaced with
	 * spaces.  All elements, regardless of type, can optionally have a "d"
	 * string, which is a description.  Descriptions are translatable with tr(),
	 * but names should remain consistent between platforms.  The tree can have
	 * any combination of the following elements:
	 * - _Attribute_:  A string setting.  Attributes can have a "default"
	 * string, otherwise the default will be an empty string.  When reported to
	 * init(), they will be string members of their parent object with their
	 * name as the key.  Attributes may not have their own subelements.
	 * - _Item_:  An element which can be duplicated and individually configured,
	 * each with a unique name.  Items can have any number of subelements; any
	 * element with an "A", "I", or "C" key after the "n" and "d" elements will
	 * be listed as a subelement on each instance of an item.  When reported to
	 * init(), items will appear in a JSON array with the key "items" to
	 * guarantee they retain their user-assigned order.  Each declared item will
	 * be an object with an "n" pair indicating the user-assigned unique name
	 * and a "t" pair indicating the item type corresponding to a reported
	 * item's "n" string.  Any subelements will follow the "t" pair.  Any
	 * category in which items were declared will always have an "items" array
	 * when returned, even if no items were declared.
	 * - _Category_:  A purely aesthetic subgroup of elements.  Any element with
	 * an "A", "I", or "C" key after the "n" and "d" elements will be listed as
	 * a subelement.  When reported to init(), they will be objects with their
	 * name as a key and their subelements as actual subelements.  When
	 * displayed in the configuration GUI, any attributes directly under a
	 * category will be shown as attributes of the category itself.
	 * 
	 * When reporting back to init(), all attributes will appear in declaration
	 * order at the front of their parent category.  All subcategories will
	 * follow in the order they were declared.  Then all items will appear
	 * following these categories in the order they were instantiated by the
	 * user.  A Module's instance is shown as the master category in the
	 * configuration GUI, so attributes in the highest level will appear to be
	 * attributes directly affecting the module (rather than a category or
	 * item).
	 * 
	 * Settings must not be named "A", "I", "C", "n", "t", "d", "default", or
	 * "items".  Duplicate names are checked case-insensitively, so variations
	 * on capitalization of any reserved keywords will also fail.  It is
	 * recommendend that you (and users) avoid non-alphanumeric characters in
	 * names with the exceptions of hypens and underscores.
	 * 
	 * TODO:  Add booleans and selects???
	 * 
	 * Here is an example of a settings tree:
	 ~~~{.json}
	 * {
	 *   "Flow_Rate": {
	 *     "t": "A",
	 *     "d": "Sample flow rate in l/min",
	 *     "default": "10"
	 *   },
	 *   "Analog_Inputs": {
	 *     "t": "C",
	 *     "d": "List any analog inputs here",
	 *     "Voltage_AI": {
	 *       "t": "I",
	 *       "d": "A voltage-based analog input",
	 *       "Max_Voltage": {
	 *         "t": "A"
	 *       },
	 *       "Min_Voltage": {
	 *         "t": "A"
	 *       }
	 *     },
	 *     "Current_AI": {
	 *       "t": "I",
	 *       "d": "A current-based analog input",
	 *       "Max_Current": {
	 *         "t": "A"
	 *       },
	 *       "Min_Current": {
	 *         "t": "A"
	 *       }
	 *     }
	 *   }
	 * }
	 ~~~
	 * And an example response:
	 ~~~{.json}
	 * {
	 *   "Flow_Rate": "12",
	 *   "Analog_Inputs": {
	 *     "items": [
	 *       {
	 *         "n": "Temperature Sensor",
	 *         "t": "Voltage_AI",
	 *         "Max_Voltage": "3.3",
	 *         "Min_Voltage": "0"
	 *       },
	 *       {
	 *         "n": "Power Meter",
	 *         "t": "Current_AI",
	 *         "Max_Current": "20",
	 *         "Min_Current": "0"
	 *       },
	 *       {
	 *         "n": "Barometer",
	 *         "t": "Voltage_AI",
	 *         "Max_Voltage": "2",
	 *         "Min_Voltage": "1"
	 *       }
	 *     ]
	 *   }
	 * }
	 ~~~
	 */
	virtual QJsonObject publishSettings() const;
	
	virtual QJsonObject publishActions() const;
	
	explicit Module(Path *parent, const QString name);
	
	~Module();
	
	/*!
	 * \brief Manages reconfiguration
	 * 
	 * Resets columns to their starting state and calls handleReconfigure().
	 */
	void reconfigure();
	
	/*!
	 * \brief Called before destruction
	 * 
	 * Modules are required to do full memory management because they may be
	 * loaded and destroyed many times in a Daemon instance which may run
	 * autonomously for years.  Here is the place to do so.  Note that Columns
	 * are already fully managed.  _This is a virtual function which must be
	 * reimplemented._
	 */
	virtual void cleanup();
	
	/*!
	 * \brief Set pointer to external input columns (for Path linkage)
	 * \param c The external input ::DataDef
	 */
	inline void setInputColumnsPtr(const DataDef *c) {inputColumns = c;}
	
	/*!
	 * \brief Get pointer to internal output columns (for Path linkage)
	 * \return The internal output ::DataDef
	 */
	inline const DataDef* getOutputColumns() const {return outputColumns;}
	
	/*!
	 * \brief Get the Module's name
	 * \return The Module's name
	 */
	inline QString getName() const {return name;}
	
protected:
	Path *path;
	
	DataDef *outputColumns;  // Super owned, elements owned by newColumns and inputColumns
	
	/*!
	 * \brief Update to a new data structure
	 * 
	 * This is called when data flow starts and subsequently when an upstream
	 * Module reconfigures.  The code inside is responsible for updating
	 * outgoing column structure and adding accessors to columns for
	 * quick access in process().  _This is a virtual function which must be
	 * reimplemented._
	 * 
	 * ### Initial State
	 * Prior to every call, previously inserted Columns are destroyed and the
	 * input column structure is copied to the output column structure.
	 * 
	 * ### Fast Buffer Access
	 * Because findColumn() does a slow linear search through the column list,
	 * it is best to only do this search once and then save a pointer directly
	 * to a column's buffer for reading and/or writing during process().  Once
	 * a column has been found, subclasses should store the result of
	 * Column::buffer() to refer to the result in the future.
	 * 
	 * ### Error Handling
	 * See the Module class documentation for general information on error
	 * handling.  This function should be designed to handle any possible
	 * errors that occur without interrupting data flow.  Most errors should be
	 * caught in handleReconfigure() rather than process() to prevent from
	 * overloading Beacons during a data stream.  Errors should be reported with
	 * alert().
	 */
	virtual void handleReconfigure();
	
	/*!
	 * \brief Echo a statement to all logging Beacons.
	 * \param msg The message
	 * 
	 * Alerts are tagged with the name of the Path and Module they come from.
	 */
	void alert(const QString msg) const;
	
	/*!
	 * \brief Get a pointer to a specific input Column
	 * \param name The Column's case-insensitive name
	 * \return A pointer to the Column or 0 if it doesn't exist
	 * 
	 * Searches through the input Columns for a Column with the given name.
	 * This search is case-insensitive; see handleReconfigure() for reasoning.
	 * Because only input columns are searched, inserted columns will not be
	 * found.  They must be managed with the pointer returned from
	 * insertColumn().
	 */
	Column* findColumn(const QString name) const;
	
	/*!
	 * \brief Generate a new Column and add it to the output Columns
	 * \param name A unique, case-insensitive identifier
	 * \param index Position index in the Module's output columns
	 * \return A pointer to the created buffer (must be saved!) or 0
	 * 
	 * Generates a new column buffer, adds it to the Module's output columns,
	 * and adds a reference to the accessor map.  This function will first
	 * search for an existing output Column with the given name.  If one is
	 * found, it returns 0.
	 * 
	 * __Unsafe outside of reconfigure() or handleReconfigure()!__
	 */
	QString* insertColumn(const QString name, int index);
	
	/*!
	 * \brief Remove an output Column
	 * \param c The Column to be removed
	 * 
	 * Data in a column which was also an input Column is still accessible via
	 * findColumn().
	 * 
	 * __Unsafe outside of reconfigure() or handleReconfigure()!__
	 */
	void removeColumn(const Column *c);  // Unsafe outside of handleReconfigure();
	
	void terminate(const QString msg);
	
private:
	
	//! This Module's name (not editable after construction)
	QString name;
	
	//! Pointer to external input columns (this is not owned!)
	const DataDef *inputColumns;  // NOT OWNED
	
	/*!
	 * \brief List of Columns created by this Module for garbage collection
	 * 
	 * Note that this will be a null pointer unless a Column was previously
	 * inserted by a Module.
	 */
	DataDef *newColumns;  // Super and elements owned
	
	//! Garbage collects inserted Columns
	inline void emptyNewColumns();
};

#endif // MODULE_H
