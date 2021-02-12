#ifndef CLIENTLOGWINDOW_H
#define CLIENTLOGWINDOW_H

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>
#include <QDialog>

class ClientLogWindow : public QDialog
{
Q_OBJECT

private:
	QLabel* plbl_status;

	QTextEdit* pte_logArea;

	QPushButton* ppb_stop;
	QPushButton* ppb_close;

public:
	ClientLogWindow(QWidget* parent = 0);
	~ClientLogWindow();

protected:
	void reject() override;

public slots:
	void slotPrintLog(QString log);
	void slotResponseSuccess();
	void slotResponseError(const QString& err);

	void slotStopClicked();
	void slotCloseClicked();

signals:
	void signalStopConnection();
};

#endif //CLIENTLOGWINDOW_H
