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

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	// Buttons
	QBoxLayout *buttonLayout = new QBoxLayout(QBoxLayout::LeftToRight);
	QPushButton *b = new QPushButton("TestDev", this);
	buttonLayout->addWidget(b);
	connect(b, &QPushButton::clicked, this, &MainWindow::newTestDevice);
	b = new QPushButton("Do nothing", this);
	buttonLayout->addWidget(b);
	
	// Log area
	logArea = new QPlainTextEdit();
	logArea->setReadOnly(true);
	
	// Master layout
	QBoxLayout *masterLayout = new QBoxLayout(QBoxLayout::TopToBottom);
	masterLayout->addLayout(buttonLayout);
	masterLayout->addWidget(logArea);
	setLayout(masterLayout);
	
	// Initializations
	td = 0;
	dm = new DevMgr(this);
	logArea->appendPlainText("Started\n");
}

MainWindow::~MainWindow()
{
	delete logArea;
}

void MainWindow::newTestDevice(bool checked) {
	checked;
	((QPushButton *) sender())->setEnabled(false);
	logArea->appendPlainText("Starting test device...");
	if ( ! td) td = new TestDev(dm, true);
	logArea->appendPlainText("Test device constructor returned");
}
