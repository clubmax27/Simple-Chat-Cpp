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
    int FreeID;
    SOCKET *Connections;
    void DistributeMessage(char* message);
    void DistributeMessage(string message);
private:
    SOCKADDR_IN addr;
    SOCKET sListen;
    SOCKET sConnect;
    int addrlen = sizeof(addr);
    int ConCounter = 1;
};

#endif // SERVER_H_INCLUDED
