#ifndef CLIENTAPPLICATION_H
#define CLIENTAPPLICATION_H

#include <QApplication>

class Client;
class ClientGUI;

class ClientApplication : public QApplication
{
Q_OBJECT

private:
	Client* m_client;
	ClientGUI* m_clientGUI;

public:
	ClientApplication(int argc, char** argv);
	~ClientApplication();
};

#endif //CLIENTAPPLICATION_H
