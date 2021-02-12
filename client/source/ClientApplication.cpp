#include "ClientApplication.h"

#include "Client.h"
#include "ClientGUI.h"

#include <QDebug>

ClientApplication::ClientApplication(int argc, char** argv) : QApplication(argc, argv)
{
	m_client = new Client;
	m_clientGUI = new ClientGUI;

	connect(m_clientGUI, SIGNAL(signalPostAnswer(QString, QString)), m_client, SLOT(slotPostAnswer(QString, QString)));
	connect(m_clientGUI, SIGNAL(signalGetReport(QString, QString)), m_client, SLOT(slotGetReport(QString, QString)));
	connect(m_clientGUI, SIGNAL(signalUpdateSettings()), m_client, SLOT(slotUpdateSettings()));
	connect(m_clientGUI, SIGNAL(signalStopConnection()), m_client, SIGNAL(signalStopConnection()));

	connect(m_client, SIGNAL(signalPrintReport(QString)), m_clientGUI, SLOT(slotPrintReport(QString)));

	connect(m_client, SIGNAL(signalPrintLog(QString)), m_clientGUI, SIGNAL(signalPrintLog(QString)), Qt::DirectConnection);
	connect(m_client, SIGNAL(signalResponseSuccess()), m_clientGUI, SIGNAL(signalResponseSuccess()), Qt::DirectConnection);
	connect(m_client, SIGNAL(signalResponseError(QString)), m_clientGUI, SIGNAL(signalResponseError(QString)), Qt::DirectConnection);

	m_clientGUI->show();
}

ClientApplication::~ClientApplication()
{
	delete m_client;
	delete m_clientGUI;
}
