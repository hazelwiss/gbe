#include"general_instructions.h"
#include<gbe/cpu/cpu.h>

//	Constructor
gbe::general_instructions_t::general_instructions_t(gbe::cpu_t& cpu): cpu{cpu}, mem{cpu.memory}  {}

//	Helper functions
inline void gbe::general_instructions_t::reset_all_flags(){
	this->cpu.regs.f.all_bits = 0;
}
inline void gbe::general_instructions_t::set_z_flag(int val){
	this->cpu.regs.f.bits.z = (val != 0);
}
inline void gbe::general_instructions_t::set_n_flag(int val){
	this->cpu.regs.f.bits.n = (val != 0);
}
inline void gbe::general_instructions_t::set_h_flag(int val){
	this->cpu.regs.f.bits.h = (val != 0);
}
inline void gbe::general_instructions_t::set_c_flag(int val){
	this->cpu.regs.f.bits.c = (val != 0);
}
void gbe::general_instructions_t::push_to_stack(word val){
	this->mem.write_word_to_memory(this->cpu.regs.sp, val);
	this->cpu.regs.sp -= 2;
}

word gbe::general_instructions_t::pop_from_stack(){
	word rtrn = this->mem.read_word_from_memory(this->cpu.regs.sp);
	this->cpu.regs.sp += 2;
	return rtrn;
}


// 8-Bit-Loads
void gbe::general_instructions_t::b8_load_reg_val(byte& dst, byte src){
	dst = src;
}
void gbe::general_instructions_t::b8_load_reg_adr(byte& dst, word src){
	dst = mem.read_byte_from_memory(src);
}
void gbe::general_instructions_t::b8_load_adr_val(word dst, byte src){
	mem.write_byte_to_memory(dst, src);
}

// 16-Bit-Loads
void gbe::general_instructions_t::b16_load_reg_val(word& dst, word src){
	dst = src;
}
void gbe::general_instructions_t::b16_load_reg_adr(word& dst, word src){
	dst = mem.read_word_from_memory(src);
}
void gbe::general_instructions_t::b16_load_adr_val(word dst, word src){
	mem.write_word_to_memory(dst, src);
}
void gbe::general_instructions_t::b16_load_sp_e(word& reg, byte offset){
	reset_all_flags();
	set_c_flag(this->cpu.regs.sp > 0xFFFF-offset);
	set_h_flag((this->cpu.regs.sp & 0x0F) + (offset & 0x0F) > 0x0F);
	reg = this->mem.read_word_from_memory(this->cpu.regs.sp + offset);
}	
void gbe::general_instructions_t::b16_push(word nn){
	push_to_stack(nn);
}
void gbe::general_instructions_t::b16_pop(word& reg){
	reg = pop_from_stack();
}

//	8-Bit-Alu
void gbe::general_instructions_t::b8_add(byte val){
	reset_all_flags();
	set_h_flag((this->cpu.regs.a & 0x0F) + (val & 0x0F) > 0x0F);
	set_c_flag(this->cpu.regs.a );
	this->cpu.regs.a += val;
	set_z_flag(this->cpu.regs.a);
}
void gbe::general_instructions_t::b8_adc(byte val){
	int sum = val+this->cpu.regs.f.bits.c;
	reset_all_flags();
	set_h_flag((this->cpu.regs.a & 0x0F) + (val & 0x0F) > 0x0F);
	set_c_flag(this->cpu.regs.a > (0xFF-val));
	this->cpu.regs.a += sum;
	set_z_flag(this->cpu.regs.a);
}
void gbe::general_instructions_t::b8_sub(byte val){
	reset_all_flags();
	set_n_flag(true);
	set_h_flag((this->cpu.regs.a & 0x0F) + ((~val+1) & 0x0F) > 0x0F);
	set_c_flag(this->cpu.regs.a > 0xFF - (~val+1));
	this->cpu.regs.a -= val;
	set_z_flag(this->cpu.regs.a);

}
void gbe::general_instructions_t::b8_sbc(byte val){
	int sum = (~val+1)+this->cpu.regs.f.bits.c;
	reset_all_flags();
	set_n_flag(true);
	set_h_flag((this->cpu.regs.a & 0x0F) + (sum & 0x0F) > 0x0F);
	set_c_flag(this->cpu.regs.a > 0xFF - sum+1);
	this->cpu.regs.a -= val;
	set_z_flag(this->cpu.regs.a);
}
void gbe::general_instructions_t::b8_and(byte val){
	reset_all_flags();
	set_h_flag(true);
	this->cpu.regs.a &= val;
	set_z_flag(this->cpu.regs.a);
}
void gbe::general_instructions_t::b8_or(byte val){
	reset_all_flags();
	this->cpu.regs.a |= val;
	set_z_flag(this->cpu.regs.a);
}
void gbe::general_instructions_t::b8_xor(byte val){
	reset_all_flags();
	this->cpu.regs.a ^= val;
	set_z_flag(this->cpu.regs.a);
}
void gbe::general_instructions_t::b8_cp(byte val){
	reset_all_flags();
	set_n_flag(true);
	set_h_flag((this->cpu.regs.a & 0x0F) + ((~val+1) & 0x0F) > 0x0F);
	set_c_flag(this->cpu.regs.a > 0xFF - (~val+1));
	set_z_flag(this->cpu.regs.a - val);
}
void gbe::general_instructions_t::b8_inc(byte& reg){
	set_n_flag(false);
	set_h_flag(reg+1 > 0x0F);
	set_z_flag(++reg);
}
void gbe::general_instructions_t::b8_inc_adr(word adr){
	byte val = mem.read_byte_from_memory(adr)+1;
	set_n_flag(false);
	set_h_flag(val > 0x0F);
	mem.write_word_to_memory(adr, val);
	set_z_flag(val);
}
void gbe::general_instructions_t::b8_dec(byte& reg){
	set_n_flag(true);
	set_h_flag(reg + ~1 > 0x0F);
	set_z_flag(--reg);
}
void gbe::general_instructions_t::b8_dec_adr(word adr){
	byte val = mem.read_byte_from_memory(adr)-1;
	set_n_flag(false);
	set_h_flag(val > 0x0F);
	mem.write_word_to_memory(adr, val);
	set_z_flag(val);
}

//	16-Bit-Arithmetic
void gbe::general_instructions_t::b16_add(word val){
	set_n_flag(false);
	set_h_flag((this->cpu.regs.hl & 0x0FFF) + (val & 0x0FFF) > 0x0FFF);
	set_c_flag(this->cpu.regs.hl > 0xFFFF - val);
	this->cpu.regs.hl += val;
}
void gbe::general_instructions_t::b16_add_sp(signed char val){
	reset_all_flags();
	set_h_flag(this->cpu.regs.sp & 0x0FFF + val > 0x0FFF);
	set_c_flag(this->cpu.regs.sp > 0xFFFF - val);
	this->cpu.regs.sp += val;
}
void gbe::general_instructions_t::b16_inc(word& reg){
	++reg;
}
void gbe::general_instructions_t::b16_dec(word& reg){
	--reg;
}

//	Miscellaneous
void gbe::general_instructions_t::misc_swap(byte& reg){
	reset_all_flags();
	byte upper = reg & 0xF0;
	byte lower = reg & 0x0F;
	reg = upper | lower;
	set_z_flag(reg);
}
void gbe::general_instructions_t::misc_daa(){
	if(!this->cpu.regs.f.bits.n){
		if(this->cpu.regs.f.bits.c || (this->cpu.regs.a > 0x99)){
			this->cpu.regs.a += 0x60;
			set_c_flag(true);
		}
		if(this->cpu.regs.f.bits.h || ((this->cpu.regs.a & 0x0F) > 0x09)){
			this->cpu.regs.a += 0x06;
		}
	}
	else{
		if(this->cpu.regs.f.bits.c)
			this->cpu.regs.a += 0x60;
		else if(this->cpu.regs.f.bits.h)
			this->cpu.regs.a += 0x06;
	}
	set_h_flag(false);
}
void gbe::general_instructions_t::misc_cpl(){
	set_n_flag(true);
	set_h_flag(true);
	this->cpu.regs.a = ~cpu.regs.a;
}
void gbe::general_instructions_t::misc_ccf(){
	set_n_flag(false);
	set_h_flag(false);
	this->cpu.regs.f.bits.c = !this->cpu.regs.f.bits.c;	//	sets the flag in of itself
}
void gbe::general_instructions_t::misc_scf(){
	set_n_flag(false);
	set_h_flag(false);
	this->cpu.regs.f.bits.c = 1;	//	sets the flag in of itself
}
void gbe::general_instructions_t::misc_nop(){
	return;
}
void gbe::general_instructions_t::misc_halt(){
	//	not sure on how to implement this one yet
}
void gbe::general_instructions_t::misc_stop(){
	//	not sure on how to implement this one yet
}
void gbe::general_instructions_t::misc_di(){
	this->mem.set_interrupt_flag(0);
}
void gbe::general_instructions_t::misc_ei(){
	this->mem.set_interrupt_flag(1);
}

//	Rotates And Shifts
void gbe::general_instructions_t::misc_rlc(byte& reg){
	byte old_b = reg >> 7;
	reset_all_flags();
	set_c_flag(old_b);
	reg <<= 1;
	reg += old_b;
	set_z_flag(reg);
}
void gbe::general_instructions_t::misc_rl(byte& reg){
	byte old_c = this->cpu.regs.f.bits.c;
	reset_all_flags();
	set_c_flag(reg >> 7);
	reg <<= 1;
	reg += old_c;
	set_z_flag(reg);
}
void gbe::general_instructions_t::misc_rrc(byte& reg){
	byte old_c = this->cpu.regs.f.all_bits << 3;	//	keeps only the c flag left in the 7th bit!
	reset_all_flags();
	set_c_flag(reg & 0x01);
	reg >>= 1;
	reg += old_c;
	set_z_flag(reg);
}
void gbe::general_instructions_t::misc_rr(byte& reg){
	byte old_b = reg << 7;
	reset_all_flags();
	set_c_flag(old_b);
	reg >>= 1;
	reg += old_b;
	set_z_flag(reg);
}
void gbe::general_instructions_t::misc_sla(byte& reg){
	reset_all_flags();
	set_c_flag(reg & 0xb1000-0000);
	reg <<= 1;
	set_z_flag(reg);
}
void gbe::general_instructions_t::misc_sra(byte& reg){
	reset_all_flags();
	byte msb = reg & 0b1000-0000;
	set_c_flag(reg & 0x01);
	reg >>= 1;
	reg += msb;
	set_z_flag(reg);
}
void gbe::general_instructions_t::misc_srl(byte& reg){
	reset_all_flags();
	set_c_flag(reg & 0xb0000-0001);
	reg >>= 1;
	set_z_flag(reg);
}

//	Bit opcodes
void gbe::general_instructions_t::b1_bit(){

}
void gbe::general_instructions_t::b1_set(){

}
void gbe::general_instructions_t::b1_res(){

}

//	Jumps
void gbe::general_instructions_t::jp_nn(word immediate){
	this->cpu.regs.pc = immediate;
}
void gbe::general_instructions_t::jp_cc_nn(word immediate, byte flag){
	if(flag)
		this->cpu.regs.pc = immediate;
	
}
void gbe::general_instructions_t::jp_hl(){
	this->cpu.regs.pc = this->cpu.regs.hl;
}
void gbe::general_instructions_t::jr(signed char offset){
	this->cpu.regs.pc += offset;
}
void gbe::general_instructions_t::jr_cc(signed char offset, byte flag){
	if(flag)
		this->cpu.regs.pc += offset;
}

//	Calls
void gbe::general_instructions_t::call_nn(word immediate){
	push_to_stack(this->cpu.regs.pc);
	this->cpu.regs.pc = immediate;
}
void gbe::general_instructions_t::call_cc_nn(word immediate, byte flag){
	if(flag){
		push_to_stack(this->cpu.regs.pc);
		this->cpu.regs.pc = immediate;
	}
}

//	Restarts
void gbe::general_instructions_t::rst(byte offset){
	push_to_stack(this->cpu.regs.pc);
	this->cpu.regs.pc = offset;
}

//	Returns
void gbe::general_instructions_t::ret(){
	this->cpu.regs.pc = pop_from_stack();
}
void gbe::general_instructions_t::ret_cc(byte flag){
	if(flag)
		this->cpu.regs.pc = pop_from_stack();
}
void gbe::general_instructions_t::reti(){
	this->cpu.regs.pc = pop_from_stack();
	this->mem.set_interrupt_flag(1);
}
