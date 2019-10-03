#ifndef __CSOCKET_H__
#define __CSOCKET_H__

#include <WinSock.h>
#include <Windows.h>
#pragma comment(lib, "ws2_32")

extern WSADATA _wsadata;
extern bool _bWsaInitialized;

class Socket
{
protected:
	SOCKET s; // socket id
	bool bConnected, bTerminateBuffer; // flags
	int bufferLen; // default buffer length
	int last;

public:
	// Constructor/Destructor
	Socket(bool create=1, int type=SOCK_STREAM, int protocol=IPPROTO_TCP);
	Socket(SOCKET);
	Socket(Socket&);
	~Socket();
	int create(int type=SOCK_STREAM, int protocol=IPPROTO_TCP);
	
	// Inline definitions
	int set_buffer_size(int size) { int old = bufferLen; bufferLen = size; return old; }
	int get_buffer_size() { return bufferLen; }
	bool set_terminate_flag(bool value) { bool old = bTerminateBuffer; bTerminateBuffer = value; return old; }
	bool get_terminate_flag() { return bTerminateBuffer; }
	int get_last_length() { return last; }
	int get_connected() { return bConnected; }

	// Socket functions
	static char* getlasterror();
	int bind(int port, int addr=INADDR_ANY);
	int connect(char* host_name, int port);
	int connect_to(sockaddr_in* sa);
	int close();
	int listen(int backlog=5);
	Socket* accept(sockaddr_in* from=0);

	// I/O
	char* recv(char* buffer, int buflen=0);
	unsigned char* recv(unsigned char* buffer, int buflen=0);
	char* recv() { return recv((char*)0); }
	int send(char* buffer, int buflen=0);
	int send(unsigned char* buffer, int buflen=0);

	// DNS
	static char* nslookup(char* host_name);
	static DWORD nslookup_raw(char* host_name);

	// Stream operators
	Socket& operator<<(char* string)
	{
		send(string);
		return *this;
	}
	Socket& operator>>(char* buffer)
	{
		recv(buffer, bufferLen);
		return *this;
	}
};



#endif /* __CSOCKET_H__ */
