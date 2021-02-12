#include "ServerGUI.h"

#include <QLineEdit>
#include <QLabel>
#include <QTextEdit>
#include <QString>
#include <QDebug>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QPushButton>

#include <QSettings>
#include <QFile>

#include <QFileDialog>
#include <QMessageBox>

#include <QStatusBar>

#include <QApplication>

#include "HostPortInputWindow.h"
#include "DialogLogSettings.h"

ServerGUI::ServerGUI(QWidget* parent) : QMainWindow(parent)
{
	setWindowTitle("OFFLINE - SERVER CHECKER");
	resize(600, 500);

	//MENU
	QMenu* pmnu_Menu = new QMenu("&Menu");

	pmnu_Menu->addAction("&Dump log",
											 this,
											 SLOT(slotDumpLog()));
	pmnu_Menu->addAction("&Quit",
											 this,
											 SLOT(slotQuit()));
	menuBar()->addMenu(pmnu_Menu);

	QMenu* pmnu_Settings = new QMenu("&Settings");
	pmnu_Settings->addAction("&Set Server Settings",
											 this,
											 SLOT(slotSetSettings()));
	pmnu_Settings->addAction("&Set Log Settings",
											 this,
											 SLOT(slotSetLogSettings()));
	menuBar()->addMenu(pmnu_Settings);
	//
	
	QSettings settings(qApp->applicationDirPath() + "/config/server.ini", QSettings::IniFormat);

	//ONLINE TOOL BAR
	QToolBar* ptb_toolBar = new QToolBar;
	ptb_toolBar->setMovable(false);
	ptb_toolBar->setFloatable(false);
	setContextMenuPolicy(Qt::PreventContextMenu);
	
	QLabel* plbl_Host = new QLabel("&Host:");
	ptl_Host = new QLineEdit(settings.value("host", QVariant("127.0.0.1")).toString());
	ptl_Host->setReadOnly(true);
	plbl_Host->setBuddy(ptl_Host);

	QLabel* plbl_Port = new QLabel("&Port:");
	ptl_Port = new QLineEdit(settings.value("port", QVariant("33333")).toString());
	ptl_Port->setReadOnly(true);
	plbl_Port->setBuddy(ptl_Port);
	
	ptb_toolBar->addWidget(plbl_Host);
	ptb_toolBar->addWidget(ptl_Host);
	ptb_toolBar->addSeparator();
	ptb_toolBar->addWidget(plbl_Port);
	ptb_toolBar->addWidget(ptl_Port);
	//
	
	addToolBar(ptb_toolBar);

	//
	
	//LOG AREA
	pte_Log = new QTextEdit;
	pte_Log->setReadOnly(true);
	pte_Log->setEnabled(false);
	setCentralWidget(pte_Log);
	//
	
	//STATUSBAR
	p_statusBar = new QStatusBar;

	ppb_start = new QPushButton("START");
	ppb_stop = new QPushButton("STOP");
	ppb_stop->setEnabled(false);

	connect(ppb_start, SIGNAL(clicked()), this, SLOT(slotServerStart()));
	connect(ppb_stop, SIGNAL(clicked()), this, SLOT(slotServerStop()));

	plbl_Status = new QLabel("<b><p style=\"color:red\">OFFLINE</p></b>");

	QLabel* plbl_connections = new QLabel("Current Connections:");
	plbl_countOfConnections = new QLabel("0");

	p_statusBar->addWidget(ppb_start);
	p_statusBar->addWidget(ppb_stop);
	p_statusBar->addWidget(plbl_Status);
	p_statusBar->addWidget(plbl_connections);
	p_statusBar->addWidget(plbl_countOfConnections);

	setStatusBar(p_statusBar);
	//
}

ServerGUI::~ServerGUI()
{
}

void ServerGUI::slotServerStarted()
{
	pte_Log->setEnabled(true);
	setWindowTitle("ONLINE - SERVER CHECKER");
	plbl_Status->setText("<b><p style=\"color:green\">ONLINE</p></b>");

	ppb_start->setEnabled(false);
	ppb_stop->setEnabled(true);
}

void ServerGUI::slotServerStart()
{
	emit signalServerStart();
}

void ServerGUI::slotServerStop()
{
	emit signalServerStop();
	pte_Log->setEnabled(false);
	setWindowTitle("OFFLINE - SERVER CHECKER");
	plbl_Status->setText("<b><p style=\"color:red\">OFFLINE</p></b>");

	ppb_start->setEnabled(true);
	ppb_stop->setEnabled(false);
}

void ServerGUI::slotQuit()
{
	emit signalQuit();
}

void ServerGUI::slotNewConnection()
{
	plbl_countOfConnections->setText(QString::number(plbl_countOfConnections->text().toUInt() + 1));
}

void ServerGUI::slotDisconnected()
{
	plbl_countOfConnections->setText(QString::number(plbl_countOfConnections->text().toUInt() - 1));
}

void ServerGUI::slotSetSettings()
{
	bool Listening;
	if (plbl_Status->text() == "<b><p style=\"color:green\">ONLINE</p></b>")
		Listening = true;
	else
		Listening = false;

	HostPortInputWindow dialog(Listening, this);
	if (dialog.exec() == QDialog::Accepted)
	{
		ptl_Host->setText(dialog.getHostPort().first);
		ptl_Port->setText(dialog.getHostPort().second);
		emit signalUpdateSettings(dialog.getHostPort().first, dialog.getHostPort().second);
	}
}

void ServerGUI::slotSetLogSettings()
{
	DialogLogSettings dialog;
	if (dialog.exec() == QDialog::Accepted)
	{
		emit signalUpdateLogSettings(dialog.getLogs().first, dialog.getLogs().second);
	}
}

void ServerGUI::slotPrintLog(const QString& log)
{
	pte_Log->append(log);
}

void ServerGUI::slotDumpLog()
{
	emit signalDumpLog(pte_Log->toPlainText());
}
