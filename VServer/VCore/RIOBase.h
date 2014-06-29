#pragma once

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
		MAX_CLIENT_PER_RIO_THREAD = 2,
		MAX_CQ_SIZE_PER_RIO_THREAD = (MAX_SEND_RQ_SIZE_PER_SOCKET + MAX_RECV_RQ_SIZE_PER_SOCKET) * MAX_CLIENT_PER_RIO_THREAD,

	};

	enum IOType
	{
		IO_NONE,
		IO_SEND,
		IO_RECV,
	};

	enum ErrorMessage
	{
		Success = -1,
		Unknowm = 0,
		WSAInitializeFail,
		MakeListenSocketFail,
		BindListenSocketFail,
		AllocFunctionTableFail,
		ListenSocketFail,
	};

	enum ListenSocketState
	{
		DISABLE = 0,
		READY,
		LISTENING,
		PAUSE,
	};

	RIO_EXTENSION_FUNCTION_TABLE m_RioFunctionTable;
	RIO_CQ m_RioCompletionQueue[MAX_RIO_THREAD];

}