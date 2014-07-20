#include <WinSock2.h>
#include "VNetwork.h"
#include "VClientSession.h"
#include "RIOBase.h"
#include "VLogger.h"

using namespace VCore;

namespace 
{
	__declspec(thread) int LIoThreadId = -1;

	unsigned int WINAPI IoWorkerThread(LPVOID lpParam)
	{
		int LIoThreadId = reinterpret_cast<int>(lpParam);

		// MAX_CQ_SIZE_PER_RIO_THREAD는 각각의 RQ길이 총 합보다 크거나 같아야한다.
		m_RioCompletionQueue[LIoThreadId] = m_RioFunctionTable.RIOCreateCompletionQueue(MAX_CQ_SIZE_PER_RIO_THREAD, 0);

		RIORESULT results[MAX_RIO_RESULT];

		// TODO :  Thread ID 출력하자
		Logger<VNetwork>::Info("Start Thread.");

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
				Logger<VNetwork>::Fatal("RIO CORRUPT CQ.");
			}

			//printf_s("[%d]Get result : %d\n", LIoThreadId, numResults);

			for (ULONG i = 0; i < numResults; ++i)
			{
				RioIoContext* context = reinterpret_cast<RioIoContext*>(results[i].RequestContext);
				ULONG transferred = results[i].BytesTransferred;

				//printf_s("[%d]Lenth : %d\n", LIoThreadId, transferred);
				if (transferred == 0 && results[i].Status != 0 && results[i].Status != 10054)
				{
					DWORD flags = 0;

					if (context->isMainContext_)
					{
						context->bufferID_ = RIO_INVALID_BUFFERID;
						RIO_BUF* buf = context->clientSession_->GetRecieveBuffer();
						if (!m_RioFunctionTable.RIOReceive(context->clientSession_->requestQueue_, buf, 1, flags, context))
						{
							//printf_s("[DEBUG] RIOReceive error: %d\n", GetLastError());
							Logger<VNetwork>::Fatal("RIOReceive error.");

							delete context;
							return -1;
						}
					}
					else
					{
						delete context;
					}
				}
				else if (transferred == 0)
				{
					LINGER lingerOption;
					lingerOption.l_onoff = 1;
					lingerOption.l_linger = 0;

					if (SOCKET_ERROR == setsockopt(context->clientSession_->socket_, SOL_SOCKET, SO_LINGER, (char*)&lingerOption, sizeof(LINGER)))
					{
						//printf_s("[DEBUG] setsockopt linger option error: %d\n", GetLastError());
						Logger<VNetwork>::Fatal("setsockopt linger option error.");
					}

					closesocket(context->clientSession_->socket_);
					delete context;

				}
				else if (IO_RECV == context->ioType_)
				{
					PacketVector& packetList = context->clientSession_->GetMessages(transferred);
					DWORD flags = 0;

					RIO_BUF* buf = NULL;
					context->ioType_ = IO_SEND;

					for each (IPacket* packet in packetList)
					{
						buf = context->clientSession_->AddSendBuffer(packet);

						if (buf == NULL)
						{
							Logger<VNetwork>::Fatal("buffer allocate fail!!!");
							continue;
						}
						context->bufferID_ = buf->BufferId;

						RecieveConunt++;

						//Logger<VNetwork>::Recieve(",");
						//Logger<VNetwork>::Info(packet->Data + sizeof(IHeader)+12);
						if (!m_RioFunctionTable.RIOSend(context->clientSession_->requestQueue_, buf, 1, flags, context))
						{
							Logger<VNetwork>::Fatal("RIOSend error.");
							//printf_s("[DEBUG] RIOSend error: %d\n", GetLastError());

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
					context->ioType_ = IO_RECV;

					if (context->isMainContext_)
					{

						SendConunt++;
						//printf_s(".", SendConunt);
						context->bufferID_ = RIO_INVALID_BUFFERID;
						RIO_BUF* buf = context->clientSession_->GetRecieveBuffer();

						//Logger<VNetwork>::Send("`");

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
