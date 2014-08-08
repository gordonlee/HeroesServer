#include "stdafx.h"

#include "Client.h"
#include "Log.h"
#include "Packet.h"
#include "RandObject.h"
#include "StressClient.h"
#include "TimeObject.h"

namespace
{
	string sampleData = "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz";
}

Client::Client(int id) : id_(id), socket_(INVALID_SOCKET), workerThread_(nullptr), exit_(false)
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
}

Client::~Client()
{
	WSACleanup();
}

bool Client::Run()
{
	try
	{
		workerThread_ = new std::thread([this]()
		{
			while (!exit_)
			{
				UInt64 start = Time::Now();

				if (theApp.IsRun())
				{
					Update();
				}

				UInt64 end = Time::Now();
				UInt64 during = (end - start);

				if (during < 50)
				{
					UInt64 diff = 50 - during;
					this_thread::sleep_for(Time::To(diff));
				}
			}
		});

		LOG("Client::Run - Ŭ���̾�Ʈ[%d] ����\n", id_);

		return true;
	}
	catch (exception& e)
	{
		LOG("%s\n", e.what());
	}

	LOG("Client::Run - Ŭ���̾�Ʈ[%d] ���� ����\n", id_);

	return false;
}

void Client::Stop()
{
	Exit();
}

void Client::Join()
{
	if (workerThread_ != nullptr)
	{
		if (workerThread_->joinable())
		{
			workerThread_->join();
		}

		delete workerThread_;
		workerThread_ = nullptr;
	}

	LOG("Client::Stop - Ŭ���̾�Ʈ[%d] ����\n", id_);
}

void Client::Init()
{
	ZeroMemory(recvBuffer_, BUFFER_SIZE);
	ZeroMemory(sendBuffer_, BUFFER_SIZE);
	recvOffset_ = 0;
	sendLength_ = 0;
	lastSendTime_ = 0;
	myPacketResponseTimes_ = pair<Int64, int>(0, 0);
	otherPacketResponseTimes_ = pair<Int64, int>(0, 0);
	myPacketRecvCount_ = 0;
	otherPacketRecvCount_ = 0;
	totalSentCount_ = 0;
	diffPacketCount_ = 0;
	errorCount_ = 0;
	packetId_ = 0;
	memoryDataList_.clear();
	isConnected_ = false;
	toBeClose_ = false;
}

void Client::Update()
{
	if (socket_ == INVALID_SOCKET)
	{
		Init();
		Connect();
	}
	else
	{
		TryClose();
		Read();
		Process();
		Write();
		Flush();
	}
}

void Client::Exit()
{
	exit_ = true;
}

bool Client::IsConnected()
{
	if (socket_ == INVALID_SOCKET)
	{
		return false;
	}

	return isConnected_;
}

void Client::Connect()
{
	if (socket_ != INVALID_SOCKET)
	{
		return;
	}

	socket_ = socket(PF_INET, SOCK_STREAM, 0);
	if (socket_ == SOCKET_ERROR)
	{
		LOG("%s - Ŭ���̾�Ʈ[%d] socket ���� ����\n", __FUNCTION__, id_);
		Close();
		return;
	}

	SOCKADDR_IN servAddr;
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr(theApp.GetConfig().serverIp_.c_str());
	servAddr.sin_port = htons(theApp.GetConfig().serverPort_); 

	if (connect(socket_, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
	{
		socket_ = INVALID_SOCKET;
		LOG("%s - Ŭ���̾�Ʈ[%d] connect ����[%d]\n", __FUNCTION__, id_, WSAGetLastError());
		return;
	}

	LOG("%s - Ŭ���̾�Ʈ[%d] connect ����\n", __FUNCTION__, id_);

	isConnected_ = true;
	lastRecvTime_ = Time::Now();

	// set buf size
	int bufsize = BUFFER_SIZE;
	setsockopt(socket_, SOL_SOCKET, SO_SNDBUF, (char*)&bufsize, sizeof(bufsize));
	setsockopt(socket_, SOL_SOCKET, SO_RCVBUF, (char*)&bufsize, sizeof(bufsize));

	// set nonblocking
	u_long arg = 1;
	ioctlsocket(socket_, FIONBIO, &arg);
}

void Client::Close()
{
	if (!IsConnected())
	{
		return;
	}

	closesocket(socket_);
	socket_ = INVALID_SOCKET;
	isConnected_ = false;

	LOG("%s - Ŭ���̾�Ʈ[%d] closesocket\n", __FUNCTION__, id_);
}

void Client::TryClose()
{
	// ����Ǿ�� �Ѵ�
	if (toBeClose_ && memoryDataList_.size() <= 0)
	{
		LOG("%s - Ŭ���̾�Ʈ[%d] close by prob\n", __FUNCTION__, id_);
		Close();
	}

	if (theApp.GetConfig().closeProbPerFrame_ <= 0)
	{
		return;
	}

	int prob = min(theApp.GetConfig().closeProbPerFrame_, 100);
	if (rand_.NextInt(((int)(10000 / prob)) - 1) == 0)
	{
		if (memoryDataList_.size() <= 0)
		{
			LOG("%s - Ŭ���̾�Ʈ[%d] close by prob immediately\n", __FUNCTION__, id_);
			Close();
		}
		else
		{
			toBeClose_ = true;
		}
	}
}

void Client::Read()
{
	if (socket_ == INVALID_SOCKET)
	{
		return;
	}

	UInt64 now = Time::Now();
	UInt64 diff = now - lastRecvTime_;

	// recv timeout
	if (diff >= 60000)
	{
		LOG("%s - Ŭ���̾�Ʈ[%d] recv timeout\n", __FUNCTION__, id_);
		Close();
		return;
	}

	int readBytes = recv(socket_, recvBuffer_ + recvOffset_, BUFFER_SIZE - recvOffset_, 0);

	if (readBytes <= 0)
	{
		// nonblock �ɼǿ����� WOULDBLOCK�� ������ üũ���� �ʴ´�
		if (readBytes < 0 && WSAGetLastError() == WSAEWOULDBLOCK)
		{
			return;
		}

		LOG("%s - Ŭ���̾�Ʈ[%d] recv ����[%d]\n", __FUNCTION__, id_, WSAGetLastError());
		Close();
		return;
	}

	recvOffset_ += readBytes;
	lastRecvTime_ = now;
}

void Client::Process()
{
	if (socket_ == INVALID_SOCKET)
	{
		return;
	}

	int headerSize = sizeof(PacketHeader);

	char* recvBuf = recvBuffer_;
	int remain = recvOffset_;
	int offset = 0;

	while (true)
	{
		if (remain < headerSize)
		{
			break;
		}

		Packet* packet = (Packet*)(recvBuf + offset);
		if (!packet)
		{
			LOG("%s - Ŭ���̾�Ʈ[%d] packet is nullptr\n", __FUNCTION__, id_);
			Close();
			return;
		}

		if (packet->dataSize_ <= 0)
		{
			LOG("%s - Ŭ���̾�Ʈ[%d] packet->dataSize[%d] is less than zero\n", __FUNCTION__, id_, packet->dataSize_);
			Close();
			return;
		}

		if (packet->dataSize_ > 65532)
		{
			LOG("%s - Ŭ���̾�Ʈ[%d] packet->dataSize[%d] is more than 65532\n", __FUNCTION__, id_, packet->dataSize_);
			Close();
			return;
		}

		if (packet->checkSum_ != 0x55)
		{
			LOG("%s - Ŭ���̾�Ʈ[%d] checksum[%d] failed\n", __FUNCTION__, id_, packet->checkSum_);
			Close();
			return;
		}

		int totalSize = headerSize + (int)packet->dataSize_;
		if (remain < totalSize)
		{
			break;
		}

		ProcessPacket(packet->data_, packet->dataSize_);

		remain -= totalSize;
		offset += totalSize;
	}

	if (remain > 0)
	{
		memmove(recvBuffer_, recvBuffer_ + offset, remain);
	}

	recvOffset_ = remain;
}

void Client::ProcessPacket(char* buf, int len)
{
	UInt64 now = Time::Now();

	PacketData* data = (PacketData*)buf;
	if (!data)
	{
		LOG("%s - Ŭ���̾�Ʈ[%d] data is nullptr\n", __FUNCTION__, id_);
		Close();
		return;
	}

	// ����ð�
	int responseTime = (int)(now - data->sentTime_);
	if (data->sentTime_ <= 0)
	{
		LOG("%s - Ŭ���̾�Ʈ[%d] sentTime_ <= 0\n", __FUNCTION__, id_);
		Close();
		return;
	}

	if (responseTime < 0)
	{
		LOG("%s - Ŭ���̾�Ʈ[%d] responseTime < 0\n", __FUNCTION__, id_);
		Close();
		return;
	}

	// ���� ���� �޽���
	if (data->clientId_ == id_)
	{
		if (memoryDataList_.count(data->packetId_) <= 0)
		{
			LOG("%s - Ŭ���̾�Ʈ[%d] packetId[%d] is not in memoryDataList\n", __FUNCTION__, id_, data->packetId_);
			Close();
			return;
		}

		// ����� ���� ������ �κ� ��������
		auto pair = memoryDataList_[data->packetId_];
		if (pair.first == nullptr)
		{
			LOG("%s - Ŭ���̾�Ʈ[%d] pair.first is nullptr\n", __FUNCTION__, id_);
			Close();
			return;
		}

		// �۽� ���� ������ �κа� ������ ��
		if (pair.second != len || memcmp(pair.first, (char*)data, pair.second) != 0)
		{
			diffPacketCount_++;
		}

		// ����س��� �κ� ����
		delete pair.first;
		memoryDataList_.erase(data->packetId_);

		myPacketRecvCount_++;
		myPacketResponseTimes_.first += responseTime;
		myPacketResponseTimes_.second++;
	}
	// ���� ���� �޽���
	else
	{
		otherPacketRecvCount_++;
		otherPacketResponseTimes_.first += responseTime;
		otherPacketResponseTimes_.second++;
	}
}

void Client::Write()
{
	if (socket_ == INVALID_SOCKET)
	{
		return;
	}

	if (toBeClose_)
	{
		return;
	}

	if (theApp.GetConfig().sendCountPerSecond_ <= 0)
	{
		return;
	}

	int sendInterval = 1000 / theApp.GetConfig().sendCountPerSecond_;

	UInt64 now = Time::Now();
	UInt64 diff = now - lastSendTime_;
	if (diff < (UInt64)sendInterval)
	{
		return;
	}

	// ��Ŷ ����
	Packet packet;

	// ��Ŷ ������
	PacketData* data = (PacketData*)packet.data_;
	data->packetId_ = packetId_++;
	data->clientId_ = id_;
	data->sentTime_ = now;

	// sizeof(PacketHeader) + sizeof(int) + sizeof(int) + sizeof(Int64)�� ������ ������ ������ �κ� ä���ֱ�
	const string& sample = sampleData.substr(0, rand_.NextInt(theApp.GetConfig().sendPacketSizeMax_ - 21) + 1);
	memcpy(data->data_, sample.c_str(), (int)sample.size());

	// ������ ������
	int dataSize = sizeof(data->packetId_) + sizeof(data->clientId_) + sizeof(data->sentTime_) + (int)sample.size();

	// ��Ŷ ���
	packet.dataSize_ = dataSize;
	packet.flag_ = theApp.GetConfig().broadcast_ ? 0x02 : 0x01;
	packet.checkSum_ = 0x55;

	// ��ü ��Ŷ ����
	int totalSize = sizeof(PacketHeader) + packet.dataSize_;

	// SendBuffer�� ���
	WritePacket((char*)&packet, totalSize);
	lastSendTime_ = now;

	// ������ �κ� ���� ���
	char* memoryData = new char[dataSize];
	memcpy(memoryData, (char*)data, dataSize);

	if (memoryDataList_.count(data->packetId_) <= 0)
	{
		memoryDataList_[data->packetId_] = make_pair(memoryData, dataSize);
	}
	else
	{
		LOG("%s - Ŭ���̾�Ʈ[%d] packetId[%d] is already in memoryDataList\n", __FUNCTION__, id_, data->packetId_);
		Close();
		return;
	}
}

void Client::WritePacket(char* buf, int len)
{
	if (!buf)
	{
		LOG("%s - Ŭ���̾�Ʈ[%d] buf is nullptr\n", __FUNCTION__, id_);
		Close();
		return;
	}

	if (len <= 0)
	{
		LOG("%s - Ŭ���̾�Ʈ[%d] len <= 0\n", __FUNCTION__, id_);
		Close();
		return;
	}

	if (sendLength_ + len > BUFFER_SIZE)
	{
		LOG("%s - Ŭ���̾�Ʈ[%d] sendbuffer overflow\n", __FUNCTION__, id_);
		Close();
		return;
	}

	if (len > theApp.GetConfig().sendPacketSizeMax_)
	{
		LOG("%s - Ŭ���̾�Ʈ[%d] len[%d] is over maximum[%d]\n", __FUNCTION__, id_, len, theApp.GetConfig().sendPacketSizeMax_);
		Close();
		return;
	}

	memcpy(sendBuffer_ + sendLength_, buf, len);
	sendLength_ += len;
}

void Client::Flush()
{
	if (socket_ == INVALID_SOCKET)
	{
		return;
	}

	if (toBeClose_)
	{
		return;
	}

	if (sendLength_ <= 0)
	{
		return;
	}

	int sendBytes = send(socket_, sendBuffer_, sendLength_, 0);

	if (sendBytes <= 0)
	{
		// nonblock �ɼǿ����� WOULDBLOCK�� ������ üũ���� �ʴ´�
		if (sendBytes < 0 && WSAGetLastError() == WSAEWOULDBLOCK)
		{
			return;
		}

		LOG("%s - Ŭ���̾�Ʈ[%d] send ����[%d]\n", __FUNCTION__, id_, WSAGetLastError());
		Close();
		return;
	}

	if (sendBytes < sendLength_)
	{
		memmove(sendBuffer_, sendBuffer_ + sendBytes, sendLength_ - sendBytes);
	}

	sendLength_ -= sendBytes;
	totalSentCount_++;
}

namespace
{
	Int64 GetResponseTimeAvg(const pair<Int64, int>& responseTimes)
	{
		auto localCopy = responseTimes;
		Int64 avg = localCopy.second > 0 ? (Int64)(localCopy.first / (Int64)localCopy.second) : 0;
		return avg;
	}
}

const Int64 Client::GetMyPacketResponseTimeAvg()
{
	return GetResponseTimeAvg(myPacketResponseTimes_);
}

const Int64 Client::GetOtherPacketResponseTimeAvg()
{
	return GetResponseTimeAvg(otherPacketResponseTimes_);
}