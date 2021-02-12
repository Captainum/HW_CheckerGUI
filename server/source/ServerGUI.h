#ifndef SERVERGUI_H
#define SERVERGUI_H

#include <QMainWindow>
#include <QString>
#include <QLineEdit>
#include <QTextEdit>
#include <QStatusBar>
#include <QPushButton>
#include <QLabel>

class ServerGUI : public QMainWindow
{
Q_OBJECT

private:
	QLineEdit* ptl_Host;
	QLineEdit* ptl_Port;
	QTextEdit* pte_Log;
	QStatusBar* p_statusBar;

	QPushButton* ppb_stop;
	QPushButton* ppb_start;

	QLabel* plbl_Status;
	QLabel* plbl_countOfConnections;
	
public:
	ServerGUI(QWidget* parent = 0);
	~ServerGUI();

public:

public slots:
	void slotServerStarted();
	void slotServerStart();
	void slotServerStop();
	void slotQuit();
	void slotNewConnection();
	void slotDisconnected();
	void slotSetSettings();
	void slotSetLogSettings();
	void slotPrintLog(const QString& log);
	void slotDumpLog();

signals:
	void signalServerStart();
	void signalServerStop();
	void signalQuit();
	void signalUpdateSettings(const QString& peerAddress, const QString& peerPort);
	void signalUpdateLogSettings(const QString& logPath, const QString& requestsLogPath);
	void signalDumpLog(const QString& log);
};

#endif //SERVERGUI_H
