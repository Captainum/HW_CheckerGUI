#ifndef CLIENTGUI_H
#define CLIENTGUI_H

#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QStatusBar>
#include <QProgressBar>

#include "Client.h"

class ClientGUI : public QMainWindow
{
Q_OBJECT

public:
	ClientGUI(QWidget* parent = 0);

private:
	//CONNECTIO SETTINGS
	QString m_host;
	QString m_port;

	//PATH TO SOLUTION
	QLineEdit* ple_filePath;

	//WORK TYPE
	QLineEdit* ple_workType;

	//REPORT AREA
	QTextEdit* pte_reportArea;

	//REPORT FILENAME
	QString m_reportFileName;

	//CONNECTION SETTINGS TEXTLINE
	QLineEdit* ple_host;
	QLineEdit* ple_port;

	//STATUSBAR
	QStatusBar* p_statusBar;

signals:
	void signalPrintLog(const QString& log);
	void signalResponseSuccess();
	void signalResponseError(const QString& err);

	void signalUpdateSettings();

	void signalPostAnswer(const QString& filename, const QString& workType);
	void signalGetReport(const QString& filename, const QString& workType);

	void signalStopConnection();

public slots:
	void slotSetFilePath();
	void slotSaveReport();
	void slotSaveReportAs();
	void slotRefresh();

	void slotSetConnectionSettings();

	void slotPostAnswer();
	void slotGetReport();

	void slotPrintReport(const QString& report);
};

#endif //CLIENTGUI_H
