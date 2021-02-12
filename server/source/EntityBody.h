#ifndef ENTITYBODY_H
#define ENTITYBODY_H

#include <QHttpRequestHeader>
#include <QByteArray>
#include <QString>

class EntityBody
{
private:
	QHttpRequestHeader m_headers;
	QByteArray m_data = "";

public:
	EntityBody();
	~EntityBody();

public:

	void addHeader(const QString& key, const QString& value);
	void setData(const QByteArray& data);
	void addData(const QByteArray& data);
	void addData(const QString& data);

	QString contentType() const;
	QString contentDisposition() const;
	QString contentTransferEncoding() const;
	QString value(const QString& key) const;

	
	QByteArray data() const;

	QByteArray serialize() const;

};

#endif //ENTITYBODY_H
