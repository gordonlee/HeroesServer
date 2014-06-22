// Copyright 2014 GordonLee

#include "common_header/base_header.h"

typedef struct _SOCKET_TRAITS {
    SOCKET Socket;
    SOCKADDR_IN SocketAddr;
    bool IsSetupAddr;
    _SOCKET_TRAITS(void) : Socket(0), IsSetupAddr(false) {
        // ::memset(&Overlapped, 0, sizeof(Overlapped));
    }
} SocketTraits;

class TcpSocket {
 public:
     TcpSocket(void) {
     }

     virtual ~TcpSocket(void) {
     }

     virtual int Initialize(void) {
         return 0;
     }

     virtual int Initialize(const SOCKET _socket, const SOCKADDR_IN& _addr, const int _addrLen) {
         m_Socket.Socket = _socket;
         ::memcpy(&(m_Socket.SocketAddr), &_addr, _addrLen);

         int addrlen = sizeof(m_Socket.SocketAddr);
         getpeername(m_Socket.Socket, (SOCKADDR *)&m_Socket.SocketAddr, &addrlen);
         m_Socket.IsSetupAddr = true;
         return 0;
     }

     // Create Listen socket
     virtual int Bind(int _listenPort) {
         m_Socket.Socket = ::WSASocket(
             AF_INET,
             SOCK_STREAM,
             0,
             NULL,
             0,
             WSA_FLAG_OVERLAPPED);
         m_Socket.SocketAddr.sin_family = AF_INET;
         m_Socket.SocketAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);
         m_Socket.SocketAddr.sin_port = ::htons(_listenPort);
         m_Socket.IsSetupAddr = true;

         ::bind(
             m_Socket.Socket,
             reinterpret_cast<SOCKADDR *>(&m_Socket.SocketAddr),
             sizeof(m_Socket.SocketAddr));

         int result = ::listen(m_Socket.Socket, SOMAXCONN);
         if (result == SOCKET_ERROR) {
             return SOCKET_ERROR;
         }
         return m_Socket.Socket;
     }

     virtual SOCKET Accept(SOCKADDR_IN* _remoteAddr, int* _remoteLength) {
         SOCKET acceptedSocket = ::accept(
             m_Socket.Socket,
             reinterpret_cast<SOCKADDR*>(_remoteAddr),
             _remoteLength);

         return acceptedSocket;
     }

     virtual int SendAsync(void) {
         return 0;
     }

     virtual int Send(Buffer* _buffer, int _sendBytes) {
         if (_buffer && _sendBytes > 0) {
             return ::send(m_Socket.Socket, _buffer->GetPtr(), _sendBytes, 0);
         }
         return -1;
     }

     virtual int RecvAsync(Buffer* _buffer, const LPOVERLAPPED _overlapped) {
         DWORD recvBytes;
         DWORD flags = 0;
         WSABUF wsaBuf;
         wsaBuf.buf = _buffer->GetPtr();
         wsaBuf.len = BUFFER_SIZE;

         int result = ::WSARecv(
             m_Socket.Socket,
             &wsaBuf,
             1,
             &recvBytes,
             &flags,
             _overlapped,
             NULL);

         if (result == SOCKET_ERROR){
             if (WSAGetLastError() != ERROR_IO_PENDING) {
                 return -1;
             }
         }
         return 0;
     }

     virtual void Close(bool isForce) {
         ::closesocket(m_Socket.Socket);
     }

     SOCKET GetSocket() {
         return m_Socket.Socket;
     }

 private:
     SocketTraits m_Socket;
};
