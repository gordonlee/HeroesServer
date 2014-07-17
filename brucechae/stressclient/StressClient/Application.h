#pragma once

#include <concurrent_vector.h>
#include <functional>
#include <string>
#include <unordered_map>

using namespace std;

class Client;

struct Config
{
	string serverIp_ = "127.0.0.1";
	Int16 serverPort_ = 9000;

	int clientCountMax_ = 1;
	int closeProbPerFrame_ = 0;
	int sendCountPerSecond_ = 1;
	int sendPacketSizeMin_ = 1;
	int sendPacketSizeMax_ = 1024;
	bool broadcast_ = false;
};

class Application
{
	unordered_map<string, function<void()>> functionTabel_;
	concurrency::concurrent_vector<Client*> clients_;
	Config config_;
	bool isRun_;

public:
	Application();
	~Application();

	static Application& GetInstance();

	void Run();

	inline const bool IsRun() const { return isRun_; }
	inline const Config& GetConfig() const { return config_; }

private:
	void Init();
	void Update();
	void Exit();

	void ChangeServerAddr();
	void AddNewClient();
	void ChangeValue(const string& name, int& value);
	void ChangeCloseProbPerFrame();
	void ChangeSendCountPerSec();
	void ChangeSendPacketSize();
	void ChangeBroadcastFlag();

	void Worker(int index);
};