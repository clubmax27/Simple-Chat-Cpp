#pragma comment(lib, "Ws2_32.lib")

#include <WinSock2.h>
#include <windows.h>
#include <stdio.h>
#include <sstream>
#include <iostream>
#include "Server.h"

using namespace std;

int ServerThread(struct parameters* args);
struct parameters
{
    Server* server;
    int ID;
};

inline int InitWinSock()
{
    int RetVal = 0;
    WSAData wsaData;
    WORD DLLVersion = MAKEWORD(2,1);
    RetVal = WSAStartup(DLLVersion, &wsaData);//Initialisation de WSA
    return RetVal;
}

inline Server::Server(int port)
{
    int RetVal = 0;
    RetVal = InitWinSock();
    if(RetVal != 0) //if the function InitWinSock worked, RetVal should be 0
    {
        MessageBoxA(NULL, "Winsock startup failed", "Error",MB_OK | MB_ICONERROR); //Send error
        exit(1);
    }

    Connections = (SOCKET*)calloc(64, sizeof(SOCKET));//Creation de l'array d'une taille de 64 de clients possible

    sListen = socket(AF_INET, SOCK_STREAM, NULL);//Socket pour écouter les connections entrantes
    sConnect = socket(AF_INET, SOCK_STREAM, NULL);//Socket pour stocker temporairement les infos de la socket du client

    //Configuration de caractèriques des sockets

    addr.sin_addr.s_addr = htonl(INADDR_ANY);//Aucune adresse IP a assigner
    addr.sin_port = htons(1234);//Assigner le port 1234
    addr.sin_family = AF_INET;

    int sock_err = bind(sListen, (SOCKADDR*)&addr, sizeof(addr));//Association des paramètres de la structure addr a la socket sListen
    if(sock_err == SOCKET_ERROR)//Si l'association échoue
    {
        cout << "Bind failed with error : " << WSAGetLastError() << endl;
        closesocket(sListen);
        WSACleanup();
        exit(1);
    }

    cout << "Server Connected " << endl;

    sock_err = listen(sListen, 9);//Ecoute des clients qui arrivent

    if (sock_err != SOCKET_ERROR)//Si aucune erreur lors de l'écoute
    {
        cout << "Waiting for a client connection..." <<endl;
    }
    int addrlen = sizeof(addr);

    //Boucle infinie
    while(1)
    {
        //Si un client demande une connection, on l'accepte
        if(sConnect = accept(sListen, (SOCKADDR*)&addr, &addrlen) )
        {
            if(sConnect!= INVALID_SOCKET)//Si aucune erreur n'a eu lieu lors de l'acceuil du client
            {
                int FreeID = 404;
                for(int i = 0; i < 64; i++)
                {
                    if(Connections[i] == 0)
                    {
                        FreeID = i;
                        break;
                    }
                }

               if(FreeID != 404)//Si une place est libre
                {
                    Connections[FreeID] = sConnect; //le tableau de socket Connection prend en charge la socket du client
                }
                else
                {
                    MessageBoxA(NULL, "Server full", "Error",MB_OK | MB_ICONERROR);
                }

                char* ID = new char[64];
                ZeroMemory(ID, sizeof(ID));
                stringstream ss;
                ss << FreeID;
                string temp = ss.str();//Convertion en string de la int ConCounter
                strcpy(ID, temp.c_str());//conertion en char* de la string temp
                send(Connections[FreeID], ID, sizeof(ID), NULL);//Donner au client son ID

                struct parameters args;
                args.server = this;
                args.ID = FreeID;

                CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE) ServerThread, &args, NULL, NULL);//On crée un thread pour gérer les imputs des clients, qui lance la fonction ServerThread
            }
        }
        Sleep(50);
    }
}

inline void Server::DistributeMessage(char* message)
{
    int bytesSend = INVALID_SOCKET;

    for(int i = 0; i < 64; i++)
    {
        bytesSend = send(Connections[i], message, 256, NULL);//Le thread envoie ce qu'a dit le client
    }
}

inline void Server::DistributeMessage(string message)
{
    char sendbuf[256];
    int bytesSend = INVALID_SOCKET;

    strcpy(sendbuf, message.c_str());
    for(int i = 0; i <= 9; i++)
    {
        bytesSend = send(Connections[i], sendbuf, 256, NULL);//Le thread envoie ce qu'a dit le client
    }
}
