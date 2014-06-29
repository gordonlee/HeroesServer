#include <WinSock2.h>
#include "VNetwork.h"
#include "VClientSession.h"
#include "RIOBase.h"

using namespace VCore;

namespace 
{
	__declspec(thread) int LIoThreadId = -1;

	unsigned int WINAPI IoWorkerThread(LPVOID lpParam)
	{
		int LIoThreadId = reinterpret_cast<int>(lpParam);

		m_RioCompletionQueue[LIoThreadId] = m_RioFunctionTable.RIOCreateCompletionQueue(MAX_CQ_SIZE_PER_RIO_THREAD, 0);

		RIORESULT results[MAX_RIO_RESULT];

		printf_s("Start Thread [%d]\n", LIoThreadId);

		while (true)
		{
			memset(results, 0, sizeof(results));

			ULONG numResults = m_RioFunctionTable.RIODequeueCompletion(m_RioCompletionQueue[LIoThreadId], results, MAX_RIO_RESULT);

			if (numResults == 0)
			{
				Sleep(1);
				continue;
			}
			else if (numResults == RIO_CORRUPT_CQ)
			{
				printf_s("[DEBUG] RIO CORRUPT CQ \n");
			}

			printf_s("[%d]Get result : %d", LIoThreadId, numResults);

			for (ULONG i = 0; i < numResults; ++i)
			{
				RioIoContext* context = reinterpret_cast<RioIoContext*>(results[i].RequestContext);
				ULONG transferred = results[i].BytesTransferred;

				if (transferred == 0)
				{
					LINGER lingerOption;
					lingerOption.l_onoff = 1;
					lingerOption.l_linger = 0;

					if (SOCKET_ERROR == setsockopt(context->mClientSession->GetClientSocket(), SOL_SOCKET, SO_LINGER, (char*)&lingerOption, sizeof(LINGER)))
					{
						printf_s("[DEBUG] setsockopt linger option error: %d\n", GetLastError());
					}

					closesocket(context->mClientSession->GetClientSocket());
					delete context->mClientSession->GetBuffer();
					delete context;

				}
				else if (IO_RECV == context->mIoType)
				{
					printf_s("%s\n", context->mClientSession->GetBuffer());

					context->mIoType = IO_SEND;
					context->Length = strnlen_s(context->mClientSession->GetBuffer(), SESSION_BUFFER_SIZE);
					DWORD flags = 0;

					/// start async send
					if (!m_RioFunctionTable.RIOSend(context->mClientSession->GetRequestQueue(), (PRIO_BUF)context, 1, flags, context))
					{
						printf_s("[DEBUG] RIOSend error: %d\n", GetLastError());

						delete context->mClientSession->GetBuffer();
						delete context;
						return false;
					}

				}
				else if (IO_SEND == context->mIoType)
				{
					context->mIoType = IO_RECV;
					context->Offset = 0;

					DWORD recvbytes = 0;
					DWORD flags = 0;

					/// start async recv
					if (!m_RioFunctionTable.RIOReceive(context->mClientSession->GetRequestQueue(), (PRIO_BUF)context, 1, flags, context))
					{
						printf_s("[DEBUG] RIOReceive error: %d\n", GetLastError());

						delete context->mClientSession->GetBuffer();
						delete context;
						return -1;
					}
				}

			}
		}
	}
}
