#pragma comment(lib, "Ws2_32.lib")

#include <WinSock2.h>
#include <string>

using namespace std;

#ifndef CLIENT_H_INCLUDED
#define CLIENT_H_INCLUDED

class Client
{
public:
    Client(string IP);
    friend int ClientThread(LPVOID param);
    friend int main();
private:
    SOCKADDR_IN addr;
    SOCKET sConnect;
};

#endif // CLIENT_H_INCLUDED
