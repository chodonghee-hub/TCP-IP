#include "cTcpManager.h"

cTcpManager::cTcpManager()
{
	//s = nullptr;
	s = new SOCKET;
	
	//cs = nullptr;
	cs = new SOCKET;

	if (!CheckWinSock()) { return; }
	*s = CreateServerSocket();
	if (!CheckValidSocket(*s)) { return; }
	if (!CheckBindSocket(*s)) { return; }
	if (!CheckListenSocket(*s)) { return; }
	*cs = AcceptClientSocket(*s);
	if (!CheckValidSocket(*cs)) { return; }

	thread recvThread = thread(ReceiveDataFrom, cs, this);
	thread sendThread = thread(SendMessageTo, cs, this);

	recvThread.join();
	sendThread.join();

	while (mFlag) { Sleep(1); }

	delete[] s;
	s = nullptr;

	delete[] cs;
	cs = nullptr;
	WSACleanup();
}

cTcpManager::~cTcpManager()
{
	delete[] s;
	s = nullptr;

	delete[] cs;
	cs = nullptr;
	WSACleanup();
}
 
bool cTcpManager::CheckWinSock()
{
	if (WSAStartup(WINSOCK_VERSION, &wsaData) != 0)
	{
		cout << "** WSAStartup error : " << WSAGetLastError() << endl;
		WSACleanup();
		return false;
	}
	return true;
}

const SOCKET & cTcpManager::CreateServerSocket()
{
	SOCKET tmp_s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	return tmp_s;
}

bool cTcpManager::CheckValidSocket(SOCKET & sock)
{
	if (sock == INVALID_SOCKET)
	{
		cout << "** INVALID_SOCKET error : " << WSAGetLastError() << endl;
		WSACleanup();
		closesocket(sock);
		return false;
	}
	return true;
}

bool cTcpManager::CheckBindSocket(SOCKET & sock)
{
	SOCKADDR_IN sock_addr;
	sock_addr.sin_family = AF_INET;
	sock_addr.sin_port = htons(8080);
	sock_addr.sin_addr.S_un.S_addr = htonl(ADDR_ANY);
	if (::bind(sock, (SOCKADDR*)&sock_addr, sizeof(sock_addr)) == SOCKET_ERROR)
	{
		cout << "** Bind socket error : " << WSAGetLastError() << endl;
		WSACleanup();
		closesocket(sock);
		return false;
	}
	return true;
}

bool cTcpManager::CheckListenSocket(SOCKET & sock)
{
	if (listen(sock, SOMAXCONN) != 0)
	{
		cout << "** Listen socket error : " << WSAGetLastError() << endl;
		WSACleanup();
		closesocket(sock);
		return false;
	}

	cout << "[ Init Server client ]" << endl;
	mFlag = true;
	return true;
}

const SOCKET & cTcpManager::AcceptClientSocket(SOCKET & server_sock)
{
	SOCKADDR_IN sock_addr;
	int sock_size = sizeof(sock_addr);
	SOCKET tmp_s = accept(server_sock, (SOCKADDR*)&sock_addr, &sock_size);
	return tmp_s;
}

void cTcpManager::ReceiveDataFrom(SOCKET * cli_sock, LPVOID lp)
{
	cTcpManager *p = (cTcpManager*)lp;
	while (p->mFlag)
	{
		ZeroMemory(p->Buffer, sizeof(p->Buffer));
		recv(*cli_sock, p->Buffer, sizeof(Buffer), 0);
		if (sizeof(Buffer) > 1)
			cout << "\n** Message from client : " << p->Buffer << "\n** Text your message : ";

		Sleep(1);
	}

	delete[] p;
	p = nullptr;
}

void cTcpManager::SendMessageTo(SOCKET * cli_sock, LPVOID lp)
{
	cTcpManager *p = (cTcpManager*)lp;
	while (p->mFlag)
	{
		ZeroMemory(p->Buffer, sizeof(p->Buffer));
		cout << "** Text your message : ";
		cin >> p->Buffer;//fgets(Buffer, sizeof(Buffer), 0);

		if (strncmp(p->Buffer, "exit", strlen("exit") + 1) == 0)
		{
			p->mFlag = false;
			cout << "[ Break Connection ]" << endl;
		}
		else
			send(*cli_sock, p->Buffer, sizeof(p->Buffer), 0);

		Sleep(1);
	}

	delete[] p;
	p = nullptr;
}
