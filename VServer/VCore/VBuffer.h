#pragma once
#include <WinSock2.h>
#include <MSWSock.h>
#include <map>

namespace VCore
{

	class VBuffer
	{
	public:
		VBuffer()
		{

		}

		VBuffer(size_t bufferSize)
			: buffer_(new char(bufferSize))
			, remainLenth_(bufferSize)
			, maxSize_(bufferSize)
			, startOffset_(0)
			, sendBufferMap_()
		{
			memset(buffer_, 0x00, SESSION_BUFFER_SIZE);
		}

		~VBuffer()
		{
			delete buffer_;
		}

		//TODO: 나중에 rio base로 옮기자
		bool RegisterBuffer()
		{
			// RIO 버퍼 등록 & 클라이언트에 생성된 버퍼ID 저장
			bufferID_ = m_RioFunctionTable.RIORegisterBuffer(buffer_, SESSION_BUFFER_SIZE);

			if (bufferID_ == RIO_INVALID_BUFFERID)
			{
				printf_s("[DEBUG] RIORegisterBuffer Error: %d\n", GetLastError());
				return false;
			}

			return true;
		}

		bool ReleaseBuffer(RIO_BUFFERID id)
		{
			m_RioFunctionTable.RIODeregisterBuffer(id);
			delete sendBufferMap_[id];
			sendBufferMap_.erase(id);

			return true;
		}

		//Message 자체를 RIO Buffer로 등록하고 데이터를 전송한다.
		RIO_BUF* InsertData(char* message)
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

		RIO_BUF* MakeRioBuf(char* message)
		{
			size_t messageLenth = strlen(message);

			RIO_BUFFERID id = m_RioFunctionTable.RIORegisterBuffer(message, messageLenth);

			if ( id == RIO_INVALID_BUFFERID)
			{
				return NULL;
			}

			RIO_BUF* rioBuf = new RIO_BUF();
			rioBuf->BufferId = id;
			rioBuf->Length = messageLenth;
			rioBuf->Offset = 0;

			sendBufferMap_.insert(SendBufferMap::value_type(id, message));

			return rioBuf;
		}

		ULONG startOffset_;

	private:

		char* buffer_;
		RIO_BUFFERID bufferID_;
		ULONG remainLenth_;
		size_t maxSize_;

		typedef std::map<RIO_BUFFERID, char*> SendBufferMap;
		SendBufferMap sendBufferMap_;
	};
}