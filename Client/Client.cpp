#pragma comment(lib, "Ws2_32.lib")

#include <WinSock2.h>
#include <Windows.h>
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <string>
#include "Client.h"

using namespace std;

int InitWinSock();
int ClientThread(struct parameters* args);
struct parameters
{
    Client* client;
    int ID;
};

Client::Client(string IP)
{
    int RetVal = InitWinSock();//Initialize WinSock
    if (RetVal != 0)
    {
        MessageBoxA(NULL, "Winsock startup failed", "Error", MB_OK | MB_ICONERROR);
        exit(1);
    }

    sConnect = socket(AF_INET, SOCK_STREAM, NULL);

    addr.sin_addr.s_addr = inet_addr(IP.c_str());
    addr.sin_port        = htons(1234);
    addr.sin_family      = AF_INET;

    do
    {
        cout << "Connecting to server..." <<endl;

        RetVal = connect(sConnect, (SOCKADDR*)&addr, sizeof(addr));//connect to the server
        if (RetVal == 0) break;//if didn't succeded

        MessageBoxA(NULL, "Could not connect to server", "Error", MB_OK | MB_ICONERROR);
    }while (true);//While not connected

    cout << "Connected!" << endl;
    cout << "--------------------------------------" << endl;

    char buffer[64];

    recv(sConnect, buffer, 64, NULL);//Recieveing the ID of our client
    int ID = atoi(buffer); //Transform the buffer in int

    cout << "You are Client No " << ID + 1 << endl;
    send(sConnect, "Connected!", 10, NULL);//Sending to the server the message "Connected!"

    struct parameters args;
    args.ID = ID;
    args.client = this;

    CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE) ClientThread, &args, NULL, NULL);//Create a thread for listeling to other client message
}

int InitWinSock()
{
    int RetVal = 0;
    WSAData wsaData;
    WORD DLLVersion = MAKEWORD(2,1);
    RetVal = WSAStartup(DLLVersion, &wsaData);//Initialisation de WSA
    return RetVal;
}
