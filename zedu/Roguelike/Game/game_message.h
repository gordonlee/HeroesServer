#pragma once
#include "Core/types.h"

#pragma pack( push, 1 )

namespace zedu {

	struct IMessage
	{
		ushort size;	// Data Size(2 byte)
		byte flag;		// Flag(1 byte)
		byte checksum;	// Check Sum(1 byte)
	};

	struct MSG_Value : public IMessage
	{
		MSG_Value()
		{
		}
	};

}

#pragma pack( pop )