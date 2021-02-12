#include "Handler.h"

#include <QStringList>

#include <QDir>
#include <QFile>
#include <QTemporaryFile>
#include <QProcess>

#include <QDebug>

#include <QCoreApplication>

Handler::Handler(QObject* parent) : QObject(parent)
{


}

Handler::~Handler()
{

}

void Handler::setResponseCode(int code)
{
	m_response.setResponseCode(code);
}

void Handler::handle(const Request& request)
{
	emit signalPrintLog("handling request");

	if (request.version() != "HTTP/1.1")
	{
		m_response.setResponseCode(505);
	}

	else if (request.method() == "POST")
	{
		if (request.contentType().split(';')[0] == "multipart/form-data")
		{
			handleMultipart(request);
		}
		else
		{
			m_response.setResponseCode(501);
		}
	}
	else if (request.method() == "GET")
	{
		try
		{
			if (request.path().split('/')[1].split('?')[0] == "reports")
			{
				handleGetReport(request);
			}
			else
			{
				m_response.setResponseCode(406);
			}
		}
		catch (...)
		{
			if (request.path() == "/")
			{
				m_response.setResponseCode(406);
			}
			else
			{
				m_response.setResponseCode(400);
			}
		}
	}
	else
	{
		m_response.setResponseCode(501);
	}
	emit signalPrintLog("request handled");
}

QByteArray Handler::response()
{
	return m_response.serialize();
}

void Handler::handleMultipart(const Request& request)
{
	emit signalPrintLog("handle multipart");
	foreach(const auto& entityBody, request.data())
	{
		if (entityBody.contentType() == "application/txz")
		{
			try
			{
				auto contentDisposition = entityBody.contentDisposition();
				auto words = contentDisposition.split("; ");
				auto name = words[1].mid(words[1].indexOf('"') + 1);
				name = name.left(name.indexOf('"'));

				auto filename = words[2].mid(words[2].indexOf('"') + 1);
				filename = filename.left(filename.indexOf('"'));
				if (name == "to_check")
				{
					QString workType = filename.left(filename.indexOf("_"));
					if (!QDir(m_checkerPath).entryList().contains(workType, Qt::CaseInsensitive))
					{
						m_response.setResponseCode(404);
					}
					else if (filename.mid(filename.indexOf(".") + 1) == "txz")
					{
						handleTxz(entityBody.contentTransferEncoding(), filename, entityBody.data());
					}
					else
					{
						m_response.setResponseCode(501);
					}
				}
				else
				{
					m_response.setResponseCode(404);
				}
			}
			catch (...)
			{
				m_response.setResponseCode(400);
			}
		}
	}
	emit signalPrintLog("multipart handled");
}

void Handler::handleTxz(const QString& contentTransferEncoding, const QString& filename, QByteArray data)
{
	emit signalPrintLog("handling txz " + filename);

	if (contentTransferEncoding == "base64")
	{
		data = QByteArray::fromBase64(data);
	}
	else
	{
		qDebug() << "se" << contentTransferEncoding;
		m_response.setResponseCode(501);
		return;
	}

	QString workType = filename.left(filename.indexOf("_"));
	QTemporaryFile file(m_checkerPath + workType + "/works/" + filename);

	QString tmpFilename;
	if (file.open())
	{
		file.write(data);
		tmpFilename = file.fileName();
		file.close();
	}
	else
	{
		m_response.setResponseCode(500);
		return;
	}

	QProcess script;
	script.setWorkingDirectory(m_checkerPath + workType);
	connect(this, SIGNAL(signalKillScript()), &script, SLOT(kill()), Qt::DirectConnection);

	script.start("./do_tasks.sh", QStringList() << ("works/" + tmpFilename.section("/", -1)));
	if (script.waitForStarted())
	{
		if (script.waitForFinished())
		{
			if (script.error() != 5)
			{
				m_response.setResponseCode(500);
			}
			else
			{
				EntityBody entityBody;
				entityBody.addHeader("Content-Type", "text/plain");
				entityBody.setData(script.readAllStandardOutput());
				emit signalSaveReport(entityBody.data(), filename);
				m_response.addEntityBody(entityBody);
				m_response.setResponseCode(200);
				return;
			}
		}
		else
		{
			m_response.setResponseCode(500);
			return;
		}
	}
	else
	{
		m_response.setResponseCode(500);
		return;
	}
	
	emit signalPrintLog("txz handled");
}

#include <QUrl>
void Handler::handleGetReport(const Request& request)
{
	emit signalPrintLog("handle get report");
	QString reportsPath = qApp->applicationDirPath() + "/reports";

	QDir dir(reportsPath);
	if (!dir.exists())
	{
		dir.mkdir(reportsPath);
	}

	QUrl url(request.path());

	QString param;
	QString report;

	try
	{
		param = url.queryItems()[0].first;
		report = url.queryItems()[0].second;
	}
	catch (...)
	{
		m_response.setResponseCode(406);
		return;
	}

	report = report.left(report.lastIndexOf('.')) + ".txt";

	QString reportName = reportsPath + "/" + report;

	QFile file(reportName);

	if (file.exists() && file.open(QIODevice::ReadOnly))
	{
		QByteArray data = file.readAll();
		file.close();

		m_response.setVersion("HTTP/1.1");
		m_response.setResponseCode(200);

		m_response.addHeader("Content-Type", "text/plain");
		m_response.addHeader("Content-Lentgth", QString::number(data.size()));

		EntityBody entityBody;
		entityBody.setData(data);
		m_response.addEntityBody(entityBody);

		m_response.setResponseCode(200);
	}
	else if (!file.exists())
	{
		m_response.setResponseCode(404);
	}
	else
	{
		m_response.setResponseCode(500);
	}
	emit signalPrintLog("handled get report");
}
