#pragma once
#include <map>
#include <WinSock2.h>
#include <MSWSock.h>
#include "VBuffer.h"

namespace VCore
{
	class IHeader
	{
	public:
		SHORT	Lenth;
		BYTE	Type;
		BYTE	CheckSum;
	};

	typedef std::vector<char*> PacketVector;

	class VBufferController
	{
	private:
		PacketVector packetVector_;
		VBuffer buffer_;

	public:
		VBufferController()
		{
		}

		VBufferController(size_t bufferSize)
			:packetVector_(), buffer_(bufferSize)
		{
		}

		RIO_BUF* AddSendBuffer(char* message)
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

				if ((ULONG)header.Lenth > recieveLenth-4)
				{
					return packetVector_;
				}

				char* message = new char(header.Lenth + 1);

				memset(message, 0x00, header.Lenth);
				memcpy_s(message, header.Lenth, buffer, header.Lenth);

				packetVector_.push_back(message);

				int processLenth = sizeof(IHeader)+header.Lenth;

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