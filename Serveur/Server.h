#pragma comment(lib, "Ws2_32.lib")
#include <WinSock2.h>
#include <string>

using namespace std;

#ifndef SERVER_H_INCLUDED
#define SERVER_H_INCLUDED

class Server
{
public:
    Server(int port);
    friend int ServerThread(Server* server);
    void DistributeMessage(char* message);
    void DistributeMessage(string message);
private:
    SOCKADDR_IN addr;
    SOCKET sListen;
    SOCKET sConnect;
    SOCKET *Connections;
    int FreeID;
};

#endif // SERVER_H_INCLUDED
