#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QHostAddress>
#include <QByteArray>
#include <QString>

class Server : public QTcpServer
{
Q_OBJECT

private:
	QHostAddress m_host;
	quint16 m_port;

public:
	Server(QObject* parent = 0);
	~Server();

public:
	QHostAddress host() const;
	quint16 port() const;


public slots:
	void slotUpdateSettings(const QString& host, const QString& port);
	void slotServerStart();
	void slotServerStop();

protected:
	virtual void incomingConnection(int socketDescriptor) override;

signals:
	void signalSaveReport(const QByteArray& data, const QString& filename);
	void signalPrintLog(const QString& log);
	void signalNewConnection();
	void signalDumpRequest(const QByteArray& data, const QString& peerAddress, const QString& peerPort);
	void signalDisconnectSocket();
	void signalDisconnected();
	void signalServerStarted();
	void signalServerStopped();
};

#endif //SERVER_H
