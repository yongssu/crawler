#define _CRT_SECURE_NO_WARNINGS
#include "socket.h"
using namespace std;

int Socket::socketCounter = 0;
Socket::Socket() :m_socket(0)
{
	start();
	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket == INVALID_SOCKET)
	{
		throw "INVALID_SOCKET";
	}
	m_refCounter = new int(1);
}

Socket::Socket(SOCKET s) :m_socket(s)
{
	start();
	m_refCounter = new int(1);
}

Socket::Socket(const Socket& o)
{
	m_refCounter = o.m_refCounter;
	(*m_refCounter)++;
	m_socket = o.m_socket;
	socketCounter++;
}

Socket::~Socket()
{
	if (!--(m_refCounter))
	{
		close();
		delete m_refCounter;
	}
	--socketCounter;
	if (!socketCounter)
		end();
}

Socket& Socket::operator=(Socket& o)
{
	(*o.m_refCounter)++;
	m_refCounter = o.m_refCounter;
	m_socket = o.m_socket;
	socketCounter++;
	return *this;
}

void Socket::start()
{
	if (!socketCounter)
	{
		WSADATA info;
		if (WSAStartup(MAKEWORD(2, 0), &info))
		{
			throw "Count not start WSA";
		}
	}
	++socketCounter;
}

void Socket::end()
{
	WSACleanup();
}

void Socket::close()
{
	closesocket(m_socket);
}

std::string Socket::receiveLine()
{
	std::string ret;
	while (1)
	{
		char r;
		switch (recv(m_socket, &r, 1, 0))
		{
		case 0:
			return ret;
		case -1:
			return "";
		}
		ret += r;
		if (r == '\n')
			return ret;
	}
}

std::string Socket::receiveBytes()
{
	std::string ret;
	char buf[1024];
	while (1)
	{
		//u_long arg = 0;
		//if (ioctlsocket(m_socket, FIONREAD, (u_long *)&arg) != 0)
			//break;
		//if (arg == 0)
			//break;
		//if (arg > 1024)
		int	arg = 1024;
		int rv = recv(m_socket, buf, arg, 0);
		if (rv <= 0)
			break;
		std::string t;
		t.assign(buf, rv);
		ret += t;
	}
	return ret;
}

void Socket::sendLine(std::string str)
{
	str += '\n';
	send(m_socket, str.c_str(), str.length(), 0);
}

void Socket::sendBytes(const std::string& str)
{
	send(m_socket, str.c_str(), str.length(), 0);
}

/////////////////////////////////////////////////////////////////
SocketServer::SocketServer(int port, int connections, TypeSocket type)
{
	sockaddr_in sa;
	memset(&sa, 0, sizeof(sa));
	
	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);
	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket == INVALID_SOCKET)
		throw "INVALID_SOCKET";

	if (type == NONBLOCKINGSOCKET)
	{
		u_long arg = 1;
		ioctlsocket(m_socket, FIONBIO, &arg);
	}

	if (bind(m_socket, (sockaddr*)&sa, sizeof(sockaddr_in)) == SOCKET_ERROR)
	{
		closesocket(m_socket);
		throw "INVALID_SOCKET";
	}
	listen(m_socket, connections);
}

Socket* SocketServer::accept_s()
{
	SOCKET new_sock = accept(m_socket, 0, 0);
	if (new_sock == INVALID_SOCKET)
	{
		int rc = WSAGetLastError();
		if (rc == WSAEWOULDBLOCK)
		{
			return 0;
		}
		else
		{
			throw "INVALID SOCKET";
		}
	}
	Socket* r = new Socket(new_sock);
	return r;
}

////////////////////////////////////////////////////////////////////////
SocketClient::SocketClient(const std::string &host, int port)
{
	std::string error;
	hostent *hp;
	if ((hp = gethostbyname(host.c_str())) == 0)
	{
		error = strerror(errno);
		throw error;
	}

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr = *((in_addr *)hp->h_addr);
	memset(&(addr.sin_zero), 0, 8);

	if (connect(m_socket, (sockaddr *)&addr, sizeof(sockaddr)))
	{
		error = strerror(WSAGetLastError());
		throw error;
	}
}


///////////////////////////////////////////////////////////////////////////////
SocketSelect::SocketSelect(Socket const* const s1, Socket const* const s2, TypeSocket type)
{
	FD_ZERO(&m_fds);
	FD_SET(const_cast<Socket*>(s1)->m_socket, &m_fds);
	if (s2)
	{
		FD_SET(const_cast<Socket*>(s2)->m_socket, &m_fds);
	}

	TIMEVAL tval;
	tval.tv_sec = 0;
	tval.tv_usec = 1;

	TIMEVAL *ptval;
	if (type == NONBLOCKINGSOCKET)
	{
		ptval = &tval;
	}
	else
	{
		ptval = 0;
	}

	if (select(0, &m_fds, (fd_set*)0, (fd_set*)0, ptval) == SOCKET_ERROR)
		throw "Error in select";
}

bool SocketSelect::readable(Socket const* const s)
{
	if (FD_ISSET(s->m_socket, &m_fds)) 
		return true;
	return false;
}