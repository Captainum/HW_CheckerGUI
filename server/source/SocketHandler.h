#ifndef SOCKETHANDLER_H
#define SOCKETHANDLER_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QTcpSocket>
#include <QThread>

#include "RequestParser.h"

class SocketHandler : public QObject
{
Q_OBJECT

private:
	int m_socketDescriptor;
	QTcpSocket* m_socket;
	QThread* m_thread;

	RequestParser requestParser;
	QByteArray m_message = "";

public:
	SocketHandler(int socketDescriptor, QObject* parent = 0);
	~SocketHandler();

public:
	void sendResponse(const QByteArray& message);

public slots:
	void slotReadyRead();
	void slotDisconnected();
	void slotEndOfRequestReceived();
	void slotPrintLog(const QString& log);

signals:
	void signalDisconnected();
	void signalDumpRequest(const QByteArray& data, const QString& peerAddress, const QString& peerPort);
	void signalPrintLog(const QString& log);
	void signalSaveReport(const QByteArray& data, const QString& filename);
};

#endif //SOCKETHANDLER_H
