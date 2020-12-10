#pragma once
#include"banks.h"

namespace gbe{
	struct mem_t{
		void load_ROM(const char*);

		void write_byte_to_memory(word adr, byte value);
		byte read_byte_from_memory(word adr);
		void write_word_to_memory(word adr, word value);
		word read_word_from_memory(word adr);

	protected: 
		template<typename t>
		void write_to_internal_memory(const word& adr, t value){
			//*((t*)&mem[adr]) = value;
		}
		template<typename t>
		t read_from_internal_memory(word adr){
			//if(adr >= 0xE000 && adr <= 0xFDFF);	//	echo ram?!
				//adr -= 2000;
			//return (t)mem[adr];
		}
		using mmu = memory_bank_controller_t*;
		struct mem_controller_t{
			virtual inline ~mem_controller_t(){
				if(mem_controller)
					delete mem_controller;
			}
			inline memory_bank_controller_t& get_mmu(){
				return *this->mem_controller;
			}
			void set_bank_type(const int& type, const int& rom_size, const int& ram_size);
		private:
			int type{0};
			mmu mem_controller{0};
			bool is_initalized{false};
		} mem_controller;
	};
}