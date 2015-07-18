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

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	s = new QTcpSocket(this);
	t = new QTimer(this);
	t->setInterval(500);
	connect(t, &QTimer::timeout, this, &MainWindow::boop);
	l = new QLabel("Initial");
	this->setCentralWidget(l);
	n = QString::number(QTime::currentTime().msec());
	t->start();
	c = 0;
}

MainWindow::~MainWindow()
{
	
}

void MainWindow::boop() {
	if (s->state() == QAbstractSocket::ConnectedState) {  // Connected
		if (c >= 5) {
			s->disconnectFromHost();
			l->setText("Disconnected");
			return;
		}
		s->write(QString("{\"test\":\"%1\"}\n").arg(n).toLatin1());
		s->flush();
		l->setText(tr("Connected (%1)").arg(n));
		c++;
	}
	else {  // Not connected
		c = 0;
		if (s->state() == QAbstractSocket::HostLookupState
			|| s->state() == QAbstractSocket::ConnectingState)
			return;
		s->connectToHost(QHostAddress(QHostAddress::LocalHost), 4388);
		l->setText(tr("Connection requested (%1)").arg(n));
	}
}
