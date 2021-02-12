#include "ServerApplication.h"

#include <QThread>

#include <QDateTime>

#include <QCoreApplication>

#include <QSettings>
#include <QMessageBox>

ServerApplication::ServerApplication(int argc, char** argv) : QApplication(argc, argv)
{
	m_server = new Server;
	m_serverGUI = new ServerGUI;
	m_serverThread = nullptr;

	QSettings settings(qApp->applicationDirPath() + "/config/server.ini", QSettings::IniFormat);
	m_logPath = settings.value("logPath", QVariant(qApp->applicationDirPath() + "/log/")).toString();
	m_requestsLogPath = settings.value("requestsLogPath", QVariant(qApp->applicationDirPath() + "/log_requests/")).toString();
	m_reportsPath = qApp->applicationDirPath() + "/reports";

	connect(m_serverGUI, SIGNAL(signalServerStart()), this, SLOT(slotServerStart()));
	connect(m_serverGUI, SIGNAL(signalServerStop()), this, SLOT(slotServerStop()));
	connect(m_serverGUI, SIGNAL(signalQuit()), this, SLOT(slotQuit()));

	connect(m_serverGUI, SIGNAL(signalUpdateSettings(QString, QString)), m_server, SLOT(slotUpdateSettings(QString, QString)));
	connect(m_serverGUI, SIGNAL(signalUpdateLogSettings(QString, QString)), this, SLOT(slotUpdateLogSettings(QString,QString)));
	connect(m_serverGUI, SIGNAL(signalDumpLog(QString)), this, SLOT(slotDumpLog(QString)));

	connect(m_server, SIGNAL(signalSaveReport(QByteArray, QString)), this, SLOT(slotSaveReport(QByteArray, QString)));
	connect(m_server, SIGNAL(signalPrintLog(QString)), this, SLOT(slotPrintLog(QString)));
	connect(m_server, SIGNAL(signalDumpRequest(QByteArray, QString, QString)), this, SLOT(slotDumpRequest(QByteArray, QString, QString)));
	
	connect(m_server, SIGNAL(signalNewConnection()), m_serverGUI, SLOT(slotNewConnection()));
	connect(m_server, SIGNAL(signalDisconnected()), m_serverGUI, SLOT(slotDisconnected()));
	connect(m_server, SIGNAL(signalServerStarted()), m_serverGUI, SLOT(slotServerStarted()));

	connect(this, SIGNAL(signalPrintLog(QString)), m_serverGUI, SLOT(slotPrintLog(QString)));

	m_serverGUI->show();
}

ServerApplication::~ServerApplication()
{
	qDebug() << "~ServerApplication()";
	if (m_serverThread != nullptr)
	{
		delete m_serverThread;
	}
	delete m_server;
	delete m_serverGUI;
}

void ServerApplication::slotServerStart()
{
	if (m_logPath == "" || m_requestsLogPath == "")
	{
		QMessageBox::warning(m_serverGUI, tr("ERROR!"),
												 tr("Setup log path first!"),
												 QMessageBox::Ok);
		return;
	}

	slotPrintLog("starting server at " + m_server->host().toString() + ":" + QString::number(m_server->port()));
	if (m_serverThread != nullptr)
	{
		return;
	}
	m_serverThread = new QThread;
	m_server->moveToThread(m_serverThread);

	connect(this, SIGNAL(aboutToQuit()), m_server, SLOT(slotServerStop()));
	connect(m_serverThread, SIGNAL(started()), m_server, SLOT(slotServerStart()));
	connect(m_serverThread, SIGNAL(finished()), m_server, SLOT(slotServerStop()));
	
	connect(this, SIGNAL(aboutToQuit()), m_serverThread, SLOT(quit()));
	connect(m_serverThread, SIGNAL(finished()), m_serverThread, SLOT(deleteLater()));
	connect(m_server, SIGNAL(signalServerStopped()), this, SLOT(slotServerStop()));
	
	m_serverThread->start();
}

void ServerApplication::slotServerStop()
{
	if (m_serverThread != nullptr)
	{
		m_serverThread->quit();
		m_serverThread = nullptr;
	}
}

void ServerApplication::slotSetSettings(const QString& host, const quint16 port)
{
//UPDATE SETTINGS SERVER SLOT?
}

void ServerApplication::slotUpdateLogSettings(const QString& logPath, const QString& requestsLogPath)
{
	m_logPath = logPath;
	m_requestsLogPath = requestsLogPath;

	QSettings settings(qApp->applicationDirPath() + "/config/server.ini", QSettings::IniFormat);
	settings.setValue("logPath", QVariant(m_logPath));
	settings.setValue("requestsLogPath", QVariant(m_requestsLogPath));
}

void ServerApplication::slotQuit()
{
	emit quit();
}

void ServerApplication::slotPrintLog(QString log)
{
	log.prepend("[" + QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss.zzz") + "] ");
	emit signalPrintLog(log);
}

#include <QFile>
#include <QDir>

void ServerApplication::slotDumpRequest(const QByteArray& rawData, const QString& peerAddress, const QString& peerPort)
{
	QDir dir(m_requestsLogPath);
	if (!dir.exists())
	{
		dir.mkdir(m_requestsLogPath);
	}

	QString log = m_requestsLogPath + "/log_" + peerAddress + "_" + QDateTime::currentDateTime().toString("dd-MM-yyyy_hh:mm:ss.zzz") + ".txt";

	QFile requestsLog(log);
	if (requestsLog.open(QIODevice::WriteOnly))
	{
		requestsLog.write(rawData);
		requestsLog.close();
	}
	else
		slotPrintLog("ERROR WHILE DUMPING REQUEST");
}

void ServerApplication::slotSaveReport(const QByteArray& data, const QString& filename)
{
	QDir dir(m_reportsPath);
	if (!dir.exists())
	{
		dir.mkdir(m_reportsPath);
	}

	QString reportName = m_reportsPath + "/" + filename.split('.')[0] + ".txt";
	QFile file(reportName);
	if (file.open(QIODevice::WriteOnly))
	{
		file.write(data);
		file.close();
	}
	else
		slotPrintLog("ERROR WHILE SAVING REPORT");
}

#include <QTextStream>
void ServerApplication::slotDumpLog(const QString& data)
{
	QDir dir(m_logPath);
	if (!dir.exists())
	{
		dir.mkdir(m_logPath);
	}

	QString logName = m_logPath + "/logDump_" + QDateTime::currentDateTime().toString("dd-MM-yyyy_hh:mm:ss.zzz") + ".txt";
	QFile file(logName);
	if (file.open(QIODevice::WriteOnly))
	{
		QTextStream out(&file);
		out << data;
		file.close();
	}
	else
		slotPrintLog("ERROR WHILE DUMPING LOG");
}
