#ifndef HOSTPORTINPUTWINDOW_H
#define HOSTPORTINPUTWINDOW_H

#include <QDialog>
#include <QString>

#include <QLineEdit>
#include <QPair>

class HostPortInputWindow : public QDialog
{
Q_OBJECT
private:
	QLineEdit* ple_host;
	QLineEdit* ple_port;

public:
	HostPortInputWindow(QWidget* parent = 0);
	~HostPortInputWindow();

public:
	QPair<QString, QString> getHostPort() const;

public slots:
	void slotOkClicked();
};

#endif //HOSTPORTINPUTWINDOW_H
