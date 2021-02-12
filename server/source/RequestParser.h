#ifndef REQUESTPARSER_H
#define REQUESTPARSER_H

#include <QObject>
#include <QByteArray>

#include "Request.h"

class RequestParser : public QObject
{
Q_OBJECT

private:
	Request m_request;

	int m_stage = 0;
	bool m_parsed = false;
	bool m_error = false;

	bool m_recordingData = false;

public:
	RequestParser();
	~RequestParser();

public:
	void ParseRequest(QByteArray& message);

	bool error() const
	{
		return m_error;
	}

	bool parsed() const
	{
		return m_parsed;
	}

	Request request() const
	{
		return m_request;
	}

private:
	void ParseRequestLine(const QString& line);
	void ParseHeader(const QString& line);
	void ParseEntityBody(const QString& line);

	void nextStage();
};

#endif //REQUESTPARSER_H
