#include "winsock2.h"
#include <stdio.h>
#include <conio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

class EasySocket
{
private:
	WSADATA wsaData;
	SOCKET m_socket;
public:

	EasySocket();

	void connectServer(std::string ip, std::string port);
	int sendData(std::string);
	std::string recvData(int);
	void fileSend(std::string);
	void fileReceive(std::string);
	void closeConnection();
};