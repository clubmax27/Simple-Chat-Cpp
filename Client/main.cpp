#pragma comment(lib, "Ws2_32.lib")

#include <WinSock2.h>
#include <Windows.h>
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <string>
#include "Client.cpp"

using namespace std;

int ClientThread(LPVOID param)
{
    struct parameters* args;
    args = reinterpret_cast<parameters*>(param); //Transform LPVOID into parameters*

    int ID = args->ID;
    SOCKET sConnect = args->client->sConnect;
    char buffer[256];
    char message[256];

    for(;; Sleep(10))//Always, but sleep(10) reduce lag
    {
        int numRead = recv(sConnect, buffer, sizeof(buffer), NULL);//recieve the mesages from the client
        if (numRead < 1) break;//if error, restart the loop

        else
        {
            int RecvID = buffer[0] - '0';//Getting the first digit
            RecvID *= 10;
            RecvID += buffer[1] - '0';//Getting the 2nd digit

            if(RecvID == ID)//If the message is ours
            {
                continue;
            }

            for(int i = 0; i < 256; i++)
            {
                message[i] = buffer[i+2];//Stock the real message into char* message
            }

            cout << "<Client " << RecvID + 1 << ":> " << message << endl; // Show the message
        }
    }
    delete args->client;
    delete args;
    return 0;
}

int main()
{
    cout << "What IP do you want to connect to ?" << endl;

    string IP;
    getline(cin,IP);
    Client client(IP);

    for(;; Sleep(10))
    {
        string buffer;
        getline(cin, buffer);//Get what our client says
        if (send(client.sConnect, buffer.c_str(), buffer.length(), NULL) < 1) exit(1);//If the send didn't succeded, exit the program
        if(buffer == "exit") return 0;//If the user said "exit" we close the program
    }

    return 0;
}
