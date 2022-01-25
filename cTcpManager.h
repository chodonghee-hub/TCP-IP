#pragma once
#include <iostream>
#include <stdio.h>
#include <thread>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32.lib")

using namespace std;
const static int RECV_MAX = 1024;

class cTcpManager
{
private:
	SOCKET *s, *cs;
	WSADATA		wsaData;
	bool mFlag = false;
	char Buffer[RECV_MAX];

public:
	cTcpManager();
	~cTcpManager();
	bool CheckWinSock();
	const SOCKET& CreateServerSocket();
	bool CheckValidSocket(SOCKET &sock);
	bool CheckBindSocket(SOCKET &sock);
	bool CheckListenSocket(SOCKET &sock);
	const SOCKET& AcceptClientSocket(SOCKET &server_sock);
	static void ReceiveDataFrom(SOCKET *, LPVOID lp);
	static void SendMessageTo(SOCKET *cli_sock, LPVOID lp);
};

