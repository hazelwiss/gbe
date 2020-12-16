#pragma once
#include<standard_lib/typedefs.h>
#define RAM_BANKING_MODE 0x01
#define ROM_BANKING_MODE 0x00

namespace gbe{
	template<int size>
	struct bank_t{
		template<typename t>
		inline void write_to(const word adr, t value){
			*(reinterpret_cast<t*>(&mem[adr % size])) = value;
		}
		template<typename t>
		inline t read_from(const word adr){
			return *reinterpret_cast<t*>(&mem[adr]);
		}
	protected:
		//	Adding more members might fuck up some code where I use sizeof()
		byte mem[size]{};
	};
	struct rom_bank_t: bank_t<16_kb>{};
	struct ram_bank_t: bank_t<8_kb>{};
	//	abstract class for runtime polymorphism
	struct memory_bank_controller_t{
		inline memory_bank_controller_t(int& switchable_rom_banks, int& switchable_ram_banks):
			switchable_rom_banks_size_type{switchable_rom_banks}, switchable_ram_banks_size_type{switchable_ram_banks} {}
		void write_mem(const word& adr, byte val);
		byte read_mem(const word& adr);
		virtual bool determine_ram_enable(const word& adr, byte val);
		virtual bool determine_ram_rom_mode(const word& adr, byte val);
		virtual void determine_bank_swap(const word& adr, byte val) = NO_IMPLEMENTATION;
		void copy_rom(const byte* rom_data, int rom_size);
	protected:
		rom_bank_t rom_0;
		rom_bank_t* const active_switchable_rom_bank{0};
		const int active_switchable_rom_bank_offset{0};
		ram_bank_t* const active_switchable_ram_bank{0};
		const int active_switchable_ram_bank_offset{0};
		rom_bank_t* switchable_rom_banks{0};
		ram_bank_t* switchable_ram_banks{0};
		int switchable_ram_banks_size_type{0};	//	Ambigious naming?
		int switchable_ram_banks_size{0};
		int switchable_rom_banks_size_type{0};
		int switchable_rom_banks_size{0};
		const bool ram_writing_enabled{false};
		const int rom_ram_mode{0};
		virtual void setup_rom_and_ram_banks();
		void determine_bank_size();
		inline bool set_write_ram(const bool& value){
			return *const_cast<bool*>(&ram_writing_enabled) = value;
		}
		inline int set_rom_ram_mode(const int& value){
			return *const_cast<int*>(&rom_ram_mode) = value;
		}
		inline void swap_rom_bank(int bank){
			(int&)this->active_switchable_rom_bank_offset = bank;
			(rom_bank_t*&)this->active_switchable_rom_bank = &this->switchable_rom_banks[bank];
		}
		inline void swap_ram_bank(int bank){
			(int&)this->active_switchable_ram_bank_offset = bank;
			(ram_bank_t*&)this->active_switchable_ram_bank = &this->switchable_ram_banks[bank];
		}
		inline rom_bank_t& select_rom_bank_from_address(const word& adr){
			if(adr >= 16_kb)
				return *this->active_switchable_rom_bank;
			return this->rom_0;
		}
		inline bool determine_if_rom_address(const word& adr){
			return adr && adr < 32_kb;
		}
		inline bool determine_if_ram_address(const word& adr){
			return adr >= 0xA000 && adr <= 0xBFFF;
		}
	};
	struct memory_bank_controller_none_t: memory_bank_controller_t{
		inline memory_bank_controller_none_t(int switchable_rom_banks, int switchable_ram_banks): 
			memory_bank_controller_t(switchable_rom_banks, switchable_ram_banks) {} 
		bool determine_ram_enable(const word& adr, byte val) override;
		void determine_bank_swap(const word& adr, byte val) override;
	};
	struct memory_bank_controller_mbc1_t: memory_bank_controller_t{
		inline memory_bank_controller_mbc1_t(int switchable_rom_banks, int switchable_ram_banks): 
			memory_bank_controller_t(switchable_rom_banks, switchable_ram_banks) {}
			void determine_bank_swap(const word& adr, byte val) override;
	protected:
		void setup_rom_and_ram_banks() override;
	};	
	struct memory_bank_controller_mbc2_t: memory_bank_controller_t{ 		//	Has no external ram, so we'll have to account for that!
		inline memory_bank_controller_mbc2_t(int switchable_rom_banks, int switchable_ram_banks): 
			memory_bank_controller_t(switchable_rom_banks, switchable_ram_banks) {}
		bool determine_ram_enable(const word& adr, byte val) override;
		void determine_bank_swap(const word& adr, byte val) override;
	};	
	struct memory_bank_controller_mbc3_t: memory_bank_controller_t{
		inline memory_bank_controller_mbc3_t(int switchable_rom_banks, int switchable_ram_banks): 
			memory_bank_controller_t(switchable_rom_banks, switchable_ram_banks) {}
		void determine_bank_swap(const word& adr, byte val) override;
	};
}