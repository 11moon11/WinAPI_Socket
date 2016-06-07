#include "sockServer.h"

sockServer::sockServer(int port, char *(*callback)(void*))
{
	active = false;
	this->port = port;
	this->callback = callback;
}

bool sockServer::sockSetup()
{
	// Trying to create a Mutex
	hMutex = CreateMutex(NULL, TRUE, "{SOCKSERVER-BY-11MOON11}");

	// Making sure no other instance of our mutex exists
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		// One instance of our mutex already exists - exiting
		_sntprintf_s(errMsg, sizeof(errMsg), "CreateMutex failed with error: %d", GetLastError());

		return false;
	}

	// Setting up Winsock
	iResult = WSAStartup(MAKEWORD(1, 1), &wd);
	if (iResult != 0)
	{
		// Error setting up Winsock
		_sntprintf_s(errMsg, sizeof(errMsg), "WSAStartup failed with error: %d", iResult);

		return false;
	}

	// Cleaning and setting up Socket Addr Information
	ZeroMemory(&sai, sizeof(sai));
	sai.sin_family		= AF_INET;
	sai.sin_addr.s_addr = INADDR_ANY;
	sai.sin_port		= htons(port);

	// Allocating memory for reciving data and creating a socket
	recData		= (void *)VirtualAlloc(NULL, DEFAULT_BUFLEN, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	listenSock	= socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// Making sure socket started properly
	if (listenSock == INVALID_SOCKET)
	{
		// Error occurred wile staring socket - exiting
		_sntprintf_s(errMsg, sizeof(errMsg), "Socket failed with error: %ld", WSAGetLastError());

		WSACleanup();
		return false;
	}

	// Binding information and starting listening for connections
	bind(listenSock, (sockaddr*)&sai, sizeof(sai));
	listen(listenSock, MAX_CONNECTIONS);

	// We did it!
	printf("Setup() was successfull!\n");
	return true;
}


void sockServer::sockStart()
{
	if (!active)
	{
		printf("Starting server thread...\n");
		active = true;
		serverThread();
	}
	else
	{
		printf("Server thread is started already!\n");
	}
}

DWORD WINAPI sockServer::serverThread()
{
	printf("Server thread is now active!\n");

	for (; active == true; Sleep(10))
	{
		printf("Waiting for a client to connect...\n");
		clientSock = accept(listenSock, NULL, NULL);
		printf("Client has connected!\n");

		if (clientSock == INVALID_SOCKET)
		{
			_sntprintf_s(errMsg, sizeof(errMsg), "Accept failed with error: %d", WSAGetLastError());

			closesocket(listenSock);
			WSACleanup();
			break;
		}

		iResult = recv(clientSock, reinterpret_cast<char *>(recData), DEFAULT_BUFLEN, 0);

		if (iResult > 0)
		{
			printf("Bytes received: %d\n", iResult);

			senData = callback(recData);
			
			iSendResult = send(clientSock, reinterpret_cast<char *>(senData), strlen(reinterpret_cast<char *>(senData)), 0);

			if (iSendResult == SOCKET_ERROR)
			{
				printf("Send failed with error: %d\n", WSAGetLastError());
				closesocket(clientSock);
				WSACleanup();
				break;
			}

			printf("Bytes sent: %d\n", iSendResult);
		}
		else if (iResult < 0)
			printf("recv failed with error: %d\n", WSAGetLastError());

		VirtualFree(senData, 128, MEM_FREE);
		memset(recData, 0, sizeof(void*));
		memset(senData, 0, sizeof(void*));
	}

	active = false;
	return 0;
}

sockServer::~sockServer()
{
	delete[] &errMsg;
	delete[] recData;
	delete[] senData;

	ReleaseMutex(hMutex);
}
