#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCKAPI_

#include <Windows.h>
#include <stdio.h>

#include "sockClient.h"

typedef struct PACKET
{
	BYTE Operation;
	char Buffer[1024];
}BACKDOOR_PACKET, *PPACKET;

#define DISPLAY_MESSAGE 8

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	AllocConsole();
	freopen("CON", "wt", stdout);
	freopen("CON", "wt", stderr);

	PACKET data;
	char *addr = "127.0.0.1";
	memset(&data, 0, sizeof(data));

	strcat(data.Buffer, "Hello form client!");
	data.Operation = DISPLAY_MESSAGE;

	sockClient sc(addr, 65533);
	if (sc.sockSetup())
	{
		sc.sendData((char *)&data, sizeof(data));
	}

	printf("^_^\n");
	return 0;
}