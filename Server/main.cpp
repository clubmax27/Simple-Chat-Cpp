#include "Server.cpp"

using namespace std;

int main()
{
    Server server(1234);
}

int ServerThread(struct parameters* args)
{
    int ID = args->ID;
    SOCKET* Connections = args->server->Connections;
    int bytesRecv = SOCKET_ERROR;
    int bytesSend = SOCKET_ERROR;
    char recvbuf[256] = "";
    char sendbuf[256] = "";
    char IDtoAdd[1];

    while (1)
    {
        ZeroMemory (recvbuf, sizeof(recvbuf));//Remise a zéro de recvbuf
        bytesRecv = recv(Connections[ID], recvbuf, 256 ,NULL);//Le thread recoit ce qu'a dit le client
        if(strcmp(recvbuf, "exit") == 0)//Si le client a envoyé "exit"
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
                    cout << "<Client " << ID + 1 << ":> Disconnected..." <<endl;
                    string message;

                    stringstream ss;
                    if(ID <= 9)
                    {
                        ss << "0" << ID << "Disconnected...";
                    }
                    else
                    {
                        ss << ID << "Disconnected...";
                    }
                    message = ss.str();//Convertion en string de la int ConCounter

                    args->server->DistributeMessage(message);
                    Connections[ID] = 0;
                    return 0;
                }
                else
                {
                    WSASetLastError(0);

                }
            }
            if(bytesRecv == 0)
            {
                cout << "<Client " << ID + 1 << ":> Disconnected..." <<endl;
                string message;

                stringstream ss;
                ss << ID << "Disconnected...";
                message = ss.str();//Convertion en string de la int ConCounter

                args->server->DistributeMessage(message);
                Connections[ID] = 0;
                return 0;
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

        args->server->DistributeMessage(sendbuf);
    }
}
