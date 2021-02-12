#include "EntityBody.h"

EntityBody::EntityBody()
{
	
}

EntityBody::~EntityBody()
{

}
void EntityBody::addHeader(const QString& key, const QString& value)
{
	m_headers.setValue(key, value);
}

void EntityBody::setData(const QByteArray& data)
{
	m_data = data;
}

void EntityBody::addData(const QByteArray& data)
{
	m_data.append(data);
}

void EntityBody::addData(const QString& data)
{
	m_data.append(data);
}

QString EntityBody::contentType() const
{
	return value("Content-Type");
}

QString EntityBody::contentDisposition() const
{
	return value("Content-Disposition");
}

QString EntityBody::contentTransferEncoding() const
{
	return value("Content-Transfer-Encoding");
}

QString EntityBody::value(const QString& key) const
{
	return m_headers.value(key);
}

QByteArray EntityBody::data() const
{
	return m_data;
}

QByteArray EntityBody::serialize() const
{
	QByteArray message = "";
	foreach(const auto& header, m_headers.values())
	{
		message.append(header.first + ": " + header.second + "\r\n");
	}
	message.append("\r\n");
	message.append(m_data);
	
	return message;
}
