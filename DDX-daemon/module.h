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
#include <QStringList>
#include "../rapidjson/include/rapidjson/document.h"
#include "../rapidjson/include/rapidjson/stringbuffer.h"
#include "../rapidjson/include/rapidjson/writer.h"
#include "../rapidjson/include/rapidjson/reader.h"
#include "data.h"

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
 * not recommended.
 * 
 * ## %Module %Settings
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
 * data safely.  Inlets should buffer asynchronous data as necessary to avoid
 * missing incoming data because it's coming in faster than process() can
 * handle it.
 * 
 * TODO:  Write about Path::terminate()
 * 
 * ## Inter-Module Communication
 * Modules can communicate with each other with the use of Path::getModule() and
 * Column#p, although the former is recommended over the latter.
 * 
 * ## %Module Registration
 * Modules must be properly registered before you can use them.  Registration
 * instructions are documented with UnitManager.
 * 
 * ## Memory Management
 * Modules are required to do full memory management because they may be
 * loaded and destroyed many times in a Daemon instance which may run
 * autonomously for years.  Ensure to write complete destructors and set parent
 * pointers when relying on QObject memory management.  Note that all Column
 * instances are already fully managed.
 * 
 * \ingroup daemon
 */
class Module : public QObject
{
	Q_OBJECT
public:
	
	explicit Module(Path *parent, const QByteArray &name);
	
	/*!
	 * \brief Module destructor
	 * 
	 * Modules are required to do full memory management because they may be
	 * loaded and destroyed many times in a Daemon instance which may run
	 * autonomously for years.  Here is the place to do so.  Note that Columns
	 * are already fully managed.
	 */
	~Module();
	
	/*!
	 * \brief Configure the Module for operation
	 * \param config The JSON config tree
	 * 
	 * This function can be reimplemented to offer setup space for a Module.  It
	 * is guaranteed to be called exactly once and prior to the first call to
	 * handleReconfigure().  This function or a later slot **must** call
	 * Path::moduleReady() on #path when it is ready to be started.  Failure to
	 * do so will cause path iniitialization to time out.  _This is a virtual
	 * function which must be reimplemented._
	 * 
	 * ### Error Handling
	 * See the Module class documentation for general information on error
	 * handling.  This function should be designed to handle any possible
	 * errors that occur without interrupting data flow.  However, this is the
	 * one function which _can_ report fatal configuration errors which will
	 * terminate a path before it begins; see terminate().  Errors should be
	 * reported with alert().
	 */
	virtual void init(rapidjson::Value &config);
	
	/*!
	 * \brief Handle a data line
	 * 
	 * This function is the core of a Module's work.  This is where line-by-line
	 * processing occurs.  Any code here can make use of buffer pointers saved
	 * by handleReconfigure() to read from and write to columns.  Upon entering
	 * process(), column buffers are already loaded with the values from the next
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
	virtual void process() = 0;
	
	/*!
	 * \brief Return a JSON tree of settings for this Module
	 * \param a The RapidJSON allocator to use
	 * \return The settings tree
	 * 
	 * See DDX-RPC.md for information on settings trees.
	 */
	virtual rapidjson::Value publishSettings(rapidjson::MemoryPoolAllocator<> &a) const;
	
	virtual rapidjson::Value publishActions(rapidjson::MemoryPoolAllocator<> &a) const;
	
	/*!
	 * \brief Manages reconfiguration
	 * 
	 * Resets columns to their starting state and calls handleReconfigure().
	 */
	void reconfigure();
	
	/*!
	 * \brief Called before destruction
	 * 
	 *   _This is a virtual function which must be
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
	inline const DataDef* getOutputColumns() const {return &outputColumns;}
	
	/*!
	 * \brief Get the Module's name
	 * \return The Module's name
	 */
	inline QByteArray getName() const {return name;}
	
protected:
	Path *path;  //!< Convenience pointer to Path instance
	
	DataDef outputColumns;  // Elements owned by newColumns and inputColumns
	
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
	 * \brief Send a high-level message to the user
	 * \param msg The message
	 * 
	 * Alerts are tagged with the name of the Path and Module they come from.
	 */
	void alert(const QString msg) const;
	
	/*!
	 * \brief Log a low-level event message
	 * \param msg The message
	 * 
	 * Log events are tagged with the name of the Path and Module they come from.
	 */
	void log(const QString msg) const;
	
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
	 * \return A pointer to the created column (must be saved!) or 0
	 * 
	 * Generates a new column buffer, adds it to the Module's output columns,
	 * and adds a reference to the accessor map.  This function will first
	 * search for an existing output Column with the given name.  If one is
	 * found, it returns 0.
	 * 
	 * __Unsafe outside of reconfigure() or handleReconfigure()!__
	 */
	Column *insertColumn(const QString name, int index);
	
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
	QByteArray name;
	
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
