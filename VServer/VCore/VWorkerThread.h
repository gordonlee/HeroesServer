#include <WinSock2.h>
#include "VNetwork.h"
#include "VClientSession.h"
#include "RIOBase.h"
#include "VLogger.h"

namespace VCore
{
	__declspec(thread) int LIoThreadId = -1;

	VOID VNetwork::Worker(int threadIndex)
	{
		int LIoThreadId = threadIndex;

		RIORESULT results[MAX_RIO_RESULT];

		// TODO :  Thread ID 출력하자
		Logger<VNetwork>::Info("Start Thread.");

		while (true)
		{
			memset(results, 0, sizeof(results));

			ULONG numResults = RIOBase::GetInstance()->RIODequeueCompletion(LIoThreadId, results, MAX_RIO_RESULT);

			if (numResults == 0)
			{
				std::this_thread::yield();
				continue;
			}
			else if (numResults == RIO_CORRUPT_CQ)
			{
				Logger<VNetwork>::Fatal("RIO CORRUPT CQ.");
			}

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
						if (!RIOBase::GetInstance()->RIOReceive(context->clientSession_->requestQueue_, buf, 1, flags, context))
						{
							//printf_s("[DEBUG] RIOReceive error: %d\n", GetLastError());
							Logger<VNetwork>::Fatal("RIOReceive error.");

							delete context;
							return;
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

						//Logger<VNetwork>::Info(packet->Data + sizeof(IHeader)+12);
						if (!RIOBase::GetInstance()->RIOSend(context->clientSession_->requestQueue_, buf, 1, flags, context))
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
						//printf_s(".", SendConunt);
						context->bufferID_ = RIO_INVALID_BUFFERID;
						RIO_BUF* buf = context->clientSession_->GetRecieveBuffer();

						if (!RIOBase::GetInstance()->RIOReceive(context->clientSession_->requestQueue_, buf, 1, flags, context))
						{
							printf_s("[DEBUG] RIOReceive error: %d\n", GetLastError());

							delete context;
							return;
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
