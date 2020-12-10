#pragma once
#include<standard_lib/typedefs.h>

namespace gbe{
	template<int size>
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
		//	Adding more members might fuck up some code where I use sizeof()
		byte mem[size]{};
	};
	struct rom_bank_t: bank_t<16_kb>{};
	struct ram_bank_t: bank_t<8_kb>{};
	enum class reserved_memory_locations_enum : word{
		CARTRIDGE_TYPE = 0x0147,
		ROM_SIZE = 0x0148,
		RAM_SIZE = 0x0149,
	};
	//	abstract class for runtime polymorphism
	struct memory_bank_controller_t{
		inline memory_bank_controller_t(int& switchable_rom_banks, int& switchable_ram_banks):
			switchable_rom_banks_size_type{switchable_rom_banks}, switchable_ram_banks_size_type{switchable_ram_banks} {}
		virtual void write_ram(const word& adr, byte val) = NO_IMPLEMENTATION;
		virtual byte read_ram(const word& adr) = NO_IMPLEMENTATION;
		virtual bool determine_ram_enable() = NO_IMPLEMENTATION;
		virtual bool should_grab_control_of_write_or_read_operation() = NO_IMPLEMENTATION; 	//	should be called every memory write/read to determine
																							//	whether or not the memory bank should grab control.
		inline void set_write_ram(const bool& value){
			*const_cast<bool*>(&ram_writing_enabled) = value;
		}
		void copy_rom(const byte* rom_data, int rom_size);
	protected:
		rom_bank_t rom_0;
		rom_bank_t* active_switchable_rom_bank;
		ram_bank_t* active_switchable_ram_bank;
		rom_bank_t* switchable_rom_banks;
		ram_bank_t* switchable_ram_banks;
		int switchable_ram_banks_size_type{0};	//	Ambigious naming?
		int switchable_ram_banks_size{0};
		int switchable_rom_banks_size_type{0};
		int switchable_rom_banks_size{0};
		const bool ram_writing_enabled{false};
		virtual void setup_rom_and_ram_banks();
		void determine_bank_size();
	};
	struct memory_bank_controller_none_t: memory_bank_controller_t{
		inline memory_bank_controller_none_t(int switchable_rom_banks, int switchable_ram_banks): 
			memory_bank_controller_t(switchable_rom_banks, switchable_ram_banks) {} 
		void write_ram(const word& adr, byte val) override;
		byte read_ram(const word& adr) override;
		bool determine_ram_enable() override;
		bool should_grab_control_of_write_or_read_operation() override;
	};
	struct memory_bank_controller_mbc1_t: memory_bank_controller_t{
		inline memory_bank_controller_mbc1_t(int switchable_rom_banks, int switchable_ram_banks): 
			memory_bank_controller_t(switchable_rom_banks, switchable_ram_banks) {}
		void write_ram(const word& adr, byte val) override;
		byte read_ram(const word& adr) override;
		bool determine_ram_enable() override;
		bool should_grab_control_of_write_or_read_operation() override;
	};	
	struct memory_bank_controller_mbc2_t: memory_bank_controller_t{ 		//	Has no external ram, so we'll have to account for that!
		inline memory_bank_controller_mbc2_t(int switchable_rom_banks, int switchable_ram_banks): 
			memory_bank_controller_t(switchable_rom_banks, switchable_ram_banks) {}
		void write_ram(const word& adr, byte val) override;
		byte read_ram(const word& adr) override;
		bool determine_ram_enable() override;
		bool should_grab_control_of_write_or_read_operation() override;
	protected:
		void setup_rom_and_ram_banks() override;
	};	
	struct memory_bank_controller_mbc3_t: memory_bank_controller_t{
		inline memory_bank_controller_mbc3_t(int switchable_rom_banks, int switchable_ram_banks): 
			memory_bank_controller_t(switchable_rom_banks, switchable_ram_banks) {}
		void write_ram(const word& adr, byte val) override;
		byte read_ram(const word& adr) override;
		bool determine_ram_enable() override;
		bool should_grab_control_of_write_or_read_operation() override;
	};
}