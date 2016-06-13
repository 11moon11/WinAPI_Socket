#pragma once
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <mutex>
#include <thread>
#include <vector>
#include <stdio.h>
#include <tchar.h>
#include <Windows.h>
#include <WinSock2.h>

#pragma comment(lib,"ws2_32.lib")

#define DEFAULT_BUFLEN	512*128
#define MAX_CONNECTIONS 10

class sockServer
{
public:
	sockServer(int port, char *(*callback)(void*));
	bool sockSetup();
	void sockStart();
	void sockStop();

	~sockServer();

private:
	void serverThread();
	void messageMonitor();

	std::thread clientThread;
	std::thread servThread;
	std::mutex mutexLock;

	SOCKET clientSock[MAX_CONNECTIONS];
	SOCKET listenSock;
	int connections;
	void *recData;
	void *senData;

	char *(*callback)(void*);
	TCHAR errMsg[100];
	int iSendResult;
	sockaddr_in sai;
	HANDLE hMutex;
	int iResult;
	bool active;
	WSADATA wd;
	int port;
};

