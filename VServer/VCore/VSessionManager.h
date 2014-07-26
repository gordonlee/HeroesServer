#include <vector>
#include <map>
#include "VClientSession.h"
#include "VBufferController.h"
#include "VLogger.h"

namespace VCore
{
	class VSessionManager
	{
	private:

		typedef std::map<ULONG, ClientSession*> SessionMap;
		typedef std::vector<ClientSession*> SessionVector;

		SessionVector	remainSsession_;
		SessionMap		usingSsession_;

	public:
		
		VSessionManager()
			: remainSsession_(), usingSsession_()
		{
		}

		ClientSession* GetSession(const ULONG id)
		{
			if (remainSsession_.size() > 0)
			{
				usingSsession_.insert(SessionMap::value_type(id, remainSsession_.back()));
				remainSsession_.pop_back();
			}

			return usingSsession_[id];
		}


		VOID MakeSession(const UINT maxSessionCount)
		{
			for (UINT id = 0; id < maxSessionCount; ++id)
			{
				ClientSession* client = new ClientSession();
				remainSsession_.push_back(client);
			}

		}

	};

}
