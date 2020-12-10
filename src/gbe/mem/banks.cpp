#include"banks.h"

void gbe::memory_bank_controller_t::copy_rom(const byte* rom_data, int rom_size){
	this->determine_bank_size();
	this->setup_rom_and_ram_banks();
	int i{0};
	for(; i < 16_kb; ++i){
		this->rom_0.write_to(i, rom_data[i]);
	}
	for(int bank{0}, bank_i; i < rom_size; ++i, bank_i = (bank_i+1) % sizeof(*switchable_rom_banks), bank = bank_i ? bank : bank+1){
		if(bank >= this->switchable_rom_banks_size)
			throw gbe::gbe_error::CARTRIDGE_BANKS_AND_ROM_MISMATCH;
		this->switchable_rom_banks[bank].write_to(bank_i, rom_data[i]);
		int test = this->switchable_rom_banks[bank].read_from<byte>(bank_i);
		int x = 2;
	}
}

void gbe::memory_bank_controller_t::determine_bank_size(){
	int& rom{this->switchable_rom_banks_size}, &ram{this->switchable_ram_banks_size};
	if(this->switchable_rom_banks_size_type <= 8){
		rom = 1;
		for(int i{0}; i <= this->switchable_rom_banks_size_type; ++i)
			rom *= 2;
	}
	else
		switch(this->switchable_rom_banks_size_type){
			case 0x52:
				rom = 72;
				break;
			case 0x53:
				rom = 80;
				break;
			case 0x54:
				rom = 96;
				break;
			default:
				throw gbe_error::CARTRIDGE_BANKS_AND_ROM_MISMATCH;
		}
	switch(this->switchable_ram_banks_size_type){
		case 0:
			ram = 0;
			break;
		case 1:
			ram = -1;	// don't know what to do with this yet?!
			break;
		case 2:
			ram = 1;
			break;
		case 3:
			ram = 4;
			break;
		case 4:
			ram = 16;
			break;
		case 5:
			ram = 8;
			break;
	}
}

void gbe::memory_bank_controller_t::setup_rom_and_ram_banks(){
	if(this->switchable_rom_banks)
		delete[] this->switchable_rom_banks;
	if(this->switchable_ram_banks)
		delete this->switchable_ram_banks;
	this->switchable_rom_banks = new rom_bank_t[this->switchable_rom_banks_size];
	this->switchable_ram_banks = new ram_bank_t[this->switchable_ram_banks_size];
}

void gbe::memory_bank_controller_none_t::write_ram(const word& adr, byte val){}
byte gbe::memory_bank_controller_none_t::read_ram(const word& adr){
	int x = 5;
}
bool gbe::memory_bank_controller_none_t::determine_ram_enable(){}
bool gbe::memory_bank_controller_none_t::should_grab_control_of_write_or_read_operation(){}

void gbe::memory_bank_controller_mbc1_t::write_ram(const word& adr, byte val){}
byte gbe::memory_bank_controller_mbc1_t::read_ram(const word& adr){
	int x = 5;
}
bool gbe::memory_bank_controller_mbc1_t::determine_ram_enable(){}
bool gbe::memory_bank_controller_mbc1_t::should_grab_control_of_write_or_read_operation(){}

void gbe::memory_bank_controller_mbc2_t::write_ram(const word& adr, byte val){}
byte gbe::memory_bank_controller_mbc2_t::read_ram(const word& adr){
	int x = 5;
}
bool gbe::memory_bank_controller_mbc2_t::determine_ram_enable(){}
bool gbe::memory_bank_controller_mbc2_t::should_grab_control_of_write_or_read_operation(){}
void gbe::memory_bank_controller_mbc2_t::setup_rom_and_ram_banks(){
	if(this->switchable_rom_banks)
		delete[] this->switchable_rom_banks;
	if(this->switchable_ram_banks)
		delete this->switchable_ram_banks;
	if(this->switchable_rom_banks_size == 64)
		this->switchable_rom_banks_size = 63;
	if(this->switchable_rom_banks_size == 128)
		this->switchable_rom_banks_size = 125;

	this->switchable_rom_banks = new rom_bank_t[this->switchable_rom_banks_size];
	this->switchable_ram_banks = new ram_bank_t[this->switchable_ram_banks_size];
}

void gbe::memory_bank_controller_mbc3_t::write_ram(const word& adr, byte val){}
byte gbe::memory_bank_controller_mbc3_t::read_ram(const word& adr){
	int x = 5;
}
bool gbe::memory_bank_controller_mbc3_t::determine_ram_enable(){}
bool gbe::memory_bank_controller_mbc3_t::should_grab_control_of_write_or_read_operation(){}
