#include"mem.h"
#include<fstream>
#include<cstring>

inline bool see_if_bank_memory(const word& adr){
	return (adr >= 0x4000 && adr <= 0x7FFF);
}

void gbe::mem_t::write_byte_to_memory(word adr, byte value){
	if(see_if_bank_memory(adr))
		this->active_bank->write_to(adr, value);
	else
		write_to_memory(adr, value);
}
byte gbe::mem_t::read_byte_from_memory(word adr){
	if(see_if_bank_memory(adr))
		return this->active_bank->read_from<byte>(adr);
	return read_from_memory<byte>(adr);
}
void gbe::mem_t::write_word_to_memory(word adr, word value){
	if(see_if_bank_memory(adr))
		this->active_bank->write_to(adr, value);
	else 
		write_to_memory(adr, value);
}
word gbe::mem_t::read_word_from_memory(word adr){
	if(see_if_bank_memory(adr))
		return this->active_bank->read_from<word>(adr);
	return read_from_memory<word>(adr);
}

void gbe::mem_t::load_ROM(const char* rom){
	std::fstream stream(rom, std::ios::binary | std::ios::ate);
	if(!stream)
		throw gbe::gbe_exception(gbe::gbe_error_codes::FSTREAM_INVALID);
	char buffer[16_kb];
	char* iter_buffer = buffer;
	while(stream.get(*(iter_buffer++)));
	memcpy(buffer, iter_buffer, sizeof(buffer));
}