#pragma once

#include "RandObject.h"

#include <deque>
#include <thread>
#include <unordered_map>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32.lib")

class Client
{
	static const int BUFFER_SIZE = 65536;
	static const int REPLY_SAMPLING_COUNT = 1000;

	int id_;
	SOCKET socket_;

	char recvBuffer_[BUFFER_SIZE];
	char sendBuffer_[BUFFER_SIZE];
	int recvOffset_ = 0;
	int sendLength_ = 0;

	UInt64 lastRecvTime_ = 0;
	UInt64 lastSendTime_ = 0;

	Random rand_;
	std::thread* workerThread_;

	pair<Int64, int> myPacketResponseTimes_;
	pair<Int64, int> otherPacketResponseTimes_;
	Int64 myPacketRecvCount_;
	Int64 otherPacketRecvCount_;
	Int64 totalSentCount_;
	Int64 diffPacketCount_;
	Int64 errorCount_;

	int packetId_;
	unordered_map<int, pair<char*, int>> memoryDataList_;

	bool isConnected_;
	bool toBeClose_;
	bool exit_;

public:
	Client(int id);
	~Client();

	bool Run();
	void Stop();
	void Join();

	bool IsConnected();

private:
	void Init();
	void Update();
	void Exit();

	void Connect();
	void Close();
	void TryClose();

	void Read();
	void Process();
	void ProcessPacket(char* buf, int len);
	void Write();
	void WritePacket(char* buf, int len);
	void Flush();

public:
	const Int64 GetMyPacketResponseTimeAvg();
	const Int64 GetOtherPacketResponseTimeAvg();
	inline const Int64 GetMyPacketRecvCount() const { return myPacketRecvCount_; }
	inline const Int64 GetOtherPacketRecvCount() const { return otherPacketRecvCount_; }
	inline const Int64 GetTotalSentCount() const { return totalSentCount_; }
	inline const Int64 GetDiffPacketCount() const { return diffPacketCount_; }
};

