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

					if (SOCKET_ERROR == setsockopt(context->clientSession_->socket_, SOL_SOCKET, SO_LINGER, (char*)&lingerOption, sizeof(LINGER)))
					{
						printf_s("[DEBUG] setsockopt linger option error: %d\n", GetLastError());
					}

					closesocket(context->clientSession_->socket_);
					delete context;

				}
				else if (IO_RECV == context->ioType_)
				{
					

					PacketVector packetList = context->clientSession_->GetMessages(transferred);
					DWORD flags = 0;

					RIO_BUF* buf = NULL;
					context->ioType_ = IO_SEND;

					for each (char* packet in packetList)
					{
						printf_s("%s\n", packet);

						buf = context->clientSession_->AddSendBuffer(packet);
						context->bufferID_ = buf->BufferId;

						if (!m_RioFunctionTable.RIOSend(context->clientSession_->requestQueue_, buf, 1, flags, context))
						{
							printf_s("[DEBUG] RIOSend error: %d\n", GetLastError());

							delete context;
							continue;
						}

						context = new RioIoContext(context->clientSession_, IO_SEND);
					}
				}
				else if (IO_SEND == context->ioType_)
				{
					DWORD flags = 0;

					context->clientSession_->ReleaseBuffer(context->bufferID_);

					if (context->isMainContext_)
					{
						context->bufferID_ = RIO_INVALID_BUFFERID;
						RIO_BUF* buf = context->clientSession_->GetRecieveBuffer();
						if (!m_RioFunctionTable.RIOReceive(context->clientSession_->requestQueue_, buf, 1, flags, context))
						{
							printf_s("[DEBUG] RIOReceive error: %d\n", GetLastError());

							delete context;
							return -1;
						}
					}
					else
					{
						delete context;
					}

				}

			}
		}
	}
}
