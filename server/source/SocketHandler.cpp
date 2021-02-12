#include "SocketHandler.h"

#include "Handler.h"

#include <QFile>

#include <QStringList>
#include <QHostAddress>

SocketHandler::SocketHandler(int socketDescriptor, QObject* parent) : QObject(parent), m_socketDescriptor(socketDescriptor)
{
	m_socket = new QTcpSocket;
	m_socket->setSocketDescriptor(m_socketDescriptor);

	connect(m_socket, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
	connect(m_socket, SIGNAL(disconnected()), this, SLOT(slotDisconnected()), Qt::DirectConnection);

	//connect(&requestParser, SIGNAL(signalEndOfRequest()), this, SLOT(slotEndOfRequestReceived()));

	m_thread = new QThread;

	moveToThread(m_thread);
	m_socket->moveToThread(m_thread);

	emit slotPrintLog("start connection with " + m_socket->peerAddress().toString() + ":" + QString::number(m_socket->peerPort()));
	emit slotPrintLog("start waiting for a request");

	m_thread->start();
}

SocketHandler::~SocketHandler()
{
	slotDisconnected();
	delete m_socket;
	delete m_thread;
}

void SocketHandler::slotReadyRead()
{
	m_message.append(m_socket->readAll());

	if (m_message.size() == 0)
	{
		return;
	}

	emit slotPrintLog("new data is available: " + QString::number(m_message.size()) + " bytes");
	emit slotPrintLog("parsing request");

	
	while (m_message.size() != 0)
	{
		if (requestParser.error() == true)
		{
			if (m_message == "\r\n")
			{
				m_message = "";
				slotEndOfRequestReceived();
			}
			else
			{
				m_message = m_message.mid(m_message.indexOf("\r\n") + 2); //???
			}
		}
		else
		{
			requestParser.ParseRequest(m_message);
		}
		m_message.append(m_socket->readAll());
		if (requestParser.parsed())
		{
			break;
		}
	}

	if (!requestParser.parsed())
		emit slotPrintLog("waiting for the rest part of request");
	else
		slotEndOfRequestReceived();
}

void SocketHandler::slotDisconnected()
{
	if (m_socketDescriptor != 0)
	{
		emit signalDisconnected();
		emit slotPrintLog("socket closed");
		m_socketDescriptor = 0;
		m_socket->flush();
		m_socket->close();
	}
	m_thread->quit();
}

#include "Handler.h"

void SocketHandler::slotEndOfRequestReceived()
{
	slotPrintLog("request parsed");
	if (requestParser.error())
	{
		Handler handler;
		handler.setResponseCode(400);
		sendResponse(handler.response());
		return;
	}

	Request request = requestParser.request();

	//slotPrintLog(request.rawData()); //VIVod request to log
	
	Handler handler;

	connect(&handler, SIGNAL(signalSaveReport(QByteArray, QString)), this, SIGNAL(signalSaveReport(QByteArray, QString)));
	connect(&handler, SIGNAL(signalPrintLog(QString)), this, SLOT(slotPrintLog(QString)));

	connect(this, SIGNAL(signalDisconnected()), &handler, SIGNAL(signalKillScript()), Qt::DirectConnection);

	handler.handle(request);
	emit signalDumpRequest(request.rawData() + handler.response(), m_socket->peerAddress().toString(), QString::number(m_socket->peerPort()));
	sendResponse(handler.response());
}

void SocketHandler::sendResponse(const QByteArray& message)
{
	if (!m_socketDescriptor)
		return;

	slotPrintLog("sending response");
	auto size = message.size();
	while(size)
	{
		slotPrintLog(QString::number(size) + " bytes left");
		size -= m_socket->write(message);
	}
	slotPrintLog("response sended");
	slotDisconnected();
}

void SocketHandler::slotPrintLog(const QString& log)
{
	if (m_socketDescriptor)
		emit signalPrintLog("[connection: " + QString::number(m_socketDescriptor) + "]: " + log);
}
