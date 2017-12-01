#pragma comment(lib, "Ws2_32.lib")

#include <WinSock2.h>
#include <windows.h>
#include <stdio.h>
#include <sstream>
#include <iostream>

using namespace std;

SOCKADDR_IN addr;
SOCKET sListen;
SOCKET sConnect;
SOCKET *Connections;

void DistributeMessage(string message)
{
    char sendbuf[256];
    int bytesSend = INVALID_SOCKET;

    strcpy(sendbuf, message.c_str());
    for(int i = 0; i <= 9; i++)
    {
        bytesSend = send(Connections[i], sendbuf, 256, NULL);//Le thread envoie ce qu'a dit le client
    }
}

void DistributeMessage(char* message)
{
    int bytesSend = INVALID_SOCKET;

    for(int i = 0; i < 64; i++)
    {
        bytesSend = send(Connections[i], message, 256, NULL);//Le thread envoie ce qu'a dit le client
    }
}

int addrlen = sizeof(addr);
int ConCounter = 1;

int ServerThread(int ID)
{
    int bytesRecv = SOCKET_ERROR;
    int bytesSend = SOCKET_ERROR;
    char recvbuf[256] = "";
    char sendbuf[256] = "";
    char IDtoAdd[1];

    while (1)
    {
        ZeroMemory (recvbuf, sizeof(recvbuf));//Remise a zéro de recvbuf

        bytesRecv = recv( Connections[ID], recvbuf, 256 ,NULL);//Le thread recoit ce qu'a dit le client
        if (strcmp(recvbuf, "exit") == 0)//Si le client a envoyé "exit"
        {
            Connections[ID] = 0;
            cout << "<Client " << ID + 1 << ":> Disconnected..." <<endl;
            return 0;
        }

        else
        {
            if (bytesRecv == SOCKET_ERROR)//Si le client c'est decconecté
            {
                int iError = WSAGetLastError();
                if(iError == 10054)
                {
                    WSASetLastError(0);
                    cout << "<Client " << ID + 1 << ":> User has closed the chat" <<endl;
                    string message;

                    stringstream ss;
                    ss << ID << "User has closed the chat";
                    message = ss.str();//Convertion en string de la int ConCounter

                    DistributeMessage(message);
                    Connections[ID] = 0;
                    return 0;
                }
                else
                {
                    WSASetLastError(0);

                }
            }
        }

        cout << "<Client " << ID + 1 << ":> " << recvbuf <<endl;//Afficher ce qu'a dit le client

        memcpy(sendbuf, recvbuf, 256);


        stringstream ss;
        if(ID <= 9)
        {
            ss << "0" << ID << sendbuf;
        }
        else ss << ID << sendbuf;
        string temp = ss.str();//Convertion en string de la int ConCounter
        strcpy(sendbuf, temp.c_str());//conertion en char* de la string temp

        DistributeMessage(sendbuf);
    }
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
    int RetVal = 0;
    RetVal = InitWinSock();
    if(RetVal != 0) //if the function InitWinSock worked, RetVal should be 0
    {
        MessageBoxA(NULL, "Winsock startup failed", "Error",MB_OK | MB_ICONERROR); //Send error
        return 1;
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
        return 1;
    }

    cout << "Server Connected " << endl;

    sock_err = listen(sListen, 9);//Ecoute des clients qui arrivent

    if (sock_err != SOCKET_ERROR)//Si aucune erreur lors de l'écoute
    {
        cout << "Waiting for a client connection..." <<endl;
    }

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

                Connections[FreeID] = sConnect; //le tableau de socket Connection prend en charge la socket du client

                char* ID = new char[64];
                ZeroMemory(ID, sizeof(ID));
                stringstream ss;
                ss << FreeID;
                string temp = ss.str();//Convertion en string de la int ConCounter
                strcpy(ID, temp.c_str());//conertion en char* de la string temp

                send(Connections[FreeID], ID, sizeof(ID), NULL);//Donner au client son ID
                CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE) ServerThread, (LPVOID)(FreeID), NULL, NULL);//On crée un thread pour gérer les imputs des clients, qui lance la fonction ServerThread
            }
        }
        Sleep(50);
    }
}
