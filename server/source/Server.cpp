#include "Server.h"

#include "SocketHandler.h"

#include <QCoreApplication>

#include <QSettings>

void Server::incomingConnection(int socketDescriptor)
{
	emit signalPrintLog("INCOMING CONNECTION TO SOCKET: " + QString::number(socketDescriptor));
	emit signalNewConnection();

	SocketHandler* socketHandler = new SocketHandler(socketDescriptor);

	connect(socketHandler, SIGNAL(signalPrintLog(QString)), this, SIGNAL(signalPrintLog(QString)));
	connect(socketHandler, SIGNAL(signalDumpRequest(QByteArray, QString, QString)), this, SIGNAL(signalDumpRequest(QByteArray, QString, QString)));
	connect(socketHandler, SIGNAL(signalSaveReport(QByteArray, QString)), this, SIGNAL(signalSaveReport(QByteArray, QString)));
	
	connect(this, SIGNAL(signalDisconnectSocket()), socketHandler, SLOT(slotDisconnected()), Qt::DirectConnection);

	connect(socketHandler, SIGNAL(signalDisconnected()), this, SIGNAL(signalDisconnected()));
}

Server::Server(QObject* parent) : QTcpServer(parent)
{
	QSettings settings(qApp->applicationDirPath() + "/config/server.ini", QSettings::IniFormat);
	QString host = settings.value("host", QVariant("127.0.0.1")).toString();
	quint16 port = settings.value("port", QVariant("33333")).toUInt();

	if (m_port != port || m_host.toString() != host)
	{
		m_host = host;
		m_port = port;
	}
}

Server::~Server()
{
	close();
}

QHostAddress Server::host() const
{
	return m_host;
}

quint16 Server::port() const
{
	return m_port;
}

void Server::slotUpdateSettings(const QString& host, const QString& port)
{
	if (m_port != port.toUInt() || m_host.toString() != host)
	{
		emit signalPrintLog("UPDATING SETTINGS");

		bool Listening = isListening();

		close();
		emit signalDisconnectSocket();

		QSettings settings(qApp->applicationDirPath() + "/config/server.ini", QSettings::IniFormat);
		settings.setValue("host", host);
		settings.setValue("port", port);

		m_host = host;
		m_port = port.toUShort();

		emit signalPrintLog("SETTINGS HAVE BEEN UPDATED");

		if (Listening)
		{
			slotServerStart();
		}
	}
}

void Server::slotServerStart()
{
	if (this->listen(m_host, m_port))
	{
		emit signalServerStarted();
		signalPrintLog("server started at " + m_host.toString() + ":" + QString::number(m_port));
	}
	else
	{
		signalPrintLog("Cannot start server");
		close();
		emit signalServerStopped();
		this->moveToThread(QCoreApplication::instance()->thread());
	}
}

void Server::slotServerStop()
{
	if (isListening())
	{
		close();
		emit signalDisconnectSocket();
		emit signalPrintLog("sever stopped");
		this->moveToThread(QCoreApplication::instance()->thread());
	}
}
