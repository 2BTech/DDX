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
 * prior to use.  Settings can be reported with publishSettings(), which returns
 * a QJsonObject with a specific format.  TODO: Finish
 * 
 * ## Module-Module Communication
 * Modules can communicate with each other with the use of Path::getModule() and
 * Column#p, although the former is recommended over the latter.
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
	 */
	virtual void init(QJsonObject settings);
	
	/*!
	 * \brief Update to a new data structure
	 * 
	 * This is called when data flow starts and subsequently when an upstream
	 * Module reconfigures.  The code inside is responsible for updating
	 * outgoing column structure and adding accessors to columns for
	 * quick access in process().  This function must be failsafe; it can report
	 * errors but cannot halt the data stream.  _This is a virtual function
	 * which must be reimplemented._
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
	 * which must be reimplemented._
	 * 
	 * ### Error Handling
	 * See daemon.h for the DDX philosophy on error handling.
	 * 
	 * Errors should be reported with alert().  This function should be designed
	 * to handle any possible errors that occur without interrupting data flow.
	 * Most errors should be caught in handleReconfigure() rather than process()
	 * to prevent from overloading Beacons during a data stream.
	 */
	virtual void process();
	
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
	 * \brief Return a JSON tree of settings for this Module
	 * \return The settings tree
	 * 
	 * ### Settings Tree Format
	 * The settings tree can have any combination of the following elements:
	 * - Attribute:  A string setting
	 * - Item:  An element which can be duplicated and rearranged, each with a
	 * unique name and its own elements
	 * - Category:  A subgroup of elements
	 * 
	 * Every element is represented as a JSON object with a key of "A", "I", or
	 * "C" depending on the type of element.  All objects, regardless of type,
	 * are required to have an "n" string, which is the unique name of the
	 * element.  All objects, regardless of type, can optionally have a "d"
	 * string, which is a description.  Descriptions are translatable with tr(),
	 * but names are not.  All elements retain their order when reported back to
	 * init().  
	 * 
	 * #### Attributes
	 * Attributes can have a "default" string, otherwise the default will be an
	 * empty string.  When reported to init(), they will be string members of
	 * their parent object with their name as the key.
	 * 
	 * #### Items
	 * Items can be duplicated and and individually configured.
	 * 
	 * #### Categories
	 * Categories are purely aesthetic.  Any element with an "A", "I", or "C"
	 * key after the "n" and "d" elements will be listed as a subelement.  When
	 * reported to init(), they will be objects with their name as a key and
	 * their subelements as actual subelements.
	 */
	virtual QJsonObject publishSettings();
	
	explicit Module(const QString *model, Path *parent = 0);
	~Module();
	
	/*!
	 * \brief Manages reconfiguration
	 * 
	 * Resets columns to their starting state and calls handleReconfigure().
	 */
	void reconfigure();
	
	/*!
	 * \brief Set pointer to external input columns (for Path linkage)
	 * \param c The external input ::DataDef
	 */
	void setInputColumnsPtr(const DataDef *c) {inputColumns = c;}
	
	/*!
	 * \brief Get pointer to internal output columns (for Path linkage)
	 * \return The internal output ::DataDef
	 */
	const DataDef* getOutputColumns() const {return outputColumns;}
	
signals:
	// TODO:  Figure out a way to trigger reconfigures???  I haven't really thought about that yet
	void triggerReconfigure();
	void beacon(QStringList targets, QString msg);
	void sendAlert(QString msg);
	
protected:
	QString name;
	Path *path;
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
	 * This search is case-insensitive; see handleReconfigure() for reasoning.
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
	 * and adds a reference to the accessor map.  This function will first
	 * search for an existing output Column with the given name.  If one is
	 * found, it returns 0.
	 * 
	 * __Unsafe outside of reconfigure() or handleReconfigure()!__
	 */
	QString* insertColumn(QString name, int index);
	
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
	
private:
	const DataDef *inputColumns;  // NOT OWNED
	DataDef *newColumns;  // Super and elements owned
};

#endif // MODULE_H
