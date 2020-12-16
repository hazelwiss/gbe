#include"general_instructions.h"

// 8-Bit-Loads
void gbe::instructions::b8_load(word adr, byte& reg){
	reg = this->mem.read_byte_from_memory(adr);
}

// 16-Bit-Loads
void gbe::instructions::b16_load(word adr, word& reg){
	reg = this->mem.read_word_from_memory(adr);
}
void gbe::instructions::b16_load_sp_e(byte offset, word& reg){
	reset_all_flags();
	set_c_flag(this->cpu.regs.sp > 0xFFFF-offset);
	set_h_flag((this->cpu.regs.sp & 0x0F) + (offset & 0x0F) > 0x0F);
	reg = this->mem.read_word_from_memory(this->cpu.regs.sp + offset);
}	

//	8-Bit-Alu
void gbe::instructions::b8_add(byte val){
	reset_all_flags();
	set_h_flag((this->cpu.regs.a & 0x0F) + (val & 0x0F) > 0x0F);
	set_c_flag(this->cpu.regs.a );
	this->cpu.regs.a += val;
	set_z_flag(this->cpu.regs.a);
}
void gbe::instructions::b8_adc(byte val){
	int sum = val+this->cpu.regs.f.bits.c;
	reset_all_flags();
	set_h_flag((this->cpu.regs.a & 0x0F) + (val & 0x0F) > 0x0F);
	set_c_flag(this->cpu.regs.a > (0xFF-val));
	this->cpu.regs.a += sum;
	set_z_flag(this->cpu.regs.a);
}
void gbe::instructions::b8_sub(byte val){
	reset_all_flags();
	set_n_flag(true);
	set_h_flag((this->cpu.regs.a & 0x0F) + ((~val+1) & 0x0F) > 0x0F);
	set_c_flag(this->cpu.regs.a > 0xFF - (~val+1));
	this->cpu.regs.a -= val;
	set_z_flag(this->cpu.regs.a);

}
void gbe::instructions::b8_sbc(byte val){
	int sum = (~val+1)+this->cpu.regs.f.bits.c;
	reset_all_flags();
	set_n_flag(true);
	set_h_flag((this->cpu.regs.a & 0x0F) + (sum & 0x0F) > 0x0F);
	set_c_flag(this->cpu.regs.a > 0xFF - sum+1);
	this->cpu.regs.a -= val;
	set_z_flag(this->cpu.regs.a);
}
void gbe::instructions::b8_and(byte val){
	reset_all_flags();
	set_h_flag(true);
	this->cpu.regs.a &= val;
	set_z_flag(this->cpu.regs.a);
}
void gbe::instructions::b8_or(byte val){
	reset_all_flags();
	this->cpu.regs.a |= val;
	set_z_flag(this->cpu.regs.a);
}
void gbe::instructions::b8_xor(byte val){
	reset_all_flags();
	this->cpu.regs.a ^= val;
	set_z_flag(this->cpu.regs.a);
}
void gbe::instructions::b8_cp(byte val){
	reset_all_flags();
	set_n_flag(true);
	set_h_flag((this->cpu.regs.a & 0x0F) + ((~val+1) & 0x0F) > 0x0F);
	set_c_flag(this->cpu.regs.a > 0xFF - (~val+1));
	set_z_flag(this->cpu.regs.a - val);
}
void gbe::instructions::b8_inc(byte& reg){
	set_n_flag(false);
	set_h_flag(reg+1 > 0x0F);
	set_z_flag(++reg);
}
void gbe::instructions::b8_dec(byte& reg){
	set_n_flag(true);
	set_h_flag(reg + ~1 > 0x0F);
	set_z_flag(--reg);
}

//	16-Bit-Arithmetic
void gbe::instructions::b16_add(word val){
	set_n_flag(false);
	set_h_flag((this->cpu.regs.hl & 0x0FFF) + (val & 0x0FFF) > 0x0FFF);
	set_c_flag(this->cpu.regs.hl > 0xFFFF - val);
	this->cpu.regs.hl += val;
}
void gbe::instructions::b16_add_sp(byte val){
	reset_all_flags();
	set_h_flag(this->cpu.regs.sp & 0x0FFF + val > 0x0FFF);
	set_c_flag(this->cpu.regs.sp > 0xFFFF - val);
	this->cpu.regs.sp += val;
}
void gbe::instructions::b16_inc(word& reg){

}
void gbe::instructions::b16_dec(word& reg){

}
