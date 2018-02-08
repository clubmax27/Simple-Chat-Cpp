#pragma comment(lib, "Ws2_32.lib")

#include <WinSock2.h>
#include <windows.h>
#include <stdio.h>
#include <sstream>
#include <iostream>
#include "Server.cpp"

using namespace std;

int main()
{
    Server(1234);
}
