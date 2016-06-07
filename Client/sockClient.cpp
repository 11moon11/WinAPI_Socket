#include "sockClient.h"



sockClient::sockClient(char *host, int port)
{
	this->addr = host;
	this->port = port;
}

bool sockClient::sockSetup()
{
	printf("Starting Setup...\n");
	iResult = WSAStartup(MAKEWORD(1, 1), &wd);

	if (iResult != 0)
	{
		_sntprintf_s(errMsg, sizeof(errMsg), "WSAStartup failed with error: %d", iResult);

		return false;
	}

	recData = VirtualAlloc(NULL, 128, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	host = gethostbyname(addr);

	if (!host)
	{
		printf("\nError: Unable to resolve hostname.\n");

		WSACleanup();
		return 1;
	}

	ZeroMemory(&sai, sizeof(sai));
	sai.sin_family = AF_INET;
	sai.sin_addr = *((LPIN_ADDR)*host->h_addr_list);
	sai.sin_port = htons(port);
}

void sockClient::sendData(void *data, int size)
{
	memset(recData, 0, sizeof(void *));
	
	Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (Socket == INVALID_SOCKET)
	{
		printf("\nError: Unable to init socket.\n");
		return;
	}

	if (connect(Socket, reinterpret_cast<sockaddr*>(&sai), sizeof(sai)) == SOCKET_ERROR)
	{
		printf("\nError: Unable to connect to remote computer.\n");
		closesocket(Socket);
		return;
	}

	if (send(Socket, reinterpret_cast<char*>(data), size, 0) == SOCKET_ERROR)
	{
		printf("\nError: Unable to send data to remote computer.\n");
		closesocket(Socket);
		return;
	}

	printf("\nCommand successfully sent.\n");

	memset(recData, 0, 128);
	recv(Socket, reinterpret_cast<char*>(recData), 128, 0);

	printf("\n%s\n", reinterpret_cast<char*>(recData));
	closesocket(Socket);
}

sockClient::~sockClient()
{
}
