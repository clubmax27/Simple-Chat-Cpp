#pragma comment(lib, "Ws2_32.lib")
#include <WinSock2.h>
#include <string>

using namespace std;
struct parameters;

#ifndef SERVER_H_INCLUDED
#define SERVER_H_INCLUDED

class Server
{
public:
    Server(int port);
    friend int ServerThread(struct parameters* args);
    void DistributeMessage(char* message);
    void DistributeMessage(string message);
private:
    SOCKADDR_IN addr;
    SOCKET sListen;
    SOCKET sConnect;
    SOCKET *Connections;
};

#endif // SERVER_H_INCLUDED
