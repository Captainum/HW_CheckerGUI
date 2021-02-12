#ifndef RESPONSE_H
#define RESPONSE_H

#include <QHttpResponseHeader>
#include <QList>
#include <QString>

#include "EntityBody.h"

class Response
{
private:
	int m_statusCode;
	QHttpResponseHeader m_headers;
	QList<EntityBody> m_data;

public:
	Response();
	~Response();

public:
	void setVersion(const QString& version);
	void setResponseCode(int statusCode);
	//void setVersion(Version version);
	void addHeader(const QString& key, const QString& value);
	void addEntityBody(const EntityBody& entityBody);
	
	QString value(const QString key);
	
	QByteArray serialize();

private:
	QString statusCodeToString() const;
};

#endif //RESPONSE_H
