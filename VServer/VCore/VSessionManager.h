#include <vector>
#include <map>
#include "VClientSession.h"
#include "VBufferController.h"

namespace VCore
{
	class VSessionManager
	{
	private:

		typedef std::map<ULONG, ClientSession*> SessionMap;

		SessionMap sessionList_;

	public:
		
		VSessionManager()
			: sessionList_()
		{
		}

		ClientSession* GetSession(const int id)
		{
			return sessionList_[id];
		}

		SessionMap::const_iterator Begin()
		{
			return sessionList_.begin();
		}

		SessionMap::const_iterator End()
		{
			return sessionList_.end();
		}

		ClientSession* MakeSession(const ULONG id, SOCKET acceptedSock)
		{
			ClientSession* client = new ClientSession(acceptedSock);
			sessionList_.insert(SessionMap::value_type(id, client));

			return client;
		}

	};

}
