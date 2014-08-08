
// StressClient.h : PROJECT_NAME 응용 프로그램에 대한 주 헤더 파일입니다.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'stdafx.h'를 포함합니다."
#endif

#include "resource.h"		// 주 기호입니다.

// CStressClientApp:
// 이 클래스의 구현에 대해서는 StressClient.cpp을 참조하십시오.
//

struct Config
{
	string serverIp_ = "127.0.0.1";
	Int16 serverPort_ = 9000;

	int clientCountMax_ = 1;
	int closeProbPerFrame_ = 0;
	int sendCountPerSecond_ = 1;
	int sendPacketSizeMin_ = 1;
	int sendPacketSizeMax_ = 4096;
	bool broadcast_ = false;
};

class Client;

struct Status
{
	int clientConnectedCount_;
	Int64 sendCountTotal_;
	Int64 recvMyPacketCount_;
	Int64 recvMyPacketResponseTimeAvg_;
	Int64 recvOtherPacketCount_;
	Int64 recvOtherPacketResponseTimeAvg_;
	Int64 recvDiffPacketCount_;
	int runningTime_;

	Status()
	{
		Init();
	}

	void Init()
	{
		clientConnectedCount_ = 0;
		sendCountTotal_ = 0;
		recvMyPacketCount_ = 0;
		recvMyPacketResponseTimeAvg_ = 0;
		recvOtherPacketCount_ = 0;
		recvOtherPacketResponseTimeAvg_ = 0;
		recvDiffPacketCount_ = 0;
	}
};

class CStressClientApp : public CWinApp
{
public:
	CStressClientApp();

// 재정의입니다.
public:
	virtual BOOL InitInstance();

// 구현입니다.

	DECLARE_MESSAGE_MAP()

private:
	vector<Client*> clients_;
	Config config_;
	Status status_;
	bool isRun_;

public:
	vector<Client*>& GetClients() { return clients_; }
	Config& GetConfig() { return config_; }
	Status& GetStatus() { return status_; }

	inline void SetRun(bool run) { isRun_ = run; }
	inline bool IsRun() const { return isRun_; }
};

extern CStressClientApp theApp;