#include"mem.h"
#include<fstream>
#include<cstring>

#define RESERV_LOCATION_ROM_SIZE static_cast<int>(reserved_memory_locations_enum::ROM_SIZE)
#define RESERV_LOCATION_RAM_SIZE static_cast<int>(reserved_memory_locations_enum::RAM_SIZE)
#define RESERV_LOCATION_CARTRIDGE_TYPE static_cast<int>(reserved_memory_locations_enum::CARTRIDGE_TYPE)

void gbe::mem_t::write_byte_to_memory(word adr, byte value){
	if(this->mem_controller.get_mmu().should_grab_control_of_write_or_read_operation());
	else
		write_to_internal_memory(adr, value);
}
byte gbe::mem_t::read_byte_from_memory(word adr){
	if(this->mem_controller.get_mmu().should_grab_control_of_write_or_read_operation())
		return 0;
	return read_from_internal_memory<byte>(adr);
}
void gbe::mem_t::write_word_to_memory(word adr, word value){
	if(this->mem_controller.get_mmu().should_grab_control_of_write_or_read_operation());
	else 
		write_to_internal_memory(adr, value);
}
word gbe::mem_t::read_word_from_memory(word adr){
	if(this->mem_controller.get_mmu().should_grab_control_of_write_or_read_operation())
		return 0;
	return read_from_internal_memory<word>(adr);
}

void gbe::mem_t::load_ROM(const char* rom){
	std::ifstream stream(rom, std::ios::binary | std::ios::ate);
	if(!stream)
		throw gbe::gbe_error_codes::FSTREAM_INVALID;
	int size = stream.tellg(); 
	stream.seekg(std::fstream::beg);
	char* buffer = new char[size];
	char* tmp = buffer;
	while(stream.get(*(tmp++)));

	int cartridge_mode = buffer[RESERV_LOCATION_CARTRIDGE_TYPE];
	int rom_count = buffer[RESERV_LOCATION_ROM_SIZE];
	int ram_count = buffer[RESERV_LOCATION_RAM_SIZE];

	this->mem_controller.set_bank_type(0, rom_count, ram_count);

	//	Remember to copy ROM into the mem_controller
	this->mem_controller.get_mmu().copy_rom(reinterpret_cast<byte*>(buffer), size);

	delete[] buffer;
}

void gbe::mem_t::mem_controller_t::set_bank_type(const int& type, const int& rom_size, const int& ram_size){
	switch (this->type = type)
	{
		case 0:{
			this->mem_controller = new memory_bank_controller_none_t(rom_size, ram_size);
			break;
		}
		case 1:{
			this->mem_controller = new memory_bank_controller_mbc1_t(rom_size, ram_size);
			break;
		}
		case 2:{
			this->mem_controller = new memory_bank_controller_mbc2_t(rom_size, ram_size);
			break;
		}
		case 3:{
			this->mem_controller = new memory_bank_controller_mbc3_t(rom_size, ram_size);
			break;
		}
		default:
			throw gbe_error::INVALID_CARTRIDGE_TYPE;
	}
	this->is_initalized = true;
}