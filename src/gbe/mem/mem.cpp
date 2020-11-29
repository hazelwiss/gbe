#include"mem.h"
#include<cstring>

void gbe::mem_t::swap_bank(bank_t* bank, byte pos){
	if(!active_banks.higher_bank | !active_banks.lower_bank)
		throw gbe_exception(gbe_error_codes::ACTIVE_BANK_NON_INITALIZED);
	if(pos)
		memcpy(this->active_banks.higher_bank->mem, reinterpret_cast<char*>(&this->mem)+16_kb, sizeof(this->active_banks.higher_bank->mem));
	else
		memcpy(this->active_banks.lower_bank->mem, this->mem, sizeof(this->active_banks.lower_bank->mem));
}