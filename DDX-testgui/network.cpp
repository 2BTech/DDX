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

#include "network.h"
#include "netdev.h"
#include "devmgr.h"

Network::Network(MainWindow *parent) : QObject(0)
{
	// Initialization
	mw = parent;
	this->dm = mw->dm;
	server = 0;
	// Connections
	connect(this, &Network::postToLogArea, mw->getLogArea(), &QPlainTextEdit::appendPlainText);
	// SSL Configuration
	sslConfig.setProtocol(QSsl::TlsV1_2);
	// Disable remote certificate request
	sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone);
	// TODO:  Replace these temporary certificates with files
	QByteArray certificateText("-----BEGIN CERTIFICATE-----\nMIIGzTCCBLWgAwIBAgIJAL7oZ7T5jFYeMA0GCSqGSIb3DQEBBQUAMIGfMQswCQYD\nVQQGEwJVUzERMA8GA1UECBMIQ29sb3JhZG8xEDAOBgNVBAcTB0JvdWxkZXIxHTAb\nBgNVBAoTFDJCIFRlY2hub2xvZ2llcyBJbmMuMREwDwYDVQQLEwhERFggVGVhbTEV\nMBMGA1UEAxMMSmFzb24gUHJpbmNlMSIwIAYJKoZIhvcNAQkBFhNqYXNvbnBAdHdv\nYnRlY2guY29tMB4XDTE1MDkwODE5NTA0NFoXDTE4MDYyODE5NTA0NFowgZ8xCzAJ\nBgNVBAYTAlVTMREwDwYDVQQIEwhDb2xvcmFkbzEQMA4GA1UEBxMHQm91bGRlcjEd\nMBsGA1UEChMUMkIgVGVjaG5vbG9naWVzIEluYy4xETAPBgNVBAsTCEREWCBUZWFt\nMRUwEwYDVQQDEwxKYXNvbiBQcmluY2UxIjAgBgkqhkiG9w0BCQEWE2phc29ucEB0\nd29idGVjaC5jb20wggIiMA0GCSqGSIb3DQEBAQUAA4ICDwAwggIKAoICAQDA9vS/\nSqssL6y4Edkty7KKOe/iC6IqW+9s8xpsL67q8PlfcLpvTbhh0wibPa7DYmHEBQo3\n9ykGG4hZ3CmJKv6pLaMJk5X07vA75T/4VChk4uT+D6WuE6362xYG7UZTzVwTc2/+\n+YGl6D+ooV81nxYSbGUBdsJhj+ncS9mJhieGGvOlRhPN0+oaisBbBxYCmFT2+9ha\nQ70yChsaTC9N0c9NRBBR8HAQ2mMGC5uU9QHxdBCGbi/xdyflm8ht4n0Wu2B/SnsV\nISPi3pHxReq6WkWZqooE9xNyBgqOutLE869hkDFY57pnM4aFq+BlKz1Lc47oPWXz\nTNXAvngDnUpk8N2+OeFTJPDJTs1SXDq7o5JLKWhGH6/wxMb8dceWxpFdPmm8jncK\ngQyCcioyRx9sxk2WHofnXxH6oBUaRygxypZldrOyxGD6w2p8nBcrNodwABjO7TQ/\nYGOfw51n0MA/tbuS1yLklDWUv75fhcnI+cQGMTSx7qouPo3mhaYYpQR8dnIA1RF2\noaj5ZUZtZCL0dTuPgI/JwM44qgSqagiVB/45YzfcdVIfESB4KqL1uj+/R4wOdcqr\n+Q3cuxsQ3MXZVojYgen74bCiGBnrnBOEWN5rmW+w9lAcRrpm9L2EOkw8TdaGes04\nMAEkYzmbXSChNi3ikOsU7eexl4w8wKCMxEdaGwIDAQABo4IBCDCCAQQwHQYDVR0O\nBBYEFDEiPrnoE5JpfsBZTxOgG66/WS3fMIHUBgNVHSMEgcwwgcmAFDEiPrnoE5Jp\nfsBZTxOgG66/WS3foYGlpIGiMIGfMQswCQYDVQQGEwJVUzERMA8GA1UECBMIQ29s\nb3JhZG8xEDAOBgNVBAcTB0JvdWxkZXIxHTAbBgNVBAoTFDJCIFRlY2hub2xvZ2ll\ncyBJbmMuMREwDwYDVQQLEwhERFggVGVhbTEVMBMGA1UEAxMMSmFzb24gUHJpbmNl\nMSIwIAYJKoZIhvcNAQkBFhNqYXNvbnBAdHdvYnRlY2guY29tggkAvuhntPmMVh4w\nDAYDVR0TBAUwAwEB/zANBgkqhkiG9w0BAQUFAAOCAgEAiAOkOiAWHf+9Hq83NO64\n8q57mndINq0p6aU7VZN0Btd77zF03zxYVOqzNzAwscRiSDL/PM4buTzaC9wt+3fZ\nA6QGrcgBaz/4kVt2V3qqX04aPMwDoan/qLcNC6b9/QyVI3N6Cgm7W+60r6vElKdb\nd1EfuR9pQ9iXrcENXSfVj6NxKh3pUkByz4X13ovrkP4xDSsoPRXTQuJl7dYB6mMO\n5ERmRJGLgFWuHBVLONgNo52TAldaiu7fW+Wkv/08UfBD0F+aToo7VVJ2s21ef2T4\necGIIYwcOflT3fKHPCmwuNSOldelV7Vljq8WLVgAJI5d/+PlKOfXpXZlMARuov0Y\n+LdUFXSPuCzb3uyOLidu1YNzDkLIjtU6xj5zC7qipa+AVsd0PtzZJ8cSNjxkHrq8\nDIM3q5yx+zpdc0Pis6NiF4a30DparaE9BOZl+WdaKqb/Dysfp/4SAw7txMIv+hNn\nmBnDfso/3afGZ7sp9nkeo34fqnM1LFDw8F2oQDb8/3RqkuduYBlqCo1OC8fQBfIB\nX8pgOg3yi/2owAb9HjACYGxMYW7g+z39Nh0fgpnaY+XH09vezkeQ+bjIUK6wS2Mb\ntxRLPC/VM97hcUx6Qxi51GuGjSZZpq1DwKg0b/avgnfoBmy8dznsefczwBsqVYjN\n755QBb0PnEV512Lq6rTkIGM=\n-----END CERTIFICATE-----\n");
	QSslCertificate certificate(certificateText);
	if (certificate.isNull()) log(tr("Certificate invalid"));
	else sslConfig.setLocalCertificate(certificate);
	QByteArray keyText("-----BEGIN RSA PRIVATE KEY-----\n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\n-----END RSA PRIVATE KEY-----\n");
	QSslKey key(keyText, QSsl::Rsa);
	if (key.isNull()) log(tr("Private key invalid"));
	else sslConfig.setPrivateKey(key);
	// TODO:  Tailor a list of acceptable ciphers and ECs
#if (QT_VERSION < QT_VERSION_CHECK(5, 5, 0))
	sslConfig.setCiphers(QSslSocket::supportedCiphers());
#else
	sslConfig.setCiphers(QSslConfiguration::supportedCiphers());
	sslConfig.setEllipticCurves(QSslConfiguration::supportedEllipticCurves());
#endif
	// Threading
#ifdef NETWORK_THREAD
	QThread *t = new QThread(daemon);
	moveToThread(t);
	connect(t, &QThread::started, this, &Network::init);
	connect(this, &Network::destroyed, t, &QThread::quit);
	connect(t, &QThread::finished, t, &QThread::deleteLater);
	t->start();
#else
	init();
#endif
}

Network::~Network() {
	if (server) delete server;
}

void Network::startServer() {
	if (server) return;
	server = new EncryptedServer(this);
	connect(server, &EncryptedServer::acceptError, this, &Network::handleNetworkError);
	int port = 4388;
	QHostAddress a = QHostAddress::Any;
	if ( ! server->listen(a, port)) {
		// TODO:  Handle this better
		server->deleteLater();
		server = 0;
		log("Starting server failed");
	}
	else log("Started server");
}

void Network::stopServer() {
	if ( ! server) return;
	server->deleteLater();
	server = 0;
	log(tr("Stopped server"));
}

QString Network::sslErrorsToString(const QList<QSslError>& errors) {
	QString out;
	QString sep = tr(", ", "List internal separator");
	QString empty;
	for (int i = 0; i < errors.size(); i++)
		out.append(i ? sep : empty).append(errors.at(i).errorString());
	return out;
}

void Network::init() {
	// TODO: add a QNetworkAccessManager and related stuff so Modules can use the high-level APIs
	
	// Old code
	/*if (sg->v("AllowExternal", SG_NETWORK).toBool())
		a = QHostAddress::Any;
	else {
		if (sg->v("UseIPv6Localhost", SG_NETWORK).toBool())
			a = QHostAddress::LocalHostIPv6;
		else a = QHostAddress::LocalHost;
	}
	if ( ! server->listen(a, port)) {
		// TODO:  Should be alert
		log(tr("Server creation failed with error '%1'.  This is likely "
				 "because another DDX daemon is already running on this machine.")
			  .arg(server->errorString()));
		d->quit(E_TCP_SERVER_FAILED);
		return;
	}*/
}

void Network::shutdown() {
	stopServer();
	deleteLater();
}

/*void Network::handleDisconnection() {
	QHash<QTcpSocket*, NetDev*>::iterator it = cons.begin();
	QTcpSocket *s;
	while (it != cons.end()) {
		if ((*it)->socket()->state() == QAbstractSocket::UnconnectedState) {
			s = (*it)->socket();
			it = cons.erase(it);
			s->deleteLater();
		}
		else ++it;
	}
	for (int i = 0; i < ur_sockets.size();) {
		if (ur_sockets.at(i)->state() == QAbstractSocket::UnconnectedState ) {
			s = ur_sockets.at(i);
			ur_sockets.removeAt(i);
			s->deleteLater();
		}
		else i++;
	}
	log(QString("Disconnected; there are %1 active connections").arg(QString::number(cons.size())));
	//log("Disconnect");
	// This should loop through all active RPC requests and return an error
	// for any that relied on the connection that failed
}*/

void Network::handleNetworkError(QAbstractSocket::SocketError error) {
	// TODO:  Handle server errors
}

void Network::log(const QString msg, bool isAlert) const {
	(void) isAlert;
	QString out("network: ");
	out.append(msg);
	emit postToLogArea(out);
}

void EncryptedServer::incomingConnection(qintptr socketDescriptor) {
	new NetDev(n, n->dm, socketDescriptor);
}
