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
	this->mem.write_word_to_memory(this->cpu.regs.sp-2, val);
	this->cpu.regs.sp -= 2;
}
word gbe::general_instructions_t::pop_from_stack(){
	word rtrn  = this->mem.read_word_from_memory(this->cpu.regs.sp);
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
void gbe::general_instructions_t::b8_load_adr_reg(word dst, byte src){
	mem.write_byte_to_memory(dst, src);
}

// 16-Bit-Loads
void gbe::general_instructions_t::b16_load_reg_val(word& dst, word src){
	dst = src;
}
void gbe::general_instructions_t::b16_load_reg_adr(word& dst, word src){
	dst = mem.read_word_from_memory(src);
}
void gbe::general_instructions_t::b16_load_adr_sp(word adr){
	mem.write_word_to_memory(adr, cpu.regs.sp);
}
void gbe::general_instructions_t::b16_load_hl_sp_e(signed char val){
	reset_all_flags();
	set_h_flag((cpu.regs.sp&0x0F) + (val&0x0F) > 0x0F);
	set_c_flag((cpu.regs.sp&0xFF) + (byte)val > 0xFF);
	cpu.regs.hl = cpu.regs.sp+val;
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
	set_h_flag((this->cpu.regs.a&0xF) + (val&0xF) > 0x0F);
	set_c_flag(this->cpu.regs.a + val > 0xFF);
	this->cpu.regs.a += val;
	set_z_flag(!this->cpu.regs.a);
}
void gbe::general_instructions_t::b8_adc(byte val){
	byte c = this->cpu.regs.f.bits.c;
	reset_all_flags();
	set_h_flag((this->cpu.regs.a&0x0F) + (val&0x0F) + c > 0x0F);
	set_c_flag(this->cpu.regs.a + val + c > 0xFF);
	this->cpu.regs.a += val + c;
	set_z_flag(!this->cpu.regs.a);
}
void gbe::general_instructions_t::b8_sub(byte val){
	reset_all_flags();
	set_n_flag(true);
	set_h_flag((this->cpu.regs.a&0x0F) < (val&0x0F));
	set_c_flag(this->cpu.regs.a < val);
	this->cpu.regs.a -= val;
	set_z_flag(!this->cpu.regs.a);

}
void gbe::general_instructions_t::b8_sbc(byte val){
	byte c = this->cpu.regs.f.bits.c;
	reset_all_flags();
	set_n_flag(true);
	set_h_flag((this->cpu.regs.a&0x0F) < (val&0x0F) + c);
	set_c_flag(this->cpu.regs.a < val + c);
	this->cpu.regs.a -= val + c;
	set_z_flag(!this->cpu.regs.a);
}
void gbe::general_instructions_t::b8_and(byte val){
	reset_all_flags();
	set_h_flag(true);
	this->cpu.regs.a &= val;
	set_z_flag(!this->cpu.regs.a);
}
void gbe::general_instructions_t::b8_or(byte val){
	reset_all_flags();
	this->cpu.regs.a |= val;
	set_z_flag(!this->cpu.regs.a);
}
void gbe::general_instructions_t::b8_xor(byte val){
	reset_all_flags();
	this->cpu.regs.a ^= val;
	set_z_flag(!this->cpu.regs.a);
}
void gbe::general_instructions_t::b8_cp(byte val){
	set_n_flag(true);
	set_h_flag((this->cpu.regs.a&0x0F) < (val&0x0F));
	set_c_flag(this->cpu.regs.a < val);
	set_z_flag(this->cpu.regs.a == val);
}
void gbe::general_instructions_t::b8_inc(byte& reg){
	set_n_flag(false);
	set_h_flag((reg & 0x0F) == 0x0F);
	set_z_flag(!++reg);
}
void gbe::general_instructions_t::b8_inc_adr(word adr){
	byte val = mem.read_byte_from_memory(adr);
	set_n_flag(false);
	set_h_flag((val & 0x0F) == 0x0F);
	mem.write_word_to_memory(adr, val+1);
	set_z_flag(!(byte)(val+1));
}
void gbe::general_instructions_t::b8_dec(byte& reg){
	set_n_flag(true);
	set_h_flag((reg & 0x0F) < 1);
	set_z_flag(!--reg);
}
void gbe::general_instructions_t::b8_dec_adr(word adr){
	byte val = mem.read_byte_from_memory(adr);
	set_n_flag(true);
	set_h_flag((val&0x0F) < 1);
	mem.write_word_to_memory(adr, val-1);
	set_z_flag(!(byte)(val-1));
}

//	16-Bit-Arithmetic
void gbe::general_instructions_t::b16_add(word val){
	set_n_flag(false);
	set_h_flag((this->cpu.regs.hl&0x0FFF) + (val&0x0FFF) > 0x0FFF);
	set_c_flag(this->cpu.regs.hl + val > 0xFFFF);
	this->cpu.regs.hl += val;
}
void gbe::general_instructions_t::b16_add_sp(signed char val){
	reset_all_flags();
	set_h_flag((this->cpu.regs.sp&0x0F) + (val&0x0F) > 0x0F);
	set_c_flag((this->cpu.regs.sp&0xFF) + (byte)val > 0xFF);
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
	byte upper = (reg&0xF0) >> 4;
	byte lower = (reg&0x0F) << 4;
	reg = lower | upper;
	set_z_flag(!reg);
}
void gbe::general_instructions_t::misc_daa(){
	if(!this->cpu.regs.f.bits.n){
		if(this->cpu.regs.f.bits.c || (this->cpu.regs.a > 0x99)){
			this->cpu.regs.a += 0x60;
			set_c_flag(true);
		}
		if(this->cpu.regs.f.bits.h || (this->cpu.regs.a & 0x0F) > 0x09){
			this->cpu.regs.a += 0x06;
		}
	}
	else{
		if(this->cpu.regs.f.bits.c){
			this->cpu.regs.a -= 0x60;
			set_c_flag(true);
		}
		if(this->cpu.regs.f.bits.h)
			this->cpu.regs.a -= 0x06;
	}
	set_h_flag(false);
	set_z_flag(!this->cpu.regs.a);
}
void gbe::general_instructions_t::misc_cpl(){
	set_n_flag(true);
	set_h_flag(true);
	this->cpu.regs.a = ~this->cpu.regs.a;
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
gbe::branching_t gbe::general_instructions_t::misc_halt(){
	if(cpu.memory.get_interrupt_flag() & cpu.memory.get_interrupt_enable()){
		if(!cpu.memory.get_ime() && !halted){
			auto instr = cpu.cpu_instructions[cpu.memory.read_byte_from_memory(cpu.regs.pc+1)];
			cpu.regs.pc += instr.byte_length;
			cpu.cycles.increment_cycles_t(instr.t_cycles);
		}
		halted = false;
		return branching_t::DO_BRANCH;
	}
	halted = true;
	return branching_t::DO_NOT_BRANCH;
}
gbe::branching_t gbe::general_instructions_t::misc_stop(){
	return branching_t::DO_BRANCH;
}
void gbe::general_instructions_t::misc_di(){
	this->cpu.request_e_interrupt = false;
	this->cpu.request_d_interrupt = true;
}
void gbe::general_instructions_t::misc_ei(){
	this->cpu.request_e_interrupt = true;
	this->cpu.request_d_interrupt = false;
}

//	Rotates And Shifts
void gbe::general_instructions_t::misc_rlc(byte& reg){
	byte old_b = reg >> 7;
	reset_all_flags();
	set_c_flag(old_b);
	reg <<= 1;
	reg += old_b;
	set_z_flag(!reg);
}
void gbe::general_instructions_t::misc_rlca(){
	byte old_b = cpu.regs.a >> 7;
	reset_all_flags();
	set_c_flag(old_b);
	cpu.regs.a <<= 1;
	cpu.regs.a += old_b;
}
void gbe::general_instructions_t::misc_rl(byte& reg){
	byte old_c = this->cpu.regs.f.bits.c;
	reset_all_flags();
	set_c_flag(reg >> 7);
	reg <<= 1;
	reg += old_c;
	set_z_flag(!reg);
}
void gbe::general_instructions_t::misc_rla(){
	byte old_c = this->cpu.regs.f.bits.c;
	reset_all_flags();
	set_c_flag(cpu.regs.a >> 7);
	cpu.regs.a <<= 1;
	cpu.regs.a += old_c;
}
void gbe::general_instructions_t::misc_rrc(byte& reg){
	byte old_b = reg & BIT(0);
	reset_all_flags();
	set_c_flag(old_b);
	reg >>= 1;
	reg += (old_b << 7);
	set_z_flag(!reg);
}
void gbe::general_instructions_t::misc_rrca(){
	byte old_b = cpu.regs.a & BIT(0);
	reset_all_flags();
	set_c_flag(old_b);
	cpu.regs.a >>= 1;
	cpu.regs.a += old_b << 7;
}
void gbe::general_instructions_t::misc_rr(byte& reg){
	byte old_c = this->cpu.regs.f.all_bits << 3;	//	keeps only the c flag left in the 7th bit!
	reset_all_flags();
	set_c_flag(reg & BIT(0));
	reg >>= 1;
	reg += old_c;
	set_z_flag(!reg);
}
void gbe::general_instructions_t::misc_rra(){
	byte old_c = this->cpu.regs.f.all_bits << 3;	//	keeps only the c flag left in the 7th bit!
	reset_all_flags();
	set_c_flag(cpu.regs.a & BIT(0));
	cpu.regs.a >>= 1;
	cpu.regs.a += old_c;
}
void gbe::general_instructions_t::misc_sla(byte& reg){
	reset_all_flags();
	set_c_flag(reg & BIT(7));
	reg <<= 1;
	set_z_flag(!reg);
}
void gbe::general_instructions_t::misc_sra(byte& reg){
	reset_all_flags();
	byte msb = reg & BIT(7);
	set_c_flag(reg & BIT(0));
	reg >>= 1;
	reg += msb;
	set_z_flag(!reg);
}
void gbe::general_instructions_t::misc_srl(byte& reg){
	reset_all_flags();
	set_c_flag(reg & BIT(0));
	reg >>= 1;
	set_z_flag(!reg);
}

//	Bit opcodes
void gbe::general_instructions_t::b1_bit(byte& reg, byte bit){
	set_n_flag(false);
	set_h_flag(true);
	set_z_flag(!((reg >> bit) & 0x01));
}
void gbe::general_instructions_t::b1_set(byte& reg, byte bit){
	reg |= (0x01 << bit);
}
void gbe::general_instructions_t::b1_res(byte& reg, byte bit){
	reg ^= ((0x01 << bit) & reg);
}

//	Jumps
void gbe::general_instructions_t::jp_nn(word immediate){
	this->cpu.regs.pc = immediate;
}
gbe::branching_t gbe::general_instructions_t::jp_cc_nn(word immediate, byte flag){
	if(flag)
		this->cpu.regs.pc = immediate;
	return flag ? gbe::branching_t::DO_BRANCH : gbe::branching_t::DO_NOT_BRANCH;
}
void gbe::general_instructions_t::jp_hl(){
	this->cpu.regs.pc = this->cpu.regs.hl;
}
void gbe::general_instructions_t::jr(signed char offset){
	this->cpu.regs.pc += offset;
}
gbe::branching_t gbe::general_instructions_t::jr_cc(signed char offset, byte flag){
	if(flag)
		this->cpu.regs.pc += offset;
	return flag ? gbe::branching_t::DO_BRANCH : gbe::branching_t::DO_NOT_BRANCH;
}

//	Calls
void gbe::general_instructions_t::call_nn(word immediate){
	push_to_stack(this->cpu.regs.pc+3);
	this->cpu.regs.pc = immediate;
}
gbe::branching_t gbe::general_instructions_t::call_cc_nn(word immediate, byte flag){
	if(flag){
		push_to_stack(this->cpu.regs.pc+3);
		this->cpu.regs.pc = immediate;
	}
	return flag ? gbe::branching_t::DO_BRANCH : gbe::branching_t::DO_NOT_BRANCH;
}

//	Restarts
void gbe::general_instructions_t::rst(byte offset){
	push_to_stack(this->cpu.regs.pc+1);
	this->cpu.regs.pc = offset;
}

//	Returns
void gbe::general_instructions_t::ret(){
	this->cpu.regs.pc = pop_from_stack();
}
gbe::branching_t gbe::general_instructions_t::ret_cc(byte flag){
	if(flag)
		this->cpu.regs.pc = pop_from_stack();
	return flag ? gbe::branching_t::DO_BRANCH : gbe::branching_t::DO_NOT_BRANCH;
}
void gbe::general_instructions_t::reti(){
	this->cpu.regs.pc = pop_from_stack();
	this->cpu.request_e_interrupt = true;
	this->cpu.request_d_interrupt = false;
}
