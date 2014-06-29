#pragma once

#pragma pack( push, 1 )

namespace zedu {

	struct Message
	{
		unsigned int		head;
		unsigned short		msg;
		unsigned int		size;
		unsigned int		total_size;
		unsigned int		checksum;
	};
}

#pragma pack( pop )