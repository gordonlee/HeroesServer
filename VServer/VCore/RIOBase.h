#pragma once
#include "VLogger.h"

#pragma comment(lib, "ws2_32.lib")

namespace VCore
{
	enum RioConfig
	{
		SESSION_BUFFER_SIZE = 65536,

		MAX_RIO_THREAD = 4,
		MAX_RIO_RESULT = 256,
		MAX_SEND_RQ_SIZE_PER_SOCKET = 32,
		MAX_RECV_RQ_SIZE_PER_SOCKET = 32,
		MAX_CLIENT_PER_RIO_THREAD = 250,
		MAX_CQ_SIZE_PER_RIO_THREAD = (MAX_SEND_RQ_SIZE_PER_SOCKET + MAX_RECV_RQ_SIZE_PER_SOCKET) * MAX_CLIENT_PER_RIO_THREAD,
		MAX_SESSION = 1000,

	};

	enum IOType
	{
		IO_NONE,
		IO_SEND,
		IO_RECV,
	};

	class RIOBase
	{
	public:
		static RIOBase* GetInstance()
		{
			if (instance_ == NULL)
			{
				instance_ = new RIOBase();
			}

			return instance_;
		}

		INT Initialize(const SOCKET listenSocket)
		{
			CQ_Index_ = 0;

			GUID functionTableId = WSAID_MULTIPLE_RIO;
			DWORD dwBytes = 0;

			return WSAIoctl(
				listenSocket,
				SIO_GET_MULTIPLE_EXTENSION_FUNCTION_POINTER,
				&functionTableId,
				sizeof(GUID),
				(void**)&rioFunctionTable_,
				sizeof(rioFunctionTable_),
				&dwBytes,
				NULL,
				NULL
				);
		}

		BOOL MakeCompletionQueue(int maxCQ)
		{
			if (maxCQ <= 0)
			{
				return FALSE;
			}

			while (--maxCQ >= 0)
			{
				rioCompletionQueue_[maxCQ]
					= rioFunctionTable_.RIOCreateCompletionQueue(MAX_CQ_SIZE_PER_RIO_THREAD, 0);

				if (rioCompletionQueue_[maxCQ] == RIO_INVALID_CQ)
				{
					return FALSE;
				}
			}
			return TRUE;
		}

		RIO_RQ& GetRequestQueue(SOCKET socket)
		{
			RIO_RQ requestQueue_ =
				rioFunctionTable_.RIOCreateRequestQueue(
				socket,
				MAX_RECV_RQ_SIZE_PER_SOCKET,
				1,
				MAX_SEND_RQ_SIZE_PER_SOCKET,
				1,
				rioCompletionQueue_[CQ_Index_],
				rioCompletionQueue_[CQ_Index_],
				NULL);

			CQ_Index_ = (CQ_Index_ + 1) % MAX_RIO_THREAD;

			return requestQueue_;
		}

		BOOL RIOReceive(RIO_RQ SocketQueue, PRIO_BUF pData, ULONG DataBufferCount, DWORD Flags, PVOID RequestContext)
		{
			Logger<RIOBase>::Recieve("@");
			return rioFunctionTable_.RIOReceive(SocketQueue, pData, DataBufferCount, Flags, RequestContext);
		}

		ULONG RIODequeueCompletion(UINT ThreadIndex, PRIORESULT Array, ULONG ArraySize)
		{
			return rioFunctionTable_.RIODequeueCompletion(rioCompletionQueue_[ThreadIndex], Array, ArraySize);
		}

		BOOL RIOSend(RIO_RQ SocketQueue, PRIO_BUF pData, ULONG DataBufferCount, DWORD Flags, PVOID RequestContext)
		{
			Logger<RIOBase>::Send("@");
			return rioFunctionTable_.RIOSend(SocketQueue, pData, DataBufferCount, Flags, RequestContext);
		}

		RIO_BUFFERID RIORegisterBuffer(PCHAR DataBuffer, DWORD DataLength)
		{
			return rioFunctionTable_.RIORegisterBuffer(DataBuffer, DataLength);
		}

		VOID RIODeregisterBuffer(RIO_BUFFERID BufferId)
		{
			rioFunctionTable_.RIODeregisterBuffer(BufferId);
		}
		

	private:
		RIOBase(){}

		static RIOBase* instance_;

		RIO_EXTENSION_FUNCTION_TABLE rioFunctionTable_;
		RIO_CQ rioCompletionQueue_[MAX_RIO_THREAD];

		int CQ_Index_;
	};

	RIOBase* RIOBase::instance_;
}