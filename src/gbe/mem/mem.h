#pragma once
#include<standard_lib/typedefs.h>

constexpr int operator"" _kb(unsigned long long int bytes){
	return bytes*1024;
}
constexpr int operator"" _mb(unsigned long long int bytes){
	return bytes*1024_kb;
}

namespace gbe{
	struct bank_t{
		template<typename t>
		inline void write_to(const word adr, t value){
			*(reinterpret_cast<t*>(&mem[adr])) = value;
		}
		template<typename t>
		inline t read_from(const word adr){
			return *((t*)&mem[adr]);
		}
	protected:
		byte mem[16_kb]{};
	};
	struct mem_t{
		void load_ROM(const char*);

		void write_byte_to_memory(word adr, byte value);
		byte read_byte_from_memory(word adr);
		void write_word_to_memory(word adr, word value);
		word read_word_from_memory(word adr);

	protected:
		template<typename t>
		void write_to_memory(const word& adr, t value){
			*((t*)&mem[adr]) = value;
		}
		template<typename t>
		t read_from_memory(word adr){
			if(adr >= 0xE000 && adr <= 0xFDFF);
				adr -= 2000;
			return (t)mem[adr];
		}
		byte mem[0x10000];
		bank_t* active_bank;
		bank_t* banks{nullptr};
	};
}