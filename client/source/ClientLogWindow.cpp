#include "ClientLogWindow.h"

#include <QDateTime>
#include <QStyle>
#include <QMessageBox>

ClientLogWindow::ClientLogWindow(QWidget* parent) : QDialog(parent)
{
	setWindowFlags(Qt::WindowTitleHint);
	setWindowTitle("Log");
	QVBoxLayout* pvbl_layout = new QVBoxLayout;

	QHBoxLayout* phbl_logStatus = new QHBoxLayout;
	QLabel* plbl_log = new QLabel("Log status:");
	plbl_status = new QLabel("<b><p style=\"color:blue\">PROCESSING</p></b>");
	phbl_logStatus->addWidget(plbl_log);
	phbl_logStatus->addWidget(plbl_status);

	pte_logArea = new QTextEdit;
	pte_logArea->setReadOnly(true);

	QHBoxLayout* phbl_buttons = new QHBoxLayout;
	ppb_stop = new QPushButton("STOP");
	ppb_close = new QPushButton("CLOSE");
	ppb_close->setEnabled(false);
	connect(ppb_stop, SIGNAL(clicked()), this, SLOT(slotStopClicked()));
	connect(ppb_close, SIGNAL(clicked()), this, SLOT(slotCloseClicked()));
	phbl_buttons->addWidget(ppb_stop);
	phbl_buttons->addWidget(ppb_close);

	pvbl_layout->addLayout(phbl_logStatus);
	pvbl_layout->addWidget(pte_logArea);
	pvbl_layout->addLayout(phbl_buttons);

	setLayout(pvbl_layout);

}

ClientLogWindow::~ClientLogWindow()
{


}

void ClientLogWindow::reject()
{
	slotStopClicked();
	slotCloseClicked();
}

void ClientLogWindow::slotStopClicked()
{
	if (ppb_stop->isEnabled())
	{
		emit signalStopConnection();
	}
	ppb_stop->setEnabled(false);
	ppb_close->setEnabled(true);
}

void ClientLogWindow::slotCloseClicked()
{
	accept();
}

void ClientLogWindow::slotPrintLog(QString log)
{
	log.prepend("[" + QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss.zzz") + "] ");
	pte_logArea->append(log);
}

#include <QDebug>
void ClientLogWindow::slotResponseSuccess()
{
	slotPrintLog("Completed!");

	ppb_stop->setEnabled(false);
	ppb_close->setEnabled(true);

	plbl_status->setText("<b><p style=\"color:green\">SUCCESS<p></b>");

	QMessageBox msgBox(this);
	msgBox.setText("Completed Successfully");
	msgBox.setWindowTitle("Completed!");
	msgBox.addButton(new QPushButton(style()->standardIcon(QStyle::SP_DialogOkButton), "OK"), QMessageBox::AcceptRole);
	msgBox.setIcon(QMessageBox::Information);
	msgBox.exec();

	slotCloseClicked();
}

void ClientLogWindow::slotResponseError(const QString& err)
{
	slotPrintLog(err);

	ppb_stop->setEnabled(false);
	ppb_close->setEnabled(true);

	plbl_status->setText("<b><p style=\"color:red\">ERROR<p></b>");

	QMessageBox msgBox(this);
	msgBox.setText("An Error Occupied");
	msgBox.setWindowTitle("Error!");
	msgBox.setInformativeText(err);
	msgBox.addButton(new QPushButton(style()->standardIcon(QStyle::SP_DialogOkButton), "OK"), QMessageBox::AcceptRole);
	msgBox.setIcon(QMessageBox::Critical);
	msgBox.exec();

	slotCloseClicked();
}
