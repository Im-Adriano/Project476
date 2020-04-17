#include "EasySocket.h"



EasySocket::EasySocket()
{
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR)
		printf("Error at WSAStartup()\n");
}


void EasySocket::connectServer(std::string ip, std::string port)
{
	struct addrinfo* result = NULL,
		* ptr = NULL,
		hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	int iResult = getaddrinfo(ip.c_str(), port.c_str(), &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return;
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL;ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		m_socket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (m_socket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return;
		}

		// Connect to server.
		iResult = connect(m_socket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(m_socket);
			m_socket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (m_socket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		return;
	}
}

int EasySocket::sendData(std::string sendbuf)
{
	return send(m_socket, sendbuf.c_str(), sendbuf.size(), 0);
}


std::string EasySocket::recvData(int size)
{
	char *buf = new char[size];
	memset(buf, 0, sizeof(buf));
	recv(m_socket, buf, size, 0);
	std::string ret(buf);
	delete(buf);
	return ret;
}


void EasySocket::closeConnection()
{
	closesocket(m_socket);
	WSACleanup();
}


void EasySocket::fileReceive(std::string filename)
{

	//char length[50] = "";
	std::cout << filename;
	FILE* fw = fopen(filename.c_str(), "wb");
	if (fw == NULL) {
		perror("Error");
		return;
	}

	char buffer[1030];

	int size = 1024;

	while (size == 1024)
	{
		size = recv(m_socket, buffer, 1024, 0);

		if (size == 1024)
		{
			fwrite(buffer, 1024, 1, fw);
		}
		else
		{
			buffer[size] = '\0';
			fwrite(buffer, size, 1, fw);
		}
	}

	fclose(fw);

}

void EasySocket::fileSend(std::string fpath)
{
	std::ifstream myFile(fpath, std::ios::in | std::ios::binary | std::ios::ate);
	int size = (int)myFile.tellg();
	myFile.close();

	FILE* fr = fopen(fpath.c_str(), "rb");

	while (size > 0)
	{
		char buffer[1030];

		if (size >= 1024)
		{
			fread(buffer, 1024, 1, fr);
			send(m_socket, buffer, 1024, 0);

		}
		else
		{
			fread(buffer, size, 1, fr);
			buffer[size] = '\0';
			send(m_socket, buffer, size, 0);
		}


		size -= 1024;

	}

	fclose(fr);

}