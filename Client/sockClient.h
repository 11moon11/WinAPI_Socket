#pragma once
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <tchar.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_BUFLEN	512*128

class sockClient
{
public:
	sockClient(char *host, int port);
	bool sockSetup();
	void sendData(void *data, int size);

	~sockClient();


private:
	TCHAR errMsg[100];
	sockaddr_in sai;
	SOCKET Socket;
	hostent* host;
	void *recData;
	int iResult;
	WSADATA wd;
	char *addr;
	int port;
};

