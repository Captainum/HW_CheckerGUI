#include "RequestParser.h"

#include <QDebug>

RequestParser::RequestParser()
{

}

RequestParser::~RequestParser()
{

}

void RequestParser::ParseRequest(QByteArray& message)
{
	int delim = message.indexOf("\r\n");
	if (delim == -1)
	{
	    return;
	}

	QByteArray line = message.left(delim);
	m_request.addRawData(line);
	if (m_stage == 0)
	{
		ParseRequestLine(line);
	}
	else if (m_stage == 1)
	{
		ParseHeader(line);
	}
	else if (m_stage == 2)
	{
		ParseEntityBody(line);
	}
	message = message.mid(delim + 2);
}

void RequestParser::ParseRequestLine(const QString& line)
{
	QStringList words = line.split(' ');
	
	if (words.size() != 3)
	{
		m_error = true;
	}
	else
	{
		m_request.setMethod(words[0]);
		m_request.setPath(words[1]);
		m_request.setVersion(words[2]);
		
		m_stage++;
	}
}

void RequestParser::ParseHeader(const QString& line)
{
	if (line == "")
	{
		m_stage++;
		if (m_request.method() != "POST")
		{
			m_parsed = true;
		}
		return;
	}

	QStringList words = line.split(": ");
	if (words.size() != 2)
	{
		m_error = true;
	}
	else
	{
		m_request.addHeader(words[0], words[1]);
	}
}

void RequestParser::ParseEntityBody(const QString& line)
{
	if (m_request.data().size() == 0)
	{
		m_request.data().append(EntityBody());
	}
	EntityBody& entityBody = m_request.data().back();

	if (line == "")
	{
		m_recordingData = true;
		return;
	}
	else if (line == ("--" + m_request.boundary()))
	{
		m_recordingData = false;
		if (entityBody.data().size() != 0)
		{
			m_request.data().append(EntityBody());
		}
	}
	else if (line == ("--" + m_request.boundary() + "--"))
	{
		m_recordingData = false;
		
		m_parsed = true;
	}
	else if (m_recordingData)
	{
			entityBody.addData(line);
	}
	else
	{
		QStringList words = line.split(": ");
		if (words.size() != 2)
		{
			m_error = true;
		}
		else
		{
			entityBody.addHeader(words[0], words[1]);
		}
	}
}

void RequestParser::nextStage()
{
	m_stage++;
}

