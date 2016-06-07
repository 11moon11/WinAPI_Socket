#pragma once
#define WIN32_LEAN_AND_MEAN

#include <stdio.h>
#include <tchar.h>
#include <Windows.h>
#include <WinSock.h>

#pragma comment(lib,"ws2_32.lib")

#define DEFAULT_BUFLEN	512*128
#define MAX_CONNECTIONS 10

class sockServer
{
public:
	sockServer(int port, char *(*callback)(void*));
	bool sockSetup();
	void sockStart();

	~sockServer();

private:
	DWORD WINAPI serverThread();

	char *(*callback)(void*);
	TCHAR errMsg[100];
	SOCKET listenSock;
	SOCKET clientSock;
	int iSendResult;
	sockaddr_in sai;
	HANDLE hMutex;
	void *recData;
	void *senData;
	int iResult;
	bool active;
	WSADATA wd;
	int port;
};

