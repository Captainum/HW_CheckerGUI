#ifndef SERVERAPPLICATION_H
#define SERVERAPPLICATION_H

#include <QApplication>
#include <QString>
#include <QByteArray>
#include <QThread>

#include "Server.h"
#include "ServerGUI.h"



class ServerApplication : public QApplication
{
Q_OBJECT

private:
	Server* m_server;
	ServerGUI* m_serverGUI;
	QThread* m_serverThread;

	QString m_logPath;
	QString m_requestsLogPath;

	QString m_reportsPath;

public:
	ServerApplication(int argc, char** argv);
	~ServerApplication();

public slots:
	void slotServerStart();
	void slotServerStop();
	void slotSetSettings(const QString& host, const quint16 port);
	void slotUpdateLogSettings(const QString& logPath, const QString& requestsLogPath);
	void slotQuit();
	void slotPrintLog(QString log);
	void slotDumpRequest(const QByteArray& rawData, const QString& peerAddress, const QString& peerPort);
	void slotSaveReport(const QByteArray& data, const QString& filename);
	void slotDumpLog(const QString& data);

signals:
	void signalPrintLog(const QString& log);
};

#endif //SERVERAPPLICATION_H
