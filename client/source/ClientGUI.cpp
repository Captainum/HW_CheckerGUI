#include "ClientGUI.h"

#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QString>
#include <QFileDialog>
#include <QLabel>
#include <QFile>
#include <QDebug>
#include <QMessageBox>
#include <QDateTime>
#include <QSettings>

#include <QCoreApplication>

#include "ClientLogWindow.h"
#include "HostPortInputWindow.h"

ClientGUI::ClientGUI(QWidget* parent) : QMainWindow(parent)
{
	setWindowTitle("[NO NAME] - Solution checker");
	resize(1000, 500);

	QSettings settings(qApp->applicationDirPath() + "/config/client.ini", QSettings::IniFormat);
	m_host = settings.value("host", QVariant("127.0.0.1")).toString();
	m_port = settings.value("port", QVariant("33333")).toString();

	//ANSWER TOOL BAR
	QToolBar* ptb_toolBar = new QToolBar;
	ptb_toolBar->setMovable(false);
	ptb_toolBar->setFloatable(false);
	setContextMenuPolicy(Qt::PreventContextMenu);
	
	QLabel* plbl_Label = new QLabel("Your archieve:");

	ple_filePath = new QLineEdit;
	ple_filePath->setReadOnly(true);

	QLabel* plbl_workType = new QLabel("Name of work:");
	ple_workType = new QLineEdit;

	ptb_toolBar->addWidget(plbl_Label);
	ptb_toolBar->addWidget(ple_filePath);
	ptb_toolBar->addSeparator();
	ptb_toolBar->addWidget(plbl_workType);
	ptb_toolBar->addWidget(ple_workType);

	addToolBar(ptb_toolBar);

	//
	
	addToolBarBreak();

	//ONLINE TOOL BAR
	ptb_toolBar = new QToolBar;
	ptb_toolBar->setMovable(false);
	ptb_toolBar->setFloatable(false);
	setContextMenuPolicy(Qt::PreventContextMenu);
	
	QLabel* plbl_host = new QLabel("Server IP:");
	ple_host = new QLineEdit(m_host);
	ple_host->setReadOnly(true);

	QLabel* plbl_port = new QLabel("Server Port:");
	ple_port = new QLineEdit(m_port);
	ple_port->setReadOnly(true);

	QPushButton* ppb_send = new QPushButton("&SEND");
	connect(ppb_send, SIGNAL(clicked()), this, SLOT(slotPostAnswer()));

	QPushButton* ppb_get = new QPushButton("&GET REPORT");
	connect(ppb_get, SIGNAL(clicked()), this, SLOT(slotGetReport()));
	
	ptb_toolBar->addWidget(plbl_host);
	ptb_toolBar->addWidget(ple_host);
	ptb_toolBar->addSeparator();
	ptb_toolBar->addWidget(plbl_port);
	ptb_toolBar->addWidget(ple_port);
	ptb_toolBar->addSeparator();
	ptb_toolBar->addWidget(ppb_send);
	ptb_toolBar->addSeparator();
	ptb_toolBar->addWidget(ppb_get);

	addToolBar(ptb_toolBar);
	//
	
	//MENU
	QMenu* pmnu_Menu = new QMenu("&Menu");

	pmnu_Menu->addAction("&Choose File",
											 this,
											 SLOT(slotSetFilePath()),
											 QKeySequence("CTRL+O"));
	pmnu_Menu->addAction("&Save Report",
											 this,
											 SLOT(slotSaveReport()),
											 QKeySequence("CTRL+S"));
	pmnu_Menu->addAction("&Save Report as...",
											 this,
											 SLOT(slotSaveReportAs()),
											 QKeySequence("CTRL+SHIFT+S"));
	pmnu_Menu->addAction("&Refresh",
											 this,
											 SLOT(slotRefresh()),
											 QKeySequence("CTRL+R"));
	menuBar()->addMenu(pmnu_Menu);

	QMenu* pmnu_Settings = new QMenu("&Settings");

	pmnu_Settings->addAction("&Set connection settings",
											 this,
											 SLOT(slotSetConnectionSettings()),
											 QKeySequence("CTRL+P"));
	menuBar()->addMenu(pmnu_Settings);

	//
	
	//REPORT AREA
	pte_reportArea = new QTextEdit("<CENTER>YOUR REPORT WILL BE PRINTED HERE</CENTER>");
	pte_reportArea->setReadOnly(true);
	pte_reportArea->setEnabled(false);
	setCentralWidget(pte_reportArea);
	//
	
	//STATUSBAR
	p_statusBar = new QStatusBar;
	setStatusBar(p_statusBar);
	//
}

void ClientGUI::slotSetFilePath()
{
	QString str = QFileDialog::getOpenFileName(0, tr("Open File"),
																						 qApp->applicationDirPath(),
																						 tr(".txz (*.txz)")
																						 );
	if (!str.isEmpty())
	{
		ple_filePath->setText(str);
	}
}

void ClientGUI::slotSaveReport()
{
	if (m_reportFileName.isEmpty())
	{
		slotSaveReportAs();
		return;
	}

	QFile file(m_reportFileName);
	if (file.open(QIODevice::WriteOnly))
	{
		QTextStream(&file) << pte_reportArea->toPlainText();
		file.close();
		setWindowTitle(m_reportFileName + " - Solution Checker");
	}
	else
	{
		QMessageBox msgBox(this);
		msgBox.setText("An Error Occupied!");
		msgBox.setIcon(QMessageBox::Critical);
		msgBox.setStandardButtons(QMessageBox::Ok);
		msgBox.setText("CRITICAL ERROR WITH OPENNING FILE OCCUPIED");
		msgBox.exec();
	}
}

void ClientGUI::slotSaveReportAs()
{
	QString str = QFileDialog::getSaveFileName(0, tr("Save File"),
																						 qApp->applicationDirPath() + "/report.txt",
																						 tr("Text files (*.txt)")
																						 );
	if (!str.isEmpty())
	{
		m_reportFileName = str;
		slotSaveReport();
	}
}

void ClientGUI::slotRefresh()
{
	setWindowTitle("[NO NAME] - Solution checker");
	ple_filePath->setText("");
	m_reportFileName = "";
	pte_reportArea->setEnabled(false);
	pte_reportArea->setText("<CENTER> YOUR REPORT WILL BE PRINTED HERE</CENTER>");
}

void ClientGUI::slotSetConnectionSettings()
{
	HostPortInputWindow settingsWindow;
	if (settingsWindow.exec() == QDialog::Accepted)
	{
		auto settings = settingsWindow.getHostPort();
		m_host = settings.first;
		m_port=  settings.second;
		ple_host->setText(m_host);
		ple_port->setText(m_port);

		QSettings settingss(qApp->applicationDirPath() = "/config/client.ini", QSettings::IniFormat);
		settingss.setValue("host", m_host);
		settingss.setValue("port", m_port);

		emit signalUpdateSettings();
	}
}

void ClientGUI::slotPostAnswer()
{
	if (ple_filePath->text() == "")
	{
		QMessageBox msgBox(this);
		msgBox.setText("No Path to the file!");
		msgBox.setIcon(QMessageBox::Critical);
		msgBox.setInformativeText("Enter your path firstly!");
		msgBox.setStandardButtons(QMessageBox::Ok);
		msgBox.exec();
		return;
	}

	if (ple_workType->text() == "")
	{
		QMessageBox msgBox(this);
		msgBox.setText("You must enter type of your work! (ex. PW-1)");
		msgBox.setIcon(QMessageBox::Critical);
		msgBox.setStandardButtons(QMessageBox::Ok);
		msgBox.exec();
		return;
	}
	
	ClientLogWindow clientLogWindow;

	connect(this, SIGNAL(signalPrintLog(QString)), &clientLogWindow, SLOT(slotPrintLog(QString)), Qt::DirectConnection);
	connect(this, SIGNAL(signalResponseSuccess()), &clientLogWindow, SLOT(slotResponseSuccess()), Qt::DirectConnection);
	connect(this, SIGNAL(signalResponseError(QString)), &clientLogWindow, SLOT(slotResponseError(QString)), Qt::DirectConnection);
	
	connect(&clientLogWindow, SIGNAL(signalStopConnection()), this, SIGNAL(signalStopConnection()));

	emit signalPostAnswer(ple_filePath->text(), ple_workType->text());
	
	clientLogWindow.exec();
}

void ClientGUI::slotGetReport()
{
	if (ple_filePath->text() == "")
	{
		QMessageBox msgBox(this);
		msgBox.setText("No Path to the file!");
		msgBox.setIcon(QMessageBox::Critical);
		msgBox.setInformativeText("Enter your path firstly!");
		msgBox.setStandardButtons(QMessageBox::Ok);
		msgBox.exec();
		return;
	}

	if (ple_workType->text() == "")
	{
		QMessageBox msgBox(this);
		msgBox.setText("You must enter type of your work! (ex. PW-1)");
		msgBox.setIcon(QMessageBox::Critical);
		msgBox.setStandardButtons(QMessageBox::Ok);
		msgBox.exec();
		return;
	}
	
	ClientLogWindow clientLogWindow;

	connect(this, SIGNAL(signalPrintLog(QString)), &clientLogWindow, SLOT(slotPrintLog(QString)), Qt::DirectConnection);
	connect(this, SIGNAL(signalResponseSuccess()), &clientLogWindow, SLOT(slotResponseSuccess()), Qt::DirectConnection);
	connect(this, SIGNAL(signalResponseError(QString)), &clientLogWindow, SLOT(slotResponseError(QString)), Qt::DirectConnection);
	
	connect(&clientLogWindow, SIGNAL(signalStopConnection()), this, SIGNAL(signalStopConnection()));

	emit signalGetReport(ple_filePath->text(), ple_workType->text());
	
	clientLogWindow.exec();
}

void ClientGUI::slotPrintReport(const QString& report)
{
	pte_reportArea->setEnabled(true);
	pte_reportArea->setText(report);
}
