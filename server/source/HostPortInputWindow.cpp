#include "HostPortInputWindow.h"

#include <QSettings>
#include <QRegExpValidator>
#include <QRegExp>
#include <QIntValidator>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QCoreApplication>

HostPortInputWindow::HostPortInputWindow(bool Listening, QWidget* parent) :
																			 	 QDialog(parent), m_listening(Listening)
{
	setWindowTitle("Server settings");
	QSettings settings(qApp->applicationDirPath() + "/config/server.ini", QSettings::IniFormat);
  
	QRegExpValidator* host_validator = new QRegExpValidator(QRegExp("[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}"), this);
	QIntValidator* port_validator = new QIntValidator(this);
	port_validator->setRange(0, 65535);
	
	QHBoxLayout* phb_host = new QHBoxLayout;
	QLabel* plbl_host = new QLabel("Host:");
	ple_host = new QLineEdit(settings.value("host", QVariant("127.0.0.1")).toString());
	ple_host->setValidator(host_validator);
	phb_host->addWidget(plbl_host);
	phb_host->addWidget(ple_host);

	QHBoxLayout* phb_port = new QHBoxLayout;
	QLabel* plbl_port = new QLabel("Port:");
	ple_port = new QLineEdit(settings.value("port", QVariant("33333")).toString());
	ple_port->setValidator(port_validator);
	phb_port->addWidget(plbl_port);
	phb_port->addWidget(ple_port);

	QHBoxLayout* phb_buttons = new QHBoxLayout;
	QPushButton* ppb_ok = new QPushButton("Ok");
	QPushButton* ppb_cancel = new QPushButton("Cancel");
	connect(ppb_ok, SIGNAL(clicked()), this, SLOT(slotOkClicked()));
	connect(ppb_cancel, SIGNAL(clicked()), this, SLOT(reject()));
	phb_buttons->addWidget(ppb_ok);
	phb_buttons->addWidget(ppb_cancel);

	QVBoxLayout* pvb_layout = new QVBoxLayout;
	pvb_layout->addLayout(phb_host);
	pvb_layout->addLayout(phb_port);
	pvb_layout->addLayout(phb_buttons);
	
	setLayout(pvb_layout);
}

HostPortInputWindow::~HostPortInputWindow()
{
}

QPair<QString, QString> HostPortInputWindow::getHostPort() const
{
	return {ple_host->text(), ple_port->text()};	
}
void HostPortInputWindow::slotOkClicked()
{
	if (ple_host->text() == "" || ple_port->text() == "")
	{
		QMessageBox msgBox(this);
		msgBox.setText("Values cannot be empty!");
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.setInformativeText("You must enter host and port!");
		msgBox.setStandardButtons(QMessageBox::Ok);
		msgBox.exec();
		return;
	}
	else
	{
		if (m_listening)
		{
			QMessageBox msgBox(this);
			msgBox.setText("SERVER is ONLINE");
			msgBox.setIcon(QMessageBox::Warning);
			msgBox.setInformativeText("It needs to be restart. Are you sure?");
			msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
			if (msgBox.exec() == 1024)
			{
				accept();
			}
			else
			{
				reject();
			}
		}
		else
		{
			accept();
		}
	}
}

