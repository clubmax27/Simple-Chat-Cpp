#pragma comment(lib, "Ws2_32.lib")

#include <WinSock2.h>
#include <Windows.h>
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <string>

using namespace std;

SOCKADDR_IN addr;

SOCKET sConnect;


int ClientThread(int ID)
{
    char tempID;
    char charID;
    char buffer[256];
    char message[256];

    for(;; Sleep(10))
    {
        sameID:
        int numRead = recv(sConnect, buffer, sizeof(buffer), NULL);
        if (numRead < 1) break;

        else
        {
            int RecvID = buffer[0] - '0';
            RecvID *= 10;
            RecvID += buffer[1] - '0';

            if(RecvID == ID)
            {
                goto sameID;
            }

            for(int i = 0; i < 256; i++)
            {
                message[i] = buffer[i+2];
            }

            cout << "<Client " << RecvID + 1 << ":> " << message << endl;
        }
    }

    return 0;
}

int main()
{
    system("cls");

    int RetVal = 0;

    WSAData wsaData;
    WORD DllVersion = MAKEWORD(2,1);
    RetVal = WSAStartup(DllVersion, &wsaData);
    if (RetVal != 0)
    {
        MessageBoxA(NULL, "Winsock startup failed", "Error", MB_OK | MB_ICONERROR);
        exit(1);
    }

    sConnect = socket(AF_INET, SOCK_STREAM, NULL);

    addr.sin_addr.s_addr = inet_addr("176.188.41.30");
    addr.sin_port        = htons(1234);
    addr.sin_family      = AF_INET;

    do
    {
        cout << "Connecting to Masterserver" <<endl;

        RetVal = connect(sConnect, (SOCKADDR*)&addr, sizeof(addr));
        if (RetVal == 0) break;

        MessageBoxA(NULL, "Could not connect to server", "Error", MB_OK | MB_ICONERROR);
    }
    while (true);

    cout << "Connected!" << endl;

    char cID[64];
    ZeroMemory(cID, 64);

    recv(sConnect, cID, 64, NULL);
    int ID = atoi(cID);

    cout << "You are Client No " << ID + 1 << endl;

    CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE) ClientThread, (LPVOID)(ID), NULL, NULL);

    send(sConnect, "Connected!", 10, NULL);
    for(;; Sleep(10))
    {
        string buffer;
        getline(cin, buffer);
        if (send(sConnect, buffer.c_str(), buffer.length(), NULL) < 1) exit(1);
        if(buffer == "exit") return 0;
    }

    return 0;
}
