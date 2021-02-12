#ifndef HANDLER_H
#define HANDLER_H

#include <QString>
#include <QByteArray>

#include "Response.h"
#include "Request.h"

#include <QCoreApplication>

class Handler : public QObject
{
Q_OBJECT

private:
	Response m_response;
	QString m_checkerPath = qApp->applicationDirPath() + "/checker/";

public:
	Handler(QObject* parent = 0);
	~Handler();

public:
	void setResponseCode(int code);

	void handle(const Request& request);
	QByteArray response();
	void handleMultipart(const Request& request);
	void handleTxz(const QString& contentTransferEncoding, const QString& filename, QByteArray data);
	void handleGetReport(const Request& request);

signals:
	void signalPrintLog(const QString& log);
	void signalSaveReport(const QByteArray& data, const QString& filename);

	void signalKillScript();
};

#endif //HANDLER_H
