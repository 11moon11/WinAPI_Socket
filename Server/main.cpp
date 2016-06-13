#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCKAPI_

#include <Windows.h>
#include <iostream>
#include <string>
#include <thread>

#include "sockServer.h"

using namespace std;

void handleServer();

typedef struct _PACKET
{
	BYTE Operation;
	char Buffer[1024];
}PACKET, *PPACKET;

#define MESSAGE 8

char *serverCallback(void *params)
{
	printf("Callback function has been called\n");

	char *response = (char *)VirtualAlloc(NULL, 64, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	switch (((PPACKET)params)->Operation)
	{

	case MESSAGE:
		//MessageBox(NULL, ((PPACKET)params)->Buffer, NULL, MB_OK);
		strcat(response, "A message has ben displayed!");
		break;



	default:
		strcat(response, "Error: Invalid command.");
		break;
	}

	return response;
}

int main()
{
	string command;
	sockServer sS(65533, &serverCallback);

	if (sS.sockSetup())
		sS.sockStart();

	for (; true; Sleep(10))
	{
		cin >> command;

		if (command == "stop")
		{
			sS.sockStop();
		}
	}

	return 0;
}