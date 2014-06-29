#pragma once
#include "message_base.h"

	
namespace zedu {

	const unsigned short		MMO_CHAT		= 10;

	#pragma pack( push, 1 )

	struct MMO_Chat : public Message
	{
		MMO_Chat() : msg(MMO_CHAT), size(sizeof( *this ))
		{
		}

		char buffer[256];
	};

	#pragma pack( pop )
}