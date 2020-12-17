#pragma once
#include"banks.h"
#define internal_memory_size (0xFFFF-0xC000)

namespace gbe{
	enum class reserved_memory_locations_enum : word{
		CARTRIDGE_TYPE = 0x0147,
		ROM_SIZE = 0x0148,
		RAM_SIZE = 0x0149,
		DIVIDER_REG = 0xFF04,
		INTERRUPT_REGISTER = 0xFFFF
	};
	struct mem_t{
		void load_ROM(const char*);
		void write_byte_to_memory(word adr, byte value);
		byte read_byte_from_memory(word adr);
		void write_word_to_memory(word adr, word value);
		word read_word_from_memory(word adr);
		inline void set_interrupt_flag(byte val){
			this->interrupt_reg = val;
		}
		inline byte get_interrupt_flag(){
			return this->interrupt_reg;
		}
	protected: 
		template<typename t>
		void write_to_internal_memory(const word& adr, t value){
			if(adr >= 0xE000 && adr <= 0xFDFF);	//	echo ram?! do it for other side too!!!!!!!!!!!!!
				*((t*)&mem[adr-2000]) = value;
			else if(adr >= 0xC000 && adr <= 0xDDFF)
				*((t*)&mem[adr+2000]) = value;
			*((t*)&mem[adr]) = value;
		}
		template<typename t>
		t read_from_internal_memory(word adr){
			return (t)mem[adr];
		}
		using mmu = memory_bank_controller_t*;
		struct mem_controller_t{
			virtual inline ~mem_controller_t(){
				if(mem_controller)
					delete mem_controller;
			}
			inline void write_byte(const word& adr, byte val){
				this->mem_controller->write_mem(adr, val);
			}
			inline void write_word(const word& adr, word val){
				this->mem_controller->write_mem(adr, val);
				this->mem_controller->write_mem(adr, val >> 8);
			}
			inline byte read_byte(const word& adr){
				return this->mem_controller->read_mem(adr);
			}
			inline word read_word(const word& adr){
				word rtrn;
				rtrn = (this->mem_controller->read_mem(adr) << 8) & this->mem_controller->read_mem(adr+1);
				return rtrn;
			}
			inline void copy_rom(byte* rom, int size){
				this->mem_controller->copy_rom(rom, size);
			}
			void set_bank_type(const int& type, const int& rom_size, const int& ram_size);
		private:
			int type{0};
			mmu mem_controller{0};
			bool is_initalized{false};
		} mem_controller;
		byte mem[internal_memory_size];		//	Because we're not emulating GBC, we don't have to make WRAM exist in a bank but rather as internal memory.
		byte& interrupt_reg{mem[static_cast<int>(reserved_memory_locations_enum::INTERRUPT_REGISTER)]};
		byte& divider_reg{}
	};
}