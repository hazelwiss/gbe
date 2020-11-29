#pragma once
#include<standard_lib/typedefs.h>

constexpr int operator"" _kb(unsigned long long int bytes){
	return bytes*1024;
}
constexpr int operator"" _mb(unsigned long long int bytes){
	return bytes*1024_kb;
}

namespace gbe{
	constexpr struct{
		word int0x0000{0x0000};	//	Reservred interrupts -
		word int0x0008{0x0008};
		word int0x0010{0x0010};
		word int0x0018{0x0018};
		word int0x0020{0x0020};
		word int0x0028{0x0028};
		word int0x0030{0x0030};
		word int0x0038{0x0038};
		word int0x0040{0x0040};
		word int0x0048{0x0048};	//	LCDC status interrupt.
		word int0x0050{0x0050};	//	Timer overflow interrupt.
		word int0x0058{0x0058};	//	Serial transfer competion interrupt.
		word int0x0060{0x0060};	//	High-to-low of P10-P13 interrupt.
								//	End of reserved interrupts.
		word cartridge_type{0x0147};
		word rom_size{0x0148};
		word ram_size{0x0149};

	} reserved_memory_locations;
	struct bank_t{
		byte mem[16_kb]{};	//	tmp!
	};
	struct mem_t{
		void swap_bank(bank_t* bank, byte pos);

	protected:
		byte mem[0x10000];
		struct{
			bank_t* lower_bank;
			bank_t* higher_bank;
		} active_banks;
		bank_t* banks{nullptr};
	};
}