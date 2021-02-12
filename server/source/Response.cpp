#include "Response.h"

#include <QDateTime>

Response::Response()
{

}

Response::~Response()
{

}

void Response::setVersion(const QString& version)
{
	
}

void Response::setResponseCode(int code)
{
	m_statusCode = code;
}

QByteArray Response::serialize()
{
	addHeader("Date", QDateTime::currentDateTime().toString("ddd, dd MMM yyyy hh:mm:ss") + " GTM+3");
	addHeader("Server", "VM");
	addHeader("Connection", "close");
	QByteArray message = "";
	message.append("HTTP/1.1 " + QString::number(m_statusCode) + " " + statusCodeToString() + "\r\n");
	
	if (m_data.size() == 0)
	{

	}
	else if (m_data.size() == 1)
	{
		addHeader("Content-Type", m_data[0].contentType());
	}
	else
	{
		addHeader("Content-Type", "multipart/form-data");
	}

	foreach(const auto& pair, m_headers.values())
	{
		message.append(pair.first + ": " + pair.second);
		message.append("\r\n");
	}
	message.append("\r\n");

	if (m_data.size() == 1)
	{
		message.append(m_data[0].data());
	}

	else if (m_data.size() > 1)
	{
	
		foreach(const auto& entityBody, m_data)
		{
			entityBody.serialize();
		}
	}
	return message;
}

void Response::addHeader(const QString& key, const QString& value)
{
	m_headers.setValue(key, value);
}

void Response::addEntityBody(const EntityBody& entityBody)
{
	m_data.append(entityBody);
}

QString Response::statusCodeToString() const
{
	switch(m_statusCode)
	{
		case 200:
			return "OK";
		case 400:
			return "Bad Request";
		case 404:
			return "Not Found";
		case 406:
			return "Not Acceptable";
		case 500:
			return "Internal Server Error";
		case 501:
			return "Not Implemented";
		case 505:
			return "HTTP Version Not Supported";
	}
}
