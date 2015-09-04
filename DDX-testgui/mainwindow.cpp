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

#include "mainwindow.h"
#include "devmgr.h"
#include "testdev.h"
#include "network.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	// Connection menu
	QMenu *m = menuBar()->addMenu(tr("&Devices"));
	newTestDeviceAct = new QAction(tr("Add new &TestDev", "Devices menu"), this);
	newTestDeviceAct->setStatusTip(tr("Add a new test device"));
	connect(newTestDeviceAct, &QAction::triggered, this, &MainWindow::newTestDevice);
	m->addAction(newTestDeviceAct);
	closeAllDevicesAct = new QAction(tr("&Close all", "Devices menu"), this);
	closeAllDevicesAct->setStatusTip(tr("Close all devices"));
	connect(closeAllDevicesAct, &QAction::triggered, this, &MainWindow::closeAllDevices);
	m->addAction(closeAllDevicesAct);
	
	// Log area
	logArea = new QPlainTextEdit();
	logArea->setReadOnly(true);
	logArea->setFont(QFont("Consolas", 12));
	setCentralWidget(logArea);
	logArea->appendPlainText(tr("Started"));
	
	// Initializations
	setWindowTitle(tr("DDX Test GUI"));
	resize(1000,500);
	logArea->appendPlainText(tr("Instantiating DevMgr"));
	dm = new DevMgr(this);
}

MainWindow::~MainWindow()
{
	delete logArea;
}

void MainWindow::closeEvent(QCloseEvent *event) {
	closeAllDevices();
	event->accept();  // ignore() to prevent closing
}

void MainWindow::newTestDevice(bool checked) {
	(void) checked;
	//newTestDeviceAct->setEnabled(false);
	logArea->appendPlainText("Starting test device");
	new TestDev(dm, true);
}

void MainWindow::closeAllDevices(bool checked) {
	(void) checked;
	dm->closeAll();
	newTestDeviceAct->setEnabled(true);
}
