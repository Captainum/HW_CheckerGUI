#include "Request.h"

Request::Request()
{

}

Request::~Request()
{

}
void Request::setVersion(const QString& version)
{
	m_version = version;
}

void Request::setPath(const QString& path)
{
	m_path = path;
}

void Request::setMethod(const QString& method)
{
	m_method = method;
}

void Request::addHeader(const QString& key, const QString& value)
{
	m_headers.setValue(key, value);
}

void Request::addData(const EntityBody& entityBody)
{
	m_data.append(entityBody);
}

QString Request::version() const
{
	return m_version;
}

QString Request::path() const
{
	return m_path;
}

QString Request::method() const
{
	return m_method;
}

QString Request::boundary() const
{
	try
	{
		QString bound = value("Content-Type").split(' ')[1];
		bound = bound.mid(bound.indexOf("\"") + 1);
		bound = bound.left(bound.indexOf("\""));
		return bound;
	}
	catch (...)
	{
		return "";
	}
}

QString Request::contentType() const
{
	return value("Content-Type");
}
	
QString Request::value(const QString& key) const
{
	return m_headers.value(key);
}

QList<EntityBody>& Request::data()
{
	return m_data;
}

const QList<EntityBody>& Request::data() const
{
  return m_data;
}
