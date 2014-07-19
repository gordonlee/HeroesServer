#pragma once
#include <map>
#include <WinSock2.h>
#include <MSWSock.h>
#include "VBuffer.h"

namespace VCore
{

	typedef std::vector<IPacket*> PacketVector;

	class VBufferController
	{
	private:
		PacketVector packetVector_;
		VBuffer buffer_;

	public:
		VBufferController()
			:packetVector_(), buffer_()
		{
			buffer_.RegisterBuffer();
		}

		RIO_BUF* AddSendBuffer(IPacket* message)
		{
			return buffer_.InsertData(message);
		}

		bool ReleaseBuffer(RIO_BUFFERID id)
		{
			return buffer_.ReleaseBuffer(id);
		}

		const PacketVector& GetMessages(ULONG recieveLenth)
		{
			packetVector_.clear();

			char*buffer = buffer_.GetBuffer();
			
			IHeader header = IHeader();
			ULONG totalLenth = recieveLenth + buffer_.startOffset_;

			while (totalLenth > 0)
			{
				memcpy_s(&header, sizeof(IHeader), buffer, sizeof(IHeader));

				if ((ULONG)header.Lenth > recieveLenth - sizeof(IHeader))
				{
					return packetVector_;
				}

				IPacket* message = new IPacket();
				ULONG messageLenth = header.Lenth + sizeof(IHeader);

				message->Data = new char[messageLenth];
				message->Lenth = messageLenth;

				memset(message->Data, 0x00, message->Lenth);
				memcpy_s(message->Data, messageLenth, buffer, messageLenth);

				packetVector_.push_back(message);

				int processLenth = messageLenth;

				buffer += processLenth;
				totalLenth -= processLenth;
			}

			buffer_.InitOffset(buffer, totalLenth);

			return packetVector_;
		}

		RIO_BUF* GetRecieveBuffer()
		{
			return buffer_.MakeRioBuf();
		}



	};

}