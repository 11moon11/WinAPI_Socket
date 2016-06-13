#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCKAPI_

#include <Windows.h>
#include <stdio.h>

#include "sockClient.h"

typedef struct _PACKET
{
	BYTE Operation;
	char Buffer[1024];
}PACKET, *PPACKET;

#define MESSAGE 8

int main()
{
	PACKET data;
	char *addr = "127.0.0.1";
	memset(&data, 0, sizeof(data));

	strcat(data.Buffer, "Fuck this Shit!");
	data.Operation = MESSAGE;

	sockClient sc(addr, 65533);
	if (sc.sockSetup())
	{
		sc.sendData((char *)&data, sizeof(data));
	}

	printf("^_^\n");

	system("pause");
	return 0;
}