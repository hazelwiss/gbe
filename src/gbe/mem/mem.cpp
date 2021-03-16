#include"mem.h"
#include<fstream>
#include<cstring>
#define RESERV_LOCATION_ROM_SIZE static_cast<int>(reserved_memory_locations_enum::ROM_SIZE)
#define RESERV_LOCATION_RAM_SIZE static_cast<int>(reserved_memory_locations_enum::RAM_SIZE)
#define RESERV_LOCATION_CARTRIDGE_TYPE static_cast<int>(reserved_memory_locations_enum::CARTRIDGE_TYPE)
#define OAM_MEMORY_BUFFER_SIZE 160
#define OAM_MEMORY_BUFFER_START_ADDRESS 0xFE00

byte gbe::mem_t::boot_rom[256]{
	0x31, 0xfe, 0xff, 0xaf, 0x21, 0xff, 0x9f, 0x32, 0xcb, 0x7c, 0x20, 0xfb, 0x21, 0x26, 0xff, 0x0e,
	0x11, 0x3e, 0x80, 0x32, 0xe2, 0x0c, 0x3e, 0xf3, 0xe2, 0x32, 0x3e, 0x77, 0x77, 0x3e, 0xfc, 0xe0,
	0x47, 0x11, 0x04, 0x01, 0x21, 0x10, 0x80, 0x1a, 0xcd, 0x95, 0x00, 0xcd, 0x96, 0x00, 0x13, 0x7b,
	0xfe, 0x34, 0x20, 0xf3, 0x11, 0xd8, 0x00, 0x06, 0x08, 0x1a, 0x13, 0x22, 0x23, 0x05, 0x20, 0xf9,
	0x3e, 0x19, 0xea, 0x10, 0x99, 0x21, 0x2f, 0x99, 0x0e, 0x0c, 0x3d, 0x28, 0x08, 0x32, 0x0d, 0x20,
	0xf9, 0x2e, 0x0f, 0x18, 0xf3, 0x67, 0x3e, 0x64, 0x57, 0xe0, 0x42, 0x3e, 0x91, 0xe0, 0x40, 0x04,
	0x1e, 0x02, 0x0e, 0x0c, 0xf0, 0x44, 0xfe, 0x90, 0x20, 0xfa, 0x0d, 0x20, 0xf7, 0x1d, 0x20, 0xf2,
	0x0e, 0x13, 0x24, 0x7c, 0x1e, 0x83, 0xfe, 0x62, 0x28, 0x06, 0x1e, 0xc1, 0xfe, 0x64, 0x20, 0x06,
	0x7b, 0xe2, 0x0c, 0x3e, 0x87, 0xe2, 0xf0, 0x42, 0x90, 0xe0, 0x42, 0x15, 0x20, 0xd2, 0x05, 0x20,
	0x4f, 0x16, 0x20, 0x18, 0xcb, 0x4f, 0x06, 0x04, 0xc5, 0xcb, 0x11, 0x17, 0xc1, 0xcb, 0x11, 0x17,
	0x05, 0x20, 0xf5, 0x22, 0x23, 0x22, 0x23, 0xc9, 0xce, 0xed, 0x66, 0x66, 0xcc, 0x0d, 0x00, 0x0b,
	0x03, 0x73, 0x00, 0x83, 0x00, 0x0c, 0x00, 0x0d, 0x00, 0x08, 0x11, 0x1f, 0x88, 0x89, 0x00, 0x0e,
	0xdc, 0xcc, 0x6e, 0xe6, 0xdd, 0xdd, 0xd9, 0x99, 0xbb, 0xbb, 0x67, 0x63, 0x6e, 0x0e, 0xec, 0xcc,
	0xdd, 0xdc, 0x99, 0x9f, 0xbb, 0xb9, 0x33, 0x3e, 0x3c, 0x42, 0xb9, 0xa5, 0xb9, 0xa5, 0x42, 0x3c,
	0x21, 0x04, 0x01, 0x11, 0xa8, 0x00, 0x1a, 0x13, 0xbe, 0x20, 0xfe, 0x23, 0x7d, 0xfe, 0x34, 0x20,
	0xf5, 0x06, 0x19, 0x78, 0x86, 0x23, 0x05, 0x20, 0xfb, 0x86, 0x20, 0xfe, 0x3e, 0x01, 0xe0, 0x50
};

inline bool determine_if_bank_address(const word& adr){
	return adr < 0x8000 || (adr >= 0xA000 && adr <= 0xBFFF);
}

void gbe::mem_t::write_to_internal_memory(const word& adr, byte value){	
	if(adr >= 0xE000 && adr <= 0xFDFF){			//	echoes ram writen to this address to the address 2000 steps lower
		mem[adr-0x2000] = value;
		mem[adr] = value;
	}
	else if(adr >= 0xC000 && adr <= 0xDDFF){	//	echoes ram writen to this address to the address 2000 steps above
		mem[adr+0x2000] = value;
		mem[adr] = value;
	}
	else if(adr >= 0xFF00 && adr <= 0xFF7F){	//	I/O registers
		if(adr == (word)reserved_memory_locations_enum::DIVIDER_REGISTER)
			divider_reg = 0;	//	resets the divider reigster to zero whenever a value is written to it
		else if(adr == (word)reserved_memory_locations_enum::LCD_STATUS_REGISTER && //	only triggered during OAM scan, v/h-blank or LY=LYC
			(lcd_status_register & 0b0000'0011) != 3){	
		}
		else if(adr == (word)reserved_memory_locations_enum::DMA_TRANSFER){
			for(int i = 0; i < OAM_MEMORY_BUFFER_SIZE; ++i){
				write_to_internal_memory(OAM_MEMORY_BUFFER_START_ADDRESS+i,
					read_byte_from_memory((value<<8)+i));
			}
			mem[adr] = value;
		}
		else if(adr == (word)reserved_memory_locations_enum::CGB_MODE_ONLY);	//	do nothing here
		else
			mem[adr] = value;
	}
	else
		mem[adr] = value;
}

byte gbe::mem_t::read_byte_from_internal_memory(word adr){		
	if(adr == (word)reserved_memory_locations_enum::CGB_MODE_ONLY)
		return 0xFF;
	return mem[adr];
}

void gbe::mem_t::write_byte_to_memory(word adr, byte value){
	if(determine_if_bank_address(adr))
		mem_bank_controller.write_byte(adr, value);
	else 
		write_to_internal_memory(adr, value);
}
byte gbe::mem_t::read_byte_from_memory(word adr){
	if(adr < sizeof(mem_t::boot_rom) && is_boot_rom_mounted)
		return boot_rom[adr];
	if(determine_if_bank_address(adr))
		return mem_bank_controller.read_byte(adr);
	return read_byte_from_internal_memory(adr);
}
void gbe::mem_t::write_word_to_memory(word adr, word value){
	write_byte_to_memory(adr, value);
	write_byte_to_memory(adr+1, value >> 8);
}
word gbe::mem_t::read_word_from_memory(word adr){
	word rtrn;
	rtrn |= read_byte_from_memory(adr);
	rtrn |= read_byte_from_memory(adr+1) << 8;
	return rtrn;
}

void gbe::mem_t::increment_timer(unsigned long long int& cycles){
	byte divider_register_cmp_val = cycles % 256;
	if(!divider_register_cmp_val || divider_register_cmp_val < this->increment_divider_tmp_value)
		++this->divider_reg;
	this->increment_divider_tmp_value = divider_register_cmp_val;
	if(this->timer_control & BIT(2)){
		byte timer_counter_cmp_val;
		switch(this->timer_control & 0xb0000-0011){
		case 0b00:
			timer_counter_cmp_val = cycles%(cpu_freq/1024);
			break;
		case 0b01:
			timer_counter_cmp_val = cycles%(cpu_freq/16);
			break;
		case 0b10:
			timer_counter_cmp_val = cycles%(cpu_freq/64);
			break;
		case 0b11:
			timer_counter_cmp_val = cycles%(cpu_freq/256);
			break;
		}
		if(!timer_counter_cmp_val || timer_counter_cmp_val < this->increment_timer_tmp_value){
			if(this->timer_counter == 0xFF){
				this->timer_counter = this->timer_modulo;
				timer_overflow = true;
			}
			else if(timer_overflow && this->timer_counter == this->timer_modulo){
				timer_overflow = false;
				this->request_interrupt((byte)interrupt_bits::TIMER);
				++this->timer_counter;
			}
			else 
				++this->timer_counter;
		}
		this->increment_timer_tmp_value = timer_counter_cmp_val;
	}

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
	rom_info.mode = cartridge_mode;
	rom_info.ram_size = ram_count;
	rom_info.rom_size = rom_count;
	this->mem_bank_controller.set_bank_type(cartridge_mode, rom_count, ram_count);
	this->mem_bank_controller.copy_rom(reinterpret_cast<byte*>(buffer), size);
	delete[] buffer;
}

void gbe::mem_t::mem_controller_t::set_bank_type(const int& type, const int& rom_size, const int& ram_size){
	switch (this->type = type)
	{
		case 0x00:
			this->bank_controller = new memory_bank_controller_none_t(1, 1);
			break;
		case 0x01:
		case 0x02:
		case 0x03:
			this->bank_controller = new memory_bank_controller_mbc1_t(rom_size, ram_size);
			break;
		case 0x05:
		case 0x06:
			this->bank_controller = new memory_bank_controller_mbc2_t(rom_size, ram_size);
			break;
		case 0x0F:
		case 0x10:
		case 0x11:
		case 0x12:
		case 0x13:
			this->bank_controller = new memory_bank_controller_mbc3_t(rom_size, ram_size);
			break;
		default:
			throw gbe_error::INVALID_CARTRIDGE_TYPE;
	}
}