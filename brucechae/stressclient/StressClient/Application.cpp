#include "Application.h"
#include "Client.h"
#include "Log.h"
#include "TimeObject.h"

#include <algorithm>
#include <conio.h>
#include <iostream>
#include <sstream>

using namespace std;

Application::Application() : isRun_(false)
{
}

Application::~Application()
{
}

Application& Application::GetInstance()
{
	static Application instance;
	return instance;
}

void Application::Run()
{
	Init();

	while (isRun_)
	{
		Update();
	}

	Exit();
}

void Application::Init()
{
	functionTabel_["1"] = [this](){ ChangeServerAddr(); };
	functionTabel_["2"] = [this](){ AddNewClient(); };
	functionTabel_["3"] = [this](){ ChangeCloseProbPerFrame(); };
	functionTabel_["4"] = [this](){ ChangeSendCountPerSec(); };
	functionTabel_["5"] = [this](){ ChangeSendPacketSize(); };
	functionTabel_["6"] = [this](){ ChangeBroadcastFlag(); };
	functionTabel_["Q"] = functionTabel_["q"] = [this](){ isRun_ = false; };

	isRun_ = true;
}

void Application::Update()
{
	stringstream strm;
	strm << endl;
	strm << "1. ���� IP Port ����" << endl;
	strm << "2. Ŭ���̾�Ʈ �߰�" << endl;
	strm << "3. ���� ���� Ȯ�� ����" << endl;
	strm << "4. �ʴ� �۽� Ƚ�� ����" << endl;
	strm << "5. �۽� ��Ŷ ������ ����" << endl;
	strm << "6. Broadcast ���� ����" << endl;
	strm << "Q. ����" << endl;
	strm << ">> ";
	LOG(strm.str().c_str());

	string input;
	cin >> input;

	if (!cin.fail())
	{
		if (functionTabel_.count(input) > 0)
		{
			functionTabel_[input]();
		}
	}
}

void Application::Exit()
{
	isRun_ = false;

	Int64 totalClientCount = (Int64)clients_.size();
	Int64 myPacketResponseAvg_Sum = 0;
	Int64 otherPacketResponseAvg_Sum = 0;
	Int64 myPacketRecvCount_Sum = 0;
	Int64 otherPacketRecvCount_Sum = 0;
	Int64 totalSentCount_Sum = 0;
	Int64 diffPacketCount_Sum = 0;

	for (auto& client : clients_)
	{
		client->Join();

		myPacketResponseAvg_Sum += client->GetMyPacketResponseTimeAvg();
		otherPacketResponseAvg_Sum += client->GetOtherPacketResponseTimeAvg();
		myPacketRecvCount_Sum += client->GetMyPacketRecvCount();
		otherPacketRecvCount_Sum += client->GetOtherPacketRecvCount();
		totalSentCount_Sum += client->GetTotalSentCount();
		diffPacketCount_Sum += client->GetDiffPacketCount();

		delete client;
		client = nullptr;
	}

	if (totalClientCount > 0)
	{
		stringstream strm;
		strm << endl;
		strm << "--------------------------------- �׽�Ʈ ��� ---------------------------------" << endl;
		strm << "�׽�Ʈ Ŭ���̾�Ʈ ��: " << totalClientCount << endl;
		strm << "���� ���� ��Ŷ ���� Ƚ��: " << myPacketRecvCount_Sum << endl;
		strm << "���� ���� ��Ŷ ���� Ƚ��: " << otherPacketRecvCount_Sum << endl;
		strm << "���� �� ��Ŷ �۽� Ƚ��: " << totalSentCount_Sum << endl;
		strm << "���� ���� ��Ŷ�� ��� ����ð�: " << (myPacketResponseAvg_Sum / totalClientCount) << "ms" << endl;
		strm << "���� ���� ��Ŷ�� ��� ����ð�: " << (otherPacketResponseAvg_Sum / totalClientCount) << "ms" << endl;
		strm << "��Ŷ ������ ����ġ Ƚ��: " << diffPacketCount_Sum << endl;
		strm << "--------------------------------------------------------------------------------" << endl;
		strm << endl;
		LOG(strm.str().c_str());
	}
}

void Application::ChangeServerAddr()
{
	stringstream strm;
	strm << endl;
	strm << "IP Port�� �Է����ּ���. (����: " << config_.serverIp_ << ":" << config_.serverPort_ << ")" << endl;
	strm << "B. �ڷ� ����" << endl;
	strm << ">> ";
	LOG(strm.str().c_str());

	fflush(stdin);
	string input;
	getline(cin, input);

	if (!cin.fail())
	{
		size_t index = input.find_first_of(" ");
		if (index != input.npos)
		{
			string ip = input.substr(0, index);
			string port = input.substr(index + 1, input.size() - index - 1);

			if (all_of(port.begin(), port.end(), ::isdigit))
			{
				config_.serverIp_ = ip;
				config_.serverPort_ = (Int16)stoi(port);
				LOG("IP Port ���� ���� %s:%d\n", config_.serverIp_.c_str(), config_.serverPort_);
				return;
			}
		}
		else
		{
			LOG("<IP> <Port> �������� �ٽ� �Է����ּ���\n");
		}
	}

	LOG("���� ����\n");
}

void Application::AddNewClient()
{
	stringstream strm;
	strm << endl;
	strm << "�߰��� Ŭ���̾�Ʈ ���� �Է����ּ���." << endl;
	strm << "B. �ڷ� ����" << endl;
	strm << ">> ";
	LOG(strm.str().c_str());

	fflush(stdin);
	string input;
	cin >> input;

	if (!cin.fail())
	{
		if (input == "B" || input == "b")
		{
			return;
		}
		else if (all_of(input.begin(), input.end(), ::isdigit))
		{			
			int count = 0;
			for (int i = 0; i < stoi(input); i++)
			{
				try
				{
					Client* client = new Client((int)clients_.size());
					if (client->Run())
					{
						clients_.push_back(client);
						count++;
					}
				}
				catch (exception& e)
				{
					LOG("%s\n", e.what());
				}
			}

			LOG("%d�� �߰� ����, ��ü Ŭ���̾�Ʈ ��: %d \n", count, clients_.size());
			return;
		}
	}

	LOG("�߰� ����\n");
}

void Application::ChangeValue(const string& name, int& value)
{
	stringstream strm;
	strm << endl;
	strm << name << " ����. (����: " << value << ")" << endl;
	strm << "B. �ڷ� ����" << endl;
	strm << ">> ";
	LOG(strm.str().c_str());

	fflush(stdin);
	string input;
	cin >> input;

	if (!cin.fail())
	{
		if (input == "B" || input == "b")
		{
			return;
		}
		else if (all_of(input.begin(), input.end(), ::isdigit))
		{
			int prob = stoi(input);

			if (value != prob)
			{
				int prevProb = value;
				value = prob;
				LOG("%d->%d ���� ����\n", prevProb, value);
				return;
			}
		}
	}

	LOG("���� ����\n");
}

void Application::ChangeCloseProbPerFrame()
{
	ChangeValue("Ŭ���̾�Ʈ ���� ���� Ȯ��", config_.closeProbPerFrame_);
}

void Application::ChangeSendCountPerSec()
{
	ChangeValue("�ʴ� �۽� Ƚ��", config_.sendCountPerSecond_);
}

void Application::ChangeSendPacketSize()
{
	ChangeValue("�۽� ��Ŷ ������", config_.sendPacketSizeMax_);
}

void Application::ChangeBroadcastFlag()
{
	function<const char*(bool flag)> getFlagStr = [](bool flag)
	{
		return flag ? "True" : "False";
	};

	stringstream strm;
	strm << endl;
	strm << "Broadcast ���� ����. (����: " << getFlagStr(config_.broadcast_) << ")" << endl;
	strm << "1. Ehco Only" << endl;
	strm << "2. Broadcast" << endl;
	strm << "B. �ڷ� ����" << endl;
	strm << ">> ";
	LOG(strm.str().c_str());

	fflush(stdin);
	string input;
	cin >> input;

	if (!cin.fail())
	{
		if (input == "B" || input == "b")
		{
			return;
		}
		else if (all_of(input.begin(), input.end(), ::isdigit))
		{
			switch (stoi(input))
			{
			case 1: config_.broadcast_ = false; break;
			case 2: config_.broadcast_ = true; break;
			default: return;
			}

			LOG("���� ���� flag: %s\n", getFlagStr(config_.broadcast_));
		}
	}

	LOG("���� ����\n");
}