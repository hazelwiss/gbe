#pragma once
namespace gbe{
	enum gbe_error_codes{
		UNKNOWN = 0,
		ACTIVE_BANK_NON_INITALIZED,	//	Thrown if either of the active_banks members are null.	
		FSTREAM_INVALID
	};
	struct gbe_exception{
		inline gbe_exception(gbe_error_codes error_code): error_code{error_code} {}
		gbe_error_codes error_code;
	};
}

typedef unsigned char byte;
typedef unsigned short word;