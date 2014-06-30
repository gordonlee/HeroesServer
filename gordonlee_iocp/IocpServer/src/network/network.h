// Copyright 2014 Gordonlee

// FIXME: remove stdio.h
#include <stdio.h>

#include "common_header/base_header.h"
#include "iocp/CIocp.h"
#include "utility/AutoLock.h"
#include "utility/Buffer.h"
#include "socket/TcpSocket.h"

typedef struct _PER_HANDLE_DATA {
    OVERLAPPED Overlapped;
    TcpSocket Socket;
    Buffer Buffer;
} PER_HANDLE_DATA, *LPPER_HANDLE_DATA;

namespace {
    // FIXME: replace this function to log class
    void err_display(char *msg) {
        LPVOID lpMsgBuf;
        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
            NULL,
            WSAGetLastError(),
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR)&lpMsgBuf,
            0,
            NULL);
        printf("[%s] %s", msg, (LPCTSTR)lpMsgBuf);
        LocalFree(lpMsgBuf);
    }
}



unsigned int __stdcall WorkerThread2(LPVOID lpParam) {
    HANDLE hcp = (HANDLE)lpParam;
    int retval;

    while (1)
    {
        // 비동기 입출력 완료 기다리기
        DWORD cbTransferred;
        SOCKET client_sock;
        LPPER_HANDLE_DATA ptr;
        retval = GetQueuedCompletionStatus(
            hcp, 
            &cbTransferred, 
            (LPDWORD)&client_sock, 
            (LPOVERLAPPED*)&ptr, 
            INFINITE);
    }
    return 0;
}

//DWORD WINAPI WorkerThread(LPVOID lpParam) {
unsigned int __stdcall WorkerThread(LPVOID lpParam) {

    // TODO: write down worker thread.
    HANDLE handle = (HANDLE)lpParam;
    int retval;
    DWORD cbTransferred;
    SOCKET client_socket = 0;
    LPPER_HANDLE_DATA data = NULL;

    while (TRUE) {
        retval = ::GetQueuedCompletionStatus(
            handle,
            &cbTransferred,
            (PULONG_PTR)&client_socket,
            (LPOVERLAPPED *)(&data),
            INFINITE);

        DWORD lastError = GetLastError();

        if (retval == 0 || cbTransferred == 0) {
            if (retval == 0) {
                DWORD temp1, temp2;
                if (data != NULL)
                {
                    ::WSAGetOverlappedResult(
                        data->Socket.GetSocket(),
                        &(data->Overlapped),
                        &temp1,
                        FALSE,
                        &temp2);
                }

                err_display("WSAGetOverlappedResult()");
            }

            SOCKADDR_IN clientaddr;
            int addrlen = sizeof(clientaddr);
            getpeername(data->Socket.GetSocket(), (SOCKADDR *)&clientaddr, &addrlen);

            printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
                inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
            
            data->Socket.Close(false);
            //FIXME: delete data at here is dangerous!
            delete data;
            continue;
        }
        else if (data) {
            if (cbTransferred > 0) {
                printf("READ 데이터 : %s\n", data->Buffer.GetPtr());

                data->Socket.Send(&(data->Buffer), cbTransferred);

                // MEMO: 여기서 굳이 버퍼 초기화를 할 필요는 없지만, 출력을 위해서 비워둔다.
                // 또, send가 WSASend 가 되면, 오히려 센드버퍼는 지우면 안된다. (버퍼를 따로 둬얄듯)
                data->Buffer.Clear();
            }

            ::ZeroMemory(&(data->Overlapped), sizeof(data->Overlapped));
            
            if (data->Socket.RecvAsync(&(data->Buffer), &(data->Overlapped)) != 0) {
                err_display("WSARecv() Error at WorkerThread.");
            }
        }
        else {
            //TODO: do something when it catches any case.
        }
    }

    return 0;
}


class Network sealed {
 public:
     Network(void) {
     }

     ~Network(void) {
     }

     void Initialize(void) {
         if (!StartNetwork())
         {
             // log here.
             return;
         }

         // create I/O completion port
         const int numberOfThread = 4;   //TODO: remove magic number later.
         m_Iocp.StartIocpThread(numberOfThread);

         // create listen socket.
         m_ListenSocket.Bind(5150);
     }

     bool RunAcceptThread(void) {
         // accept connection 
         SOCKADDR_IN saRemote;
         int remoteLen = sizeof(saRemote);
         SOCKET Accepted = m_ListenSocket.Accept(&saRemote, &remoteLen);

         // 6. create socket handle data struct
         //PER_HANDLE_DATA *PerHandleData = (LPPER_HANDLE_DATA) ::GlobalAlloc(GPTR, sizeof(PER_HANDLE_DATA));

         LPPER_HANDLE_DATA perHandleData = MakeClientSession(Accepted, saRemote, remoteLen);
         m_Iocp.BindSocket((HANDLE)(Accepted), static_cast<DWORD>(Accepted));

         printf("[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
             inet_ntoa(saRemote.sin_addr), ntohs(saRemote.sin_port));
         printf("Socket number %d connected\n", Accepted);

         // start socket's i/o job
         // MEMO: 초기화 안된 Overlapped 쓰면 객체 뻑남.
         ZeroMemory(&(perHandleData->Overlapped), sizeof(perHandleData->Overlapped));

         if ( perHandleData->Socket.RecvAsync(
                &(perHandleData->Buffer), 
                &(perHandleData->Overlapped)
                ) != 0) {
             err_display("RecvAsync() Error at main thread");
         }
         return true;
     }

     LPPER_HANDLE_DATA MakeClientSession(SOCKET _clientSocket, const SOCKADDR_IN& _addr, const int _addrLen) {
         LPPER_HANDLE_DATA data = new PER_HANDLE_DATA();
         data->Socket.Initialize(_clientSocket, _addr, _addrLen);
         return data;
     }

     void Cleanup() {
         EndNetwork();
     }

 private:
     bool StartNetwork() {
         static WSADATA wsa;
         int result = ::WSAStartup(MAKEWORD(2, 2), &wsa);
         if (result == 0)
         {
             return true;
         }
         // TODO: log here. 
         return false;
     }

     void EndNetwork() {
         ::WSACleanup();
     }

 private:
     CIocp m_Iocp;
     TcpSocket m_ListenSocket;
};
