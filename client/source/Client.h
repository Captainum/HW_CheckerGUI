#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>

class QFile;
class QNetworkReply;

class Client : public QObject
{
Q_OBJECT
private:
	QString m_host;
	quint16 m_port;

	QNetworkReply* reply;

public:
	explicit Client(QObject* parent = 0);
	~Client() = default;

public slots:
	void slotReadyRead();

	void slotPostAnswer(const QString& filename, const QString& workType);
	void slotGetReport (const QString& filename, const QString& workType);

	void slotReadResponse(QNetworkReply*);

	void slotUpdateSettings();

	void slotStopConnection();

signals:
	void signalPrintLog(const QString& log);
	void signalResponseSuccess();
	void signalResponseError(const QString& err);

	void signalPrintReport(const QString& report);
	void finished(const QString& filename);

	void signalStopConnection();
};

#endif //CLIENT_H
