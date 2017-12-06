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

    for(;; Sleep(10))//Always, but sleep(10) reduce lag
    {
        sameID:
        int numRead = recv(sConnect, buffer, sizeof(buffer), NULL);//recieve the mesages from the client
        if (numRead < 1) break;//if error, restart the loop

        else
        {
            int RecvID = buffer[0] - '0';//Getting the first digit
            RecvID *= 10;
            RecvID += buffer[1] - '0';//Getting the 2nd digit

            if(RecvID == ID)//If the message is ours
            {
                goto sameID;
            }

            for(int i = 0; i < 256; i++)
            {
                message[i] = buffer[i+2];//Stock the real message into char* message
            }

            cout << "<Client " << RecvID + 1 << ":> " << message << endl; // Show the message
        }
    }

    return 0;
}

int InitWinSock()
{
    int RetVal = 0;
    WSAData wsaData;
    WORD DLLVersion = MAKEWORD(2,1);
    RetVal = WSAStartup(DLLVersion, &wsaData);//Initialisation de WSA

    return RetVal;

}

int main()
{
    system("cls");//Clear the command prompt

    int RetVal = InitWinSock();//Initialize WinSock
    if (RetVal != 0)
    {
        MessageBoxA(NULL, "Winsock startup failed", "Error", MB_OK | MB_ICONERROR);
        exit(1);
    }

    cout << "What IP do you want to connect to ?" << endl;

    string IP;
    getline(cin,IP);

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
    }
    while (true);//While not connected

    cout << "Connected!" << endl;
    cout << "--------------------------------------" << endl;

    char cID[64];
    ZeroMemory(cID, 64);

    recv(sConnect, cID, 64, NULL);//Recieveing the ID of our client
    int ID = atoi(cID);

    cout << "You are Client No " << ID + 1 << endl;

    CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE) ClientThread, (LPVOID)(ID), NULL, NULL);//Create a thread for listeling to other client message

    send(sConnect, "Connected!", 10, NULL);//Sending to the server the message "Connected!"
    for(;; Sleep(10))
    {
        string buffer;
        getline(cin, buffer);//Get what our client says
        if (send(sConnect, buffer.c_str(), buffer.length(), NULL) < 1) exit(1);//If the send didn't succeded, exit the program
        if(buffer == "exit") return 0;//If the user said "exit" we close the program
    }

    return 0;
}
