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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QObject>
#include <QMainWindow>
#include <QPlainTextEdit>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QCloseEvent>
#include <QMutex>

class DevMgr;
class TestDev;

class MainWindow : public QMainWindow
{
	Q_OBJECT
	
public:
	MainWindow(QWidget *parent = 0);
	
	~MainWindow();
	
	QPlainTextEdit *getLogArea() const {return logArea;}
	
	void closeEvent(QCloseEvent *event) override;
	
private slots:
	
	void newTestDevice(bool checked = false);
	
	void closeAllDevices(bool checked = false);
	
private:
	
	QPlainTextEdit *logArea;
	
	QAction *newTestDeviceAct;
	
	QAction *closeAllDevicesAct;
	
	DevMgr *dm;
	
	TestDev *td;
	
};

#endif // MAINWINDOW_H
