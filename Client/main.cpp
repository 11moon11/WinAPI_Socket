#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCKAPI_

#include <Windows.h>
#include <stdio.h>

#include "sockClient.h"

typedef struct _BACKDOOR_PACKET
{
	BYTE Operation;
	char Buffer[1024];
}BACKDOOR_PACKET, *PBACKDOOR_PACKET;

#define BACKDOOR_DISPLAY_MESSAGE 8

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	AllocConsole();
	freopen("CON", "wt", stdout);
	freopen("CON", "wt", stderr);

	BACKDOOR_PACKET data;
	char *addr = "127.0.0.1";
	memset(&data, 0, sizeof(data));

	strcat(data.Buffer, "Fuck this Shit!");
	data.Operation = BACKDOOR_DISPLAY_MESSAGE;

	sockClient sc(addr, 65533);
	if (sc.sockSetup())
	{
		sc.sendData((char *)&data, sizeof(data));
	}

	printf("^_^\n");
	return 0;
}