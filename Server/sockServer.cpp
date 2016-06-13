#include "sockServer.h"

sockServer::sockServer(int port, char *(*callback)(void*))
{
	active = false;
	connections = 0;
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
		printf("CreateMutex failed with error: %d\n", GetLastError());
		printf("Server is already running!\n");

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
		printf("Socket failed with error: %ld\n", WSAGetLastError());

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
		servThread = std::thread(&sockServer::serverThread, this);
	}
	else
	{
		printf("Server thread is started already!\n");
	}
}

void sockServer::sockStop()
{
	if (active)
	{
		printf("Stopping server...\n");

		active = false;

		closesocket(listenSock);

		servThread.join();
		clientThread.join();

		printf("Server is stopped!\n");
	}
	else
	{
		printf("Server thread is not active!\n");
	}
}

void sockServer::serverThread()
{
	printf("Server thread is now active!\n");

	clientThread = std::thread(&sockServer::messageMonitor, this);

	for (; active == true; Sleep(10))
	{
		printf("Waiting for a client to connect...\n");
		clientSock[connections] = accept(listenSock, NULL, NULL);

		if (active)
			printf("Client #%d has connected!\n", connections);

		if (clientSock[connections] == INVALID_SOCKET)
		{
			printf("Accept failed with error: %d\n", WSAGetLastError());

			shutdown(clientSock[connections], SD_BOTH);
			closesocket(listenSock);
			WSACleanup();
		}

		mutexLock.lock();
		connections++;
		mutexLock.unlock();

		// When max limit of clients is reached - exit this loop
		if (connections == MAX_CONNECTIONS)
		{
			break;
		}
	}
}

void sockServer::messageMonitor()
{
	for (; active == true; Sleep(10))
	{
		for (int i = 0; i < connections; i++)
		{
			mutexLock.lock();
			iResult = recv(clientSock[i], reinterpret_cast<char *>(recData), DEFAULT_BUFLEN, 0);

			if (iResult > 0)
			{
				printf("Bytes received: %d from client #%d\n", iResult, i);

				senData = callback(recData);

				iSendResult = send(clientSock[i], reinterpret_cast<char *>(senData), strlen(reinterpret_cast<char *>(senData)), 0);

				if (iSendResult == SOCKET_ERROR)
				{
					printf("Send to client #%d failed with error: %d\n", i, WSAGetLastError());
					printf("Closing connection #%d\n", i);

					shutdown(clientSock[i], SD_SEND);
					closesocket(clientSock[i]);
				}

				printf("Bytes sent: %d\n", iSendResult);
			}

			VirtualFree(senData, 128, MEM_FREE);
			memset(recData, 0, sizeof(void*));
			memset(senData, 0, sizeof(void*));

			mutexLock.unlock();
		}
	}

	mutexLock.lock();
	for (int i = 0; i < connections; i++)
	{
		printf("Closing connection #%d\n", i);
		shutdown(clientSock[i], SD_SEND);
		closesocket(clientSock[i]);
	}
	mutexLock.unlock();
	WSACleanup();
}

sockServer::~sockServer()
{
	delete[] &errMsg;
	delete[] recData;
	delete[] senData;

	ReleaseMutex(hMutex);
}
