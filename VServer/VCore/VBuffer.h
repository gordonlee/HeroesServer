#pragma once
#include <WinSock2.h>
#include <MSWSock.h>
#include <map>

namespace VCore
{
	class IHeader
	{
	public:
		SHORT	Lenth;
		BYTE	Type;
		BYTE	CheckSum;
	};

	class IPacket
	{
	public:
		ULONG Lenth;
		char* Data;
	};

	class VBuffer
	{
	public:
		VBuffer()
			: remainLenth_(SESSION_BUFFER_SIZE)
			, maxSize_(SESSION_BUFFER_SIZE)
			, startOffset_(0)
			, sendBufferMap_()
		{
			memset(buffer_, 0x00, SESSION_BUFFER_SIZE);
		}

		~VBuffer()
		{
		}

		//TODO: 나중에 rio base로 옮기자
		bool RegisterBuffer()
		{
			// RIO 버퍼 등록 & 클라이언트에 생성된 버퍼ID 저장
			bufferID_ = RIOBase::GetInstance()->RIORegisterBuffer(buffer_, SESSION_BUFFER_SIZE);

			if (bufferID_ == RIO_INVALID_BUFFERID)
			{
				printf_s("[DEBUG] RIORegisterBuffer Error: %d\n", GetLastError());
				return false;
			}

			return true;
		}

		bool ReleaseBuffer(RIO_BUFFERID id)
		{
			RIOBase::GetInstance()->RIODeregisterBuffer(id);
			delete sendBufferMap_[id];
			sendBufferMap_.erase(id);

			return true;
		}

		//Message 자체를 RIO Buffer로 등록하고 데이터를 전송한다.
		RIO_BUF* InsertData(IPacket* message)
		{
			return MakeRioBuf(message);
		}

		char* GetBuffer()
		{
			return buffer_;
		}

		bool InitOffset(char* offset, int remainLenth)
		{
			memmove_s(buffer_, maxSize_, offset, remainLenth);
			startOffset_ = remainLenth;
			remainLenth_ = maxSize_ - remainLenth;

			memset(buffer_ + startOffset_, 0x00, remainLenth_);

			return true;
		}

		RIO_BUF* MakeRioBuf()
		{
			RIO_BUF* rioBuf = new RIO_BUF();
			rioBuf->BufferId = bufferID_;
			rioBuf->Length = remainLenth_;
			rioBuf->Offset = startOffset_;
			return rioBuf;
		}

		RIO_BUF* MakeRioBuf(IPacket* message)
		{
			RIO_BUFFERID id = RIOBase::GetInstance()->RIORegisterBuffer(message->Data, message->Lenth);

			if ( id == RIO_INVALID_BUFFERID)
			{
				return NULL;
			}

			RIO_BUF* rioBuf = new RIO_BUF();
			rioBuf->BufferId = id;
			rioBuf->Length = message->Lenth;
			rioBuf->Offset = 0;

			sendBufferMap_.insert(SendBufferMap::value_type(id, message->Data));

			return rioBuf;
		}

		ULONG startOffset_;

	private:

		char buffer_[SESSION_BUFFER_SIZE];
		RIO_BUFFERID bufferID_;
		ULONG remainLenth_;
		size_t maxSize_;

		typedef std::map<RIO_BUFFERID, char*> SendBufferMap;
		SendBufferMap sendBufferMap_;
	};
}