#include "socket.h"

/*
 * Sockets class implementation
 */

// Global variables and flags
WSADATA _wsadata;
bool _bWsaInitialized = 0;
char error[256];
void _InitWsa()
{
	if(!_bWsaInitialized)
	{
		WSAStartup(0x202, &_wsadata);
		_bWsaInitialized = true;
	}
}

// Constructor
Socket::Socket(bool create, int type, int protocol):bConnected(0),s(0),bufferLen(1024),bTerminateBuffer(1),last(0)
{
	// Start WinSock, if it's not started
	_InitWsa();

	// Create socket if need
	if(create)
		s = socket(AF_INET, type, protocol);
}

// Copy constructor
Socket::Socket(Socket& pSock)
{
	// Start WinSock, if it's not started
	_InitWsa();

	// Copy object
	bConnected = pSock.bConnected;
	s = pSock.s;
	bufferLen = pSock.bufferLen;
	bTerminateBuffer = pSock.bTerminateBuffer;
}

// Destructor
Socket::~Socket()
{
	// Disconnect if connected
	if(bConnected)
		close();
}

// Create new socket
int Socket::create(int type, int protocol)
{
	s = socket(AF_INET, type, protocol);
	if(s==-1)
		return -1;
	else
		return 0;
}

// Bind to the specified address & port
int Socket::bind(int port, int addr)
{
	// Cannot bind if connected
	if(bConnected)
	{
		SetLastError(ERROR_CONNECTION_ACTIVE);
		return -1;
	}

	// Bind
	SOCKADDR_IN sa = {0};
	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);
	sa.sin_addr.s_addr = addr;
	return ::bind(s, (const sockaddr*)&sa, sizeof(sa));
}

// Turn socket to listening mode
int Socket::listen(int backlog)
{
	// Cannot listen if connected
	if(bConnected)
	{
		SetLastError(ERROR_CONNECTION_ACTIVE);
		return -1;
	}
	return ::listen(s, backlog);
}

// Connect to the specified address & port
int Socket::connect(char* host_name, int port)
{
	// Cannot connect if connected
	if(bConnected)
	{
		SetLastError(ERROR_CONNECTION_ACTIVE);
		return -1;
	}

	// Resolve host name
	SOCKADDR_IN sa = {0};
	sa.sin_family = AF_INET;
	HOSTENT* phe = ::gethostbyname(host_name);
	if(!phe)
	{
		SetLastError(/*ERROR_DS_LOOKUP_FAILURE*/8524);
		return -1;
	}
	for(int i=0;i<4;i++) // Copy resolved address
		((BYTE*)&sa.sin_addr.s_addr)[i] = phe->h_addr_list[0][i];
	sa.sin_port = htons(port);

	// Connect
	int ret = ::connect(s, (const sockaddr*)&sa, sizeof(sa));
	if(ret!=-1)
		bConnected = 1;
	return ret;
}

// Connect using specified SOCKADDR_IN struct
int Socket::connect_to(sockaddr_in *sa)
{
	// Cannot connect if connected
	if(bConnected)
	{
		SetLastError(ERROR_CONNECTION_ACTIVE);
		return -1;
	}

	// Connect
	int ret = ::connect(s, (const sockaddr*)sa, sizeof(*sa));
	if(ret!=-1)
		bConnected = 1;
	return ret;
}

// Close opened connection
int Socket::close()
{
	// Cannot close terminated connection
	if(!bConnected)
	{
		SetLastError(ERROR_CONNECTION_INVALID);
		return -1;
	}

	// Close
	int ret = ::closesocket(s);
	if(ret!=-1)
		bConnected = 0;
	return ret;
}

// Accept new connection
Socket* Socket::accept(sockaddr_in* from)
{
	// Cannot accept if connected
	if(bConnected)
	{
		SetLastError(ERROR_CONNECTION_ACTIVE);
		return NULL;
	}

	// Create new object
	Socket* acpt = new Socket(false);
	
	// Prepare
	sockaddr_in* sa;
	if(from)
		sa = from;
	else
		sa = new sockaddr_in;
	ZeroMemory(sa, sizeof(sockaddr_in));
	sa->sin_family = AF_INET;

	// Accept
	int namelen = sizeof(sockaddr_in);
	acpt->s = ::accept(s, (sockaddr*)sa, &namelen);

	if(!from)
		delete sa;

	if(acpt->s == -1)
	{
		delete acpt;
		return NULL;
	}

	acpt->bConnected = 1;
	return acpt;
}

// Receive data
char* Socket::recv(char* buffer, int buflen)
{
	// Can't read from closed socket
	if(!bConnected)
	{
		SetLastError(ERROR_CONNECTION_INVALID);
		return 0;
	}
	
	// Create buffer if need
	char* buf;
	int len;

	if(buffer)
	{
		buf = buffer;
		len = buflen;
	}
	else
	{
		if(bTerminateBuffer)
			buf = new char[bufferLen+1];
		else
			buf = new char[bufferLen];
		len = bufferLen;
	}

	// Receive
	int ret = ::recv(s, buf, len, 0);
	if(ret==-1)
	{
		bConnected = 0;
		SetLastError(WSAECONNRESET);
		if(!buffer)
			delete buf;
		return NULL;
	}
	last = ret;

	// Store '\0' if need
	if(bTerminateBuffer)
		buf[ret] = 0;

	return buf;
}
unsigned char* Socket::recv(unsigned char *buffer, int buflen)
{
	return (unsigned char*)recv((char*)buffer, buflen);
}

// Send data
int Socket::send(char* buffer, int buflen)
{
	// Can't write to closed socket
	if(!bConnected)
	{
		SetLastError(ERROR_CONNECTION_INVALID);
		return 0;
	}
	
	if(!buflen)
		buflen = strlen(buffer);

	// Send
	int ret = ::send(s, buffer, buflen, 0);
	if(ret==-1)
	{
		bConnected = 0;
		SetLastError(WSAECONNRESET);
	}
	return ret;
}
int Socket::send(unsigned char *buffer, int buflen)
{
	return send((char*)buffer, buflen);
}


// Get last error (string)
char* Socket::getlasterror()
{
	DWORD err = GetLastError();
	FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, 0, err, 0, error, 1024, 0);
	return error;
}

// DNS lookup
char* Socket::nslookup(char* host_name)
{
	SOCKADDR_IN sa = {0};
	HOSTENT* phe = ::gethostbyname(host_name);
	if(!phe)
	{
		SetLastError(/*ERROR_DS_LOOKUP_FAILURE*/8524);
		return 0;
	}
	for(int i=0;i<4;i++) // Copy resolved address
		((BYTE*)&sa.sin_addr.s_addr)[i] = phe->h_addr_list[0][i];
	return inet_ntoa(sa.sin_addr);
}

// Raw DNS lookup
DWORD Socket::nslookup_raw(char* host_name)
{
	HOSTENT* phe = ::gethostbyname(host_name);
	if(!phe)
	{
		SetLastError(/*ERROR_DS_LOOKUP_FAILURE*/8524);
		return 0;
	}
	DWORD ret;
	for(int i=0;i<4;i++) // Copy resolved address
		((BYTE*)&ret)[i] = phe->h_addr_list[0][i];
	return ret;
}