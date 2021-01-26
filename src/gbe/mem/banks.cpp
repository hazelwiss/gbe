#include"banks.h"

void gbe::memory_bank_controller_t::copy_rom(const byte* rom_data, int rom_size){
	this->determine_bank_size();
	this->setup_rom_and_ram_banks();
	int i{0};
	for(; i < 16_kb && i < rom_size; ++i){
		this->rom_0.write_to(i, rom_data[i]);
	}
	for(int bank{0}, bank_i{0}; i < rom_size; ++i, bank_i = (bank_i+1) % sizeof(*switchable_rom_banks), bank = bank_i ? bank : bank+1){
		if(bank >= this->switchable_rom_banks_size)
			throw gbe::gbe_error::CARTRIDGE_BANKS_AND_ROM_MISMATCH;
		this->switchable_rom_banks[bank].write_to(bank_i, rom_data[i]);
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
	default:
		throw gbe_error::CARTRIDGE_BANKS_AND_ROM_MISMATCH;
	}
}
void gbe::memory_bank_controller_t::setup_rom_and_ram_banks(){
	if(this->switchable_rom_banks)
		delete[] this->switchable_rom_banks;
	if(this->switchable_ram_banks)
		delete[] this->switchable_ram_banks;
	if(this->switchable_rom_banks_size > 0){
		this->switchable_rom_banks = new rom_bank_t[this->switchable_rom_banks_size];
		this->swap_rom_bank(0);
	}
	if(this->switchable_ram_banks_size > 0){
		this->switchable_ram_banks = new ram_bank_t[this->switchable_ram_banks_size];
		this->swap_ram_bank(0);
	}
}
void gbe::memory_bank_controller_t::write_mem(const word& adr, byte val){
	determine_ram_enable(adr, val);
	determine_ram_rom_mode(adr, val);
	determine_bank_swap(adr, val);
	if(determine_if_rom_address(adr))
		return;
	else if(determine_if_ram_address(adr))
		if(this->ram_writing_enabled)
			this->active_switchable_ram_bank->write_to(adr-0xA000, val);
	else
		throw gbe_error::READ_OR_WRITE_TO_INVALID_ADDRESS;
}
byte gbe::memory_bank_controller_t::read_mem(const word& adr){
	if(determine_if_rom_address(adr))
		return select_rom_bank_from_address(adr).read_from(adr);
	else if(determine_if_ram_address(adr))
		return this->active_switchable_ram_bank->read_from(adr-0xA000);
	else
		throw gbe_error::READ_OR_WRITE_TO_INVALID_ADDRESS;
}

//	MBC NONE
void gbe::memory_bank_controller_none_t::determine_bank_swap(const word& adr, byte val){
	return;	//	this method should do nothing as it's essentially pointless when we don't have any banks.
}
void gbe::memory_bank_controller_none_t::determine_ram_enable(const word& adr, byte val){
	return;
}
void gbe::memory_bank_controller_none_t::determine_ram_rom_mode(const word& adr, byte val){
	return;
}
void gbe::memory_bank_controller_none_t::setup_rom_and_ram_banks(){
	this->switchable_rom_banks = new rom_bank_t[1];
	this->swap_rom_bank(0);
	this->switchable_ram_banks = new ram_bank_t[1];
	this->swap_ram_bank(0);
}

//	MBC 1
void gbe::memory_bank_controller_mbc1_t::determine_ram_enable(const word& adr, byte val){
	if(adr >= 0x0000 && adr <= 0x1FFF){
		bool should_enable_ram = ((val & 0x0F) == 0x0A);	//	any value with the lower 4 bits being equal to 0x0A enables writing to ram.
		this->set_write_ram(should_enable_ram);
	}
}
void gbe::memory_bank_controller_mbc1_t::setup_rom_and_ram_banks(){	//	The mbc1 bank is a special case. Don't remove!
	if(this->switchable_rom_banks)
		delete[] this->switchable_rom_banks;
	if(this->switchable_ram_banks)
		delete[] this->switchable_ram_banks;
	if(this->switchable_rom_banks_size == 64)
		this->switchable_rom_banks_size = 63;
	else if(this->switchable_rom_banks_size == 128)
		this->switchable_rom_banks_size = 125;

	this->switchable_rom_banks = new rom_bank_t[this->switchable_rom_banks_size];
	this->switchable_ram_banks = new ram_bank_t[this->switchable_ram_banks_size];
}
void gbe::memory_bank_controller_mbc1_t::determine_bank_swap(const word& adr, byte val){
	if(adr >= 0x2000 && adr <= 0x3FFF){		//	ROM swap
		// implement bit masking!
		int new_bank = adr & 0b0001-1111;
		if(!new_bank)
			new_bank = 1;		
		else if(new_bank == 0x20)
			new_bank = 0x21;
		else if(new_bank == 0x40)
			new_bank = 0x41;
		else if(new_bank == 0x60)
			new_bank = 0x61;
		if(new_bank > this->switchable_rom_banks_size)
			throw gbe_error::SWAPED_TO_NONEXISTANT_BANK;
		this->swap_rom_bank(new_bank);
	}
	else if(adr >= 0x4000 && adr <= 0x5FFF){	//	RAM swap
		if(this->rom_ram_mode == ROM_BANKING_MODE){
			int new_bank = (adr & 0b0111-1111) | this->active_switchable_rom_bank_offset;	//	this is potentially done wrong!
			this->swap_rom_bank(new_bank);
		} else if(this->rom_ram_mode == RAM_BANKING_MODE){
			int new_bank = (adr & 0b0110-0000) | this->active_switchable_ram_bank_offset;
			this->swap_ram_bank(new_bank);
		}
	}	
}
void gbe::memory_bank_controller_mbc1_t::determine_ram_rom_mode(const word& adr, byte val){
	
}

//	MBC 2
void gbe::memory_bank_controller_mbc2_t::determine_ram_enable(const word& adr, byte val){

}
void gbe::memory_bank_controller_mbc2_t::determine_ram_rom_mode(const word& adr, byte val){
	
}
void gbe::memory_bank_controller_mbc2_t::determine_bank_swap(const word& adr, byte val){

}
void gbe::memory_bank_controller_mbc2_t::setup_rom_and_ram_banks(){
	
}


//	MBC 3
void gbe::memory_bank_controller_mbc3_t::determine_ram_enable(const word& adr, byte val){

}
void gbe::memory_bank_controller_mbc3_t::determine_ram_rom_mode(const word& adr, byte val){
	
}
void gbe::memory_bank_controller_mbc3_t::determine_bank_swap(const word& adr, byte val){

}
void gbe::memory_bank_controller_mbc3_t::setup_rom_and_ram_banks(){
	
}

