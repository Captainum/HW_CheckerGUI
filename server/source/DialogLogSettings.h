#ifndef DIALOGLOGSETTINGS_H
#define DIALOGLOGSETTINGS_H

#include <QDialog>
#include <QString>

#include <QLineEdit>
#include <QPair>

class DialogLogSettings : public QDialog
{
Q_OBJECT
private:
	QLineEdit* ple_logPath;
	QLineEdit* ple_requestsLogPath;

public:
	DialogLogSettings(QWidget* parent = 0);
	~DialogLogSettings();

public:
	QPair<QString, QString> getLogs() const;

public slots:
	void slotSetLogPath();
	void slotSetRequestsLogPath();
};

#endif //DIALOGLOGSETTINGS_H
