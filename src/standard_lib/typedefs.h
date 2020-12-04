#pragma once
namespace gbe{
	enum gbe_error_codes{
		UNKNOWN = 0,
		ACTIVE_BANK_NON_INITALIZED,	//	Thrown if either of the active_banks members are null.	
		FSTREAM_INVALID
	};
	typedef gbe_error_codes gbe_error; 
}

typedef unsigned char byte;
typedef unsigned short word;