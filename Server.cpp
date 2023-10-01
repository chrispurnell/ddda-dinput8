#include "stdafx.h"
#include "Server.h"
#include "dinput8.h"

string host;
const char *hostPtr;
UINT16 port;
LPVOID oConnect;

void __declspec(naked) HConnect()
{
	asm("add	$12,%esp");
	asm("pushw	%0" : : "m"(port));
	asm("push	%0" : : "m"(hostPtr));
	asm("sub	$4,%esp");
	asm("jmp	*%0" : : "m"(oConnect));
}

void Hooks::Server()
{
	if (config.getBool("server", "enabled", false))
	{
		host = config.getStr("server", "host", "dune.dragonsdogma.com");
		hostPtr = host.c_str();
		port = config.getUInt("server", "port", 12501);
		LOG("Server: using %s:%u\n", host.c_str(), port);

		BYTE signature[] =
		{
			0x83, 0xEC, 0x24,             // sub     esp, 24h
			0x53,                         // push    ebx
			0x66, 0x8B, 0x5C, 0x24, 0x30, // mov     bx, [esp+28h+port]
			0x56,                         // push    esi
			0x68                          // push    offset nulls_string
		};

		BYTE *pOffset;
		if (FindSignature("sDDCaProto::connect", signature, &pOffset))
			CreateHook("sDDCaProto::connect", pOffset, (LPVOID)HConnect, &oConnect);
	}
	else
		LOG("Server: disabled\n");
}
