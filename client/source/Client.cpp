#include "Client.h"

#include <QFile>
#include <QTcpSocket>
#include <QSettings>
#include <QDebug>

#include <QHttpMultiPart>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#include <QCoreApplication>

Client::Client(QObject* parent) : QObject(parent)
{
	QSettings settings(qApp->applicationDirPath() + "/config/client.ini", QSettings::IniFormat);
	m_host = settings.value("host", QVariant("127.0.0.1")).toString();
	m_port = settings.value("port", QVariant("33333")).toUInt();
}

void Client::slotPostAnswer(const QString& filename, const QString& workType)
{
	emit signalPrintLog("Start forming request to " + m_host + ":" + QString::number(m_port));

	QFile file(filename);

	QHttpMultiPart* multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType, this);
	QHttpPart zipPart;
	zipPart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/txz"));
	zipPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"to_check\"; filename=\"" + workType + "_" + file.fileName().section("/", -1) + "\""));
	zipPart.setRawHeader("Content-Encoding", "gzip");
	zipPart.setRawHeader("Content-Transfer-Encoding", "base64");

	file.open(QIODevice::ReadOnly);

	QByteArray q;
	q = file.readAll();
	file.close();
	q = q.toBase64();

	zipPart.setBody(q);
	multiPart->append(zipPart);

	QUrl url;
	url.setScheme("http");
	url.setHost(m_host);
	url.setPort(m_port);
	url.setPath("/checker");

	QNetworkRequest request(url);

	QNetworkAccessManager* manager = new QNetworkAccessManager;

	emit signalPrintLog("Start sending request");

	reply = manager->post(request, multiPart);
	
	connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(slotReadResponse(QNetworkReply*)));
	connect(manager, SIGNAL(finished(QNetworkReply*)), manager, SLOT(deleteLater()));
	connect(this, SIGNAL(signalStopConnection()), this, SLOT(slotStopConnection()));
	connect(reply, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));

	emit signalPrintLog("Request sended");
	emit signalPrintLog("Waiting for response...");
}

void Client::slotGetReport(const QString& filename, const QString& workType)
{
	emit signalPrintLog("Start forming request to " + m_host + ":" + QString::number(m_port));

	QUrl url;
	url.setScheme("http");
	url.setHost(m_host);
	url.setPort(m_port);
	url.setPath("/reports");
	url.addQueryItem("filename", workType + "_" + filename.section("/", -1));

	QNetworkRequest request(url);
	QNetworkAccessManager* manager = new QNetworkAccessManager;

	emit signalPrintLog("Start sending request");
	
	reply = manager->get(request);

	connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(slotReadResponse(QNetworkReply*)));
	connect(manager, SIGNAL(finished(QNetworkReply*)), manager, SLOT(deleteLater()));
	connect(this, SIGNAL(signalStopConnection()), this, SLOT(slotStopConnection()));
	connect(reply, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));

	emit signalPrintLog("Request sended");
	emit signalPrintLog("Waiting for response...");
}

void Client::slotReadyRead()
{
	emit signalPrintLog("Getting response");
}

void Client::slotReadResponse(QNetworkReply* r)
{
	switch (r->error())
	{
		emit signalPrintLog("Response received");
		case QNetworkReply::NoError:
			emit signalPrintReport(r->readAll());
			emit signalResponseSuccess();
			return;
		case QNetworkReply::OperationCanceledError:
			emit signalResponseError("Abort");
			return;
		default:
			emit signalResponseError(r->errorString());
			return;
	}
}

void Client::slotUpdateSettings()
{
	QSettings settings(qApp->applicationDirPath() + "/config/client.ini", QSettings::IniFormat);

	m_host = settings.value("host", QVariant("127.0.0.1")).toString();
	m_port = settings.value("port", QVariant("33333")).toUInt();
}

void Client::slotStopConnection()
{
	reply->close();
}
