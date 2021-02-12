#include "DialogLogSettings.h"

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
#include <QStyle>

#include <QFileDialog>

DialogLogSettings::DialogLogSettings(QWidget* parent) :
																			 	 QDialog(parent)
{
	setWindowTitle("Server log settings");
	QSettings settings(qApp->applicationDirPath() + "/config/server.ini", QSettings::IniFormat);
  
	QHBoxLayout* phb_logPath = new QHBoxLayout;
	QLabel* plbl_logPath = new QLabel("For Logs:");
	ple_logPath = new QLineEdit(settings.value("logPath", QVariant(qApp->applicationDirPath() + "/log")).toString());
	QPushButton* ppb_logPath = new QPushButton(style()->standardIcon(QStyle::SP_DialogOpenButton), QString());
	connect(ppb_logPath, SIGNAL(clicked()), this, SLOT(slotSetLogPath()));
	phb_logPath->addWidget(plbl_logPath);
	phb_logPath->addWidget(ple_logPath);
	phb_logPath->addWidget(ppb_logPath);

	QHBoxLayout* phb_requestsLogPath = new QHBoxLayout;
	QLabel* plbl_requestsLogPath = new QLabel("For Requests Logs:");
	ple_requestsLogPath = new QLineEdit(settings.value("requestsLogPath", QVariant(qApp->applicationDirPath() + "/log_requests")).toString());
	QPushButton* ppb_requestsLogPath = new QPushButton(style()->standardIcon(QStyle::SP_DialogOpenButton), QString());
	connect(ppb_requestsLogPath, SIGNAL(clicked()), this, SLOT(slotSetRequestsLogPath()));
	phb_requestsLogPath->addWidget(plbl_requestsLogPath);
	phb_requestsLogPath->addWidget(ple_requestsLogPath);
	phb_requestsLogPath->addWidget(ppb_requestsLogPath);

	QHBoxLayout* phb_buttons = new QHBoxLayout;
	QPushButton* ppb_ok = new QPushButton("Ok");
	QPushButton* ppb_cancel = new QPushButton("Cancel");
	connect(ppb_ok, SIGNAL(clicked()), this, SLOT(accept()));
	connect(ppb_cancel, SIGNAL(clicked()), this, SLOT(reject()));
	phb_buttons->addWidget(ppb_ok);
	phb_buttons->addWidget(ppb_cancel);

	QVBoxLayout* pvb_layout = new QVBoxLayout;
	pvb_layout->addLayout(phb_logPath);
	pvb_layout->addLayout(phb_requestsLogPath);
	pvb_layout->addLayout(phb_buttons);
	
	setLayout(pvb_layout);
}

DialogLogSettings::~DialogLogSettings()
{
}

QPair<QString, QString> DialogLogSettings::getLogs() const
{
	return QPair<QString, QString>(ple_logPath->text(), ple_requestsLogPath->text());	
}

void DialogLogSettings::slotSetLogPath()
{
	QString str = QFileDialog::getExistingDirectory(0, tr("Open dir"),
																						 qApp->applicationDirPath()
																						 );
	if (!str.isEmpty())
	{
		ple_logPath->setText(str);
	}
}

void DialogLogSettings::slotSetRequestsLogPath()
{
	QString str = QFileDialog::getExistingDirectory(0, tr("Open dir"),
																						 qApp->applicationDirPath()
																						 );
	if (!str.isEmpty())
	{
		ple_logPath->setText(str);
	}
}
