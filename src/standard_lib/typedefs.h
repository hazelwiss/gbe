#pragma once
#define NO_IMPLEMENTATION 0

namespace gbe{
	enum gbe_error_codes{
		UNKNOWN = 0,
		ACTIVE_BANK_NON_INITALIZED,	//	Thrown if either of the active_banks members are null.	
		INVALID_CARTRIDGE_TYPE,
		FSTREAM_INVALID,
		CARTRIDGE_BANKS_AND_ROM_MISMATCH,
		CARTRIDGE_BANKS_AND_RAM_MISMATCH,
		SWAPED_TO_NONEXISTANT_BANK,
		READ_OR_WRITE_TO_INVALID_ADDRESS,
	};
	typedef gbe_error_codes gbe_error; 
}

typedef unsigned char byte;
typedef unsigned short word;

constexpr int operator"" _kb(unsigned long long int bytes){
	return bytes*1024;
}
constexpr int operator"" _mb(unsigned long long int bytes){
	return bytes*1024_kb;
}