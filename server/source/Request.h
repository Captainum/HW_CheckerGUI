#ifndef REQUEST_H
#define REQUEST_H

#include <QString>
#include <QHttpRequestHeader>
#include <QList>
#include "EntityBody.h"

class Request
{
private:
	QString m_version;
	QString m_path;
	QString m_method;

	QHttpRequestHeader m_headers;

	QList<EntityBody> m_data;

	QByteArray m_rawData = "";

public:
	Request();
	~Request();

public:
	void setVersion(const QString& version);
	void setPath(const QString& path);
	void setMethod(const QString& method);

	void addHeader(const QString& key, const QString& value);
	void addData(const EntityBody& entityBody);


	QString version() const;
	QString path() const;
	QString method() const;
	QString boundary() const;

	QString contentType() const;

	QString value(const QString& key) const;

	void addRawData(const QByteArray& data)
	{
		m_rawData.append(data + "\r\n");
	}

	QByteArray rawData() const
	{
		return m_rawData;
	}

	QList<EntityBody>& data();
	const QList<EntityBody>& data() const;

};

#endif //REQUEST_H
