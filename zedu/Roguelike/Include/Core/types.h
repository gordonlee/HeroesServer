#pragma once

namespace zedu {
	typedef unsigned int		uint32;
	typedef unsigned short		ushort;
	typedef unsigned long		ulong;
	typedef char				byte;
	

	struct Types
	{
		static int allocate()
		{
			static int total = 0;
			return total++;
		}
	};
};