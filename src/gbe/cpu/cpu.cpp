#include"cpu.h"
#include<iostream>
#include<fstream>
#define INSTR [](gbe::cpu_t& cpu) -> bool

void gbe::cpu_t::load_ROM(const char* file){
	this->memory.load_ROM(file);
	print_rom_info();
	this->memory.mount_boot_rom();
}
void gbe::cpu_t::print_rom_info(){
	printf("rom type: ");
	int cartridge_mode = this->memory.get_rom_info().mode;
	switch(cartridge_mode)
	{
	case 0:
		printf("none");
		break;
	case 0x1:
	case 0x2:
	case 0x3:
		printf("mbc1");
		break;
	case 0x5:
	case 0x6:
		printf("mbc2");
		break;
	default:
		printf("N/A");
		break;
	}
	printf("\n");
	printf("rom size: %d\n", this->memory.get_rom_info().rom_size);
	printf("ram size: %d\n", this->memory.get_rom_info().ram_size);
}

void gbe::cpu_t::emulate_fetch_decode_execute_cycle(){
	//print_regs();
	//flush_print_stream_to_console_and_empty();
	check_interrupt_status();
	byte instr_index = this->memory.read_byte_from_memory(this->regs.pc);
	auto instr = cpu_instructions[instr_index];
	if(this->memory.get_boot_rom_mount_status() && this->regs.pc >= 0x100){
		this->memory.unmount_boot_rom();
		printf("unmounting boot rom\n");
	}
	if(this->request_handle){
		if(this->request_e_interrupt){
			memory.enable_ime();
			this->request_e_interrupt = false;
		}else{
			memory.disable_ime();
			this->request_d_interrupt = false;
		}
		this->request_handle = false;
	}
	if(this->request_d_interrupt | this->request_e_interrupt)
		this->request_handle = true;
	byte diff = this->cycles.t_cycles;
	if(instr.func(*this))
		this->regs.pc += instr.byte_length;
	diff = this->cycles.t_cycles - diff;
	check_dma_status(diff);
	this->cycles.increment_cycles_t(instr.t_cycles+diff);
	this->memory.increment_timer(this->cycles.t_cycles);
	this->ppu.update(instr.t_cycles+diff);
}

void gbe::cpu_t::check_interrupt_status(){
	//	the interrupts are ordered after priority. That when two or more interupts are enabled witin the interrupt flag,
	//	it goes from the interrupt value at the 0th bit to the last bit in terms of priority. So the interrupt found in
	//	bit zero should have priority over one found in bit 1, 2 or anything above 0. Same logic goes for all the other bits
	//	in the flag.
	int reg_flag = this->memory.get_interrupt_flag();
	int reg_enable = this->memory.get_interrupt_enable();
	if(this->memory.get_ime() && (reg_flag & reg_enable & 0b0001'1111)){
		instructions.b16_push(regs.pc);
		if(reg_flag & reg_enable & BIT(0)){			//	V-Blank; INT 40
			regs.pc = 0x40;
			memory.set_interrupt_flag(reg_flag^BIT(0));
		}
		else if(reg_flag & reg_enable & BIT(1)){	//	LCD STAT; INT 48;
			regs.pc = 0x48;
			memory.set_interrupt_flag(reg_flag^BIT(1));
		}
		else if(reg_flag & reg_enable & BIT(2)){	//	Timer; INT 50
			regs.pc = 0x50;
			memory.set_interrupt_flag(reg_flag^BIT(2));
		}
		else if(reg_flag & reg_enable & BIT(3)){	//	Serial; INT 58
			regs.pc = 0x58;
			memory.set_interrupt_flag(reg_flag^BIT(3));
		}
		else if(reg_flag & reg_enable & BIT(4)){	//	Joypad; INT 60
			regs.pc = 0x60;
			memory.set_interrupt_flag(reg_flag^BIT(4));
		}
		memory.disable_ime();
	}
}

#define INSTRUCTION cpu.instructions
#define LOAD_IMMEDIATE_BYTE cpu.memory.read_byte_from_memory(cpu.regs.pc+1)
#define LOAD_IMMEDIATE_WORD cpu.memory.read_word_from_memory(cpu.regs.pc+1)
#define R_A cpu.regs.a
#define R_B cpu.regs.b
#define R_C cpu.regs.c
#define R_D cpu.regs.d
#define R_E cpu.regs.e
#define R_F cpu.regs.f
#define R_H cpu.regs.h
#define R_L cpu.regs.l 
#define R_BC cpu.regs.bc
#define R_DE cpu.regs.de
#define R_HL cpu.regs.hl
#define R_SP cpu.regs.sp
#define R_PC cpu.regs.pc
#define R_AF cpu.regs.af
gbe::cpu_t::instruction_t gbe::cpu_t::cpu_instructions[]{	//	returns if pc should increment(true) or not(false)
	{"NOP", 1, 4, INSTR{
		INSTRUCTION.misc_nop();
		return true;
	}},
	{"LD BC, u16", 3, 12, INSTR{
		INSTRUCTION.b16_load_reg_val(R_BC, LOAD_IMMEDIATE_WORD);
		return true;
	}},
	{"LD (BC), A", 1, 8, INSTR{
		INSTRUCTION.b8_load_adr_reg(R_BC, R_A);
		return true;
	}},
	{"INC BC", 1, 8, INSTR{
		INSTRUCTION.b16_inc(R_BC);
		return true;
	}},
	{"INC B", 1, 4, INSTR{
		INSTRUCTION.b8_inc(R_B);
		return true;
	}},
	{"DEC B", 1, 4, INSTR{ 
		INSTRUCTION.b8_dec(R_B);
		return true;
	}},
	{"LD B, u8", 2, 8, INSTR{
		INSTRUCTION.b8_load_reg_val(R_B, LOAD_IMMEDIATE_BYTE);
		return true;
	}},
	{"RLCA", 1, 4, INSTR{
		INSTRUCTION.misc_rlca();
		return true;
	}},
	{"LD (u16), SP", 3, 20, INSTR{
		INSTRUCTION.b16_load_adr_sp(LOAD_IMMEDIATE_WORD);
		return true;
	}},
	{"ADD HL, BC", 1, 8, INSTR{
		INSTRUCTION.b16_add(R_BC);
		return true;
	}},
	{"LD A, (BC)", 1, 8, INSTR{
		INSTRUCTION.b8_load_reg_adr(R_A, R_BC);
		return true;
	}},
	{"DEC BC", 1, 8, INSTR{
		INSTRUCTION.b16_dec(R_BC);
		return true;
	}},
	{"INC C", 1, 4, INSTR{
		INSTRUCTION.b8_inc(R_C);
		return true;
	}},
	{"DEC C", 1, 4, INSTR{
		INSTRUCTION.b8_dec(R_C);
		return true;
	}},
	{"LD C, u8", 2, 8, INSTR{
		INSTRUCTION.b8_load_reg_val(R_C, LOAD_IMMEDIATE_BYTE);
		return true;
	}},
	{"RRCA", 1, 4, INSTR{
		INSTRUCTION.misc_rrca();
		return true;
	}},
	{"STOP", 2, 4, INSTR{
		if(INSTRUCTION.misc_stop() == branching_t::DO_BRANCH)
			return true;
		return false;	//	returns false and waits for button press
	}},
	{"LD DE, u16", 3, 12, INSTR{
		INSTRUCTION.b16_load_reg_val(R_DE, LOAD_IMMEDIATE_WORD);
		return true;
	}},
	{"LD (DE), A", 1, 8, INSTR{
		INSTRUCTION.b8_load_adr_reg(R_DE, R_A);
		return true;
	}},
	{"INC DE", 1, 8, INSTR{
		INSTRUCTION.b16_inc(R_DE);
		return true;
	}},
	{"INC D", 1, 4, INSTR{
		INSTRUCTION.b8_inc(R_D);
		return true;
	}},
	{"DEC D", 1, 4, INSTR{
		INSTRUCTION.b8_dec(R_D);
		return true;
	}},
	{"LD D, u8", 2, 8, INSTR{
		INSTRUCTION.b8_load_reg_val(R_D, LOAD_IMMEDIATE_BYTE);
		return true;
	}},
	{"RLA", 1, 4, INSTR{
		INSTRUCTION.misc_rla();
		return true;
	}},
	{"JR i8", 2, 12, INSTR{
		INSTRUCTION.jr(LOAD_IMMEDIATE_BYTE);
		return true;
	}},
	{"ADD HL, DE", 1, 8, INSTR{
		INSTRUCTION.b16_add(R_DE);
		return true;
	}},
	{"LD A, (DE)", 1, 8, INSTR{
		INSTRUCTION.b8_load_reg_adr(R_A, R_DE);
		return true;
	}},
	{"DEC DE", 1, 8, INSTR{
		INSTRUCTION.b16_dec(R_DE);
		return true;
	}},
	{"INC E", 1, 4, INSTR{
		INSTRUCTION.b8_inc(R_E);
		return true;
	}},
	{"DEC E", 1, 4, INSTR{
		INSTRUCTION.b8_dec(R_E);
		return true;
	}},
	{"LD E, u8", 2, 8, INSTR{
		INSTRUCTION.b8_load_reg_val(R_E, LOAD_IMMEDIATE_BYTE);
		return true;
	}},
	{"RRA", 1, 4, INSTR{
		INSTRUCTION.misc_rra();
		return true;
	}},
	{"JR NZ, i8", 2, 8, INSTR{
		if(INSTRUCTION.jr_cc(LOAD_IMMEDIATE_BYTE, !R_F.bits.z) == branching_t::DO_BRANCH){
			cpu.cycles.increment_cycles_m(1);
		}
		return true;
	}},
	{"LD HL, u16", 3, 12, INSTR{
		INSTRUCTION.b16_load_reg_val(R_HL, LOAD_IMMEDIATE_WORD);
		return true;
	}},
	{"LD (HL+), A", 1, 8, INSTR{
		INSTRUCTION.b8_load_adr_reg(R_HL++, R_A);
		return true;
	}},
	{"INC HL", 1, 8, INSTR{
		INSTRUCTION.b16_inc(R_HL);
		return true;
	}},
	{"INC H", 1, 4, INSTR{
		INSTRUCTION.b8_inc(R_H);
		return true;
	}},
	{"DEC H", 1, 4, INSTR{
		INSTRUCTION.b8_dec(R_H);
		return true;
	}},
	{"LD H, u8", 2, 8, INSTR{
		INSTRUCTION.b8_load_reg_val(R_H, LOAD_IMMEDIATE_BYTE);
		return true;
	}},
	{"DAA", 1, 4, INSTR{
		INSTRUCTION.misc_daa();
		return true;
	}},
	{"JR Z, i8", 2, 8, INSTR{
		if(INSTRUCTION.jr_cc(LOAD_IMMEDIATE_BYTE, R_F.bits.z) == branching_t::DO_BRANCH){
			cpu.cycles.increment_cycles_m(1);
		}
		return true;
	}},
	{"ADD HL, HL", 1, 8, INSTR{
		INSTRUCTION.b16_add(R_HL);
		return true;
	}},
	{"LD A, (HL+)", 1, 8, INSTR{
		INSTRUCTION.b8_load_reg_adr(R_A, R_HL++);
		return true;
	}},
	{"DEC HL", 1, 8, INSTR{
		INSTRUCTION.b16_dec(R_HL);
		return true;
	}},
	{"INC L", 1, 4, INSTR{
		INSTRUCTION.b8_inc(R_L);
		return true;
	}},
	{"DEC L", 1, 4, INSTR{
		INSTRUCTION.b8_dec(R_L);
		return true;
	}},
	{"LD L, u8", 2, 8, INSTR{
		INSTRUCTION.b8_load_reg_val(R_L, LOAD_IMMEDIATE_BYTE);
		return true;
	}},
	{"CPL", 1, 4, INSTR{
		INSTRUCTION.misc_cpl();
		return true;
	}},
	{"JR NC, i8", 2, 8, INSTR{
		if(INSTRUCTION.jr_cc(LOAD_IMMEDIATE_BYTE, !R_F.bits.c) == branching_t::DO_BRANCH){
			cpu.cycles.increment_cycles_m(1);
		}
		return true;
	}},
	{"LD SP, u16", 3, 12, INSTR{
		INSTRUCTION.b16_load_reg_val(R_SP, LOAD_IMMEDIATE_WORD);
		return true;
	}},
	{"LD (HL-), A", 1, 8, INSTR{
		INSTRUCTION.b8_load_adr_reg(R_HL--, R_A);
		return true;
	}},
	{"INC SP", 1, 8, INSTR{
		INSTRUCTION.b16_inc(R_SP);
		return true;
	}},
	{"INC (HL)", 1, 12, INSTR{
		INSTRUCTION.b8_inc_adr(R_HL);
		return true;
	}},
	{"DEC (HL)", 1, 4, INSTR{
		INSTRUCTION.b8_dec_adr(R_HL);
		return true;
	}},
	{"LD (HL), u8", 2, 12, INSTR{
		INSTRUCTION.b8_load_adr_reg(R_HL, LOAD_IMMEDIATE_BYTE);
		return true;
	}},
	{"SCF", 1, 4, INSTR{
		INSTRUCTION.misc_scf();
		return true;
	}},
	{"JR C, i8", 2, 8, INSTR{
		if(INSTRUCTION.jr_cc(LOAD_IMMEDIATE_BYTE, R_F.bits.c) == branching_t::DO_BRANCH){
			cpu.cycles.increment_cycles_m(1);
		}
		return true;
	}},
	{"ADD HL, SP", 1, 8, INSTR{
		INSTRUCTION.b16_add(R_SP);
		return true;
	}},
	{"LD A, (HL-)", 1, 8, INSTR{
		INSTRUCTION.b8_load_reg_adr(R_A, R_HL--);
		return true;
	}},
	{"DEC SP", 1, 8, INSTR{
		INSTRUCTION.b16_dec(R_SP);
		return true;
	}},
	{"INC A", 1, 4, INSTR{
		INSTRUCTION.b8_inc(R_A);
		return true;
	}},
	{"DEC A", 1, 4, INSTR{
		INSTRUCTION.b8_dec(R_A);
		return true;
	}},
	{"LD A, u8", 2, 8, INSTR{
		INSTRUCTION.b8_load_reg_val(R_A, LOAD_IMMEDIATE_BYTE);
		return true;
	}},
	{"CCF", 1, 4, INSTR{
		INSTRUCTION.misc_ccf();
		return true;
	}},
	{"LD B, B", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_B, R_B);
		return true;
	}},
	{"LD B, C", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_B, R_C);
		return true;
	}},
	{"LD B, D", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_B, R_D);
		return true;
	}},
	{"LD B, E", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_B, R_E);
		return true;
	}},
	{"LD B, H", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_B, R_H);
		return true;
	}},
	{"LD B, L", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_B, R_L);
		return true;
	}},
	{"LD B, (HL)", 1, 8, INSTR{
		INSTRUCTION.b8_load_reg_adr(R_B, R_HL);
		return true;
	}},
	{"LD B, A", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_B, R_A);
		return true;
	}},
	{"LD C, B", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_C, R_B);
		return true;
	}},
	{"LD C, C", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_C, R_C);
		return true;
	}},
	{"LD C, D", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_C, R_D);
		return true;
	}},
	{"LD C, E", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_C, R_E);
		return true;
	}},
	{"LD C, H", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_C, R_H);
		return true;
	}},
	{"LD C, L", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_C, R_L);
		return true;
	}},
	{"LD C, (HL)", 1, 8, INSTR{
		INSTRUCTION.b8_load_reg_adr(R_C, R_HL);
		return true;
	}},
	{"LD C, A", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_C, R_A);
		return true;
	}},
	{"LD D, B", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_D, R_B);
		return true;
	}},
	{"LD D, C", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_D, R_C);
		return true;
	}},
	{"LD D, D", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_D, R_D);
		return true;
	}},
	{"LD D, E", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_D, R_E);
		return true;
	}},
	{"LD D, H", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_D, R_H);
		return true;
	}},
	{"LD D, L", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_D, R_L);
		return true;
	}},
	{"LD D, (HL)", 1, 8, INSTR{
		INSTRUCTION.b8_load_reg_adr(R_D, R_HL);
		return true;
	}},
	{"LD D, A", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_D, R_A);
		return true;
	}},
	{"LD E, B", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_E, R_B);
		return true;
	}},
	{"LD E, C", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_E, R_C);
		return true;
	}},
	{"LD E, D", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_E, R_D);
		return true;
	}},
	{"LD E, E", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_E, R_E);
		return true;
	}},
	{"LD E, H", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_E, R_H);
		return true;
	}},
	{"LD E, L", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_E, R_L);
		return true;
	}},
	{"LD E, (HL)", 1, 8, INSTR{
		INSTRUCTION.b8_load_reg_adr(R_E, R_HL);
		return true;
	}},
	{"LD E, A", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_E, R_A);
		return true;
	}},
	{"LD H, B", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_H, R_B);
		return true;
	}},
	{"LD H, C", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_H, R_C);
		return true;
	}},
	{"LD H, D", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_H, R_D);
		return true;
	}},
	{"LD H, E", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_H, R_E);
		return true;
	}},
	{"LD H, H", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_H, R_H);
		return true;
	}},
	{"LD H, L", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_H, R_L);
		return true;
	}},
	{"LD H, (HL)", 1, 8, INSTR{
		INSTRUCTION.b8_load_reg_adr(R_H, R_HL);
		return true;
	}},
	{"LD H, A", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_H, R_A);
		return true;
	}},
	{"LD L, B", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_L, R_B);
		return true;
	}},
	{"LD L, C", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_L, R_C);
		return true;
	}},
	{"LD L, D", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_L, R_D);
		return true;
	}},
	{"LD L, E", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_L, R_E);
		return true;
	}},
	{"LD L, H", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_L, R_H);
		return true;
	}},
	{"LD L, L", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_L, R_L);
		return true;
	}},
	{"LD L, (HL)", 1, 8, INSTR{
		INSTRUCTION.b8_load_reg_adr(R_L, R_HL);
		return true;
	}},
	{"LD L, A", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_L, R_A);
		return true;
	}},
	{"LD (HL), B", 1, 8, INSTR{
		INSTRUCTION.b8_load_adr_reg(R_HL, R_B);
		return true;
	}},
	{"LD (HL), C", 1, 8, INSTR{
		INSTRUCTION.b8_load_adr_reg(R_HL, R_C);
		return true;
	}},
	{"LD (HL), D", 1, 8, INSTR{
		INSTRUCTION.b8_load_adr_reg(R_HL, R_D);
		return true;
	}},
	{"LD (HL), E", 1, 8, INSTR{
		INSTRUCTION.b8_load_adr_reg(R_HL, R_E);
		return true;
	}},
	{"LD (HL), H", 1, 8, INSTR{
		INSTRUCTION.b8_load_adr_reg(R_HL, R_H);
		return true;
	}},
	{"LD (HL), L", 1, 8, INSTR{
		INSTRUCTION.b8_load_adr_reg(R_HL, R_L);
		return true;
	}},
	{"HALT", 1, 4, INSTR{
		if(INSTRUCTION.misc_halt() == branching_t::DO_BRANCH)
			return true;
		return false;	//	halts until any interrupt
	}},
	{"LD (HL), A", 1, 8, INSTR{
		INSTRUCTION.b8_load_adr_reg(R_HL, R_A);
		return true;
	}},
	{"LD A, B", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_A, R_B);
		return true;
	}},
	{"LD A, C", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_A, R_C);
		return true;
	}},
	{"LD A, D", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_A, R_D);
		return true;
	}},
	{"LD A, E", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_A, R_E);
		return true;
	}},
	{"LD A, H", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_A, R_H);
		return true;
	}},
	{"LD A, L", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_A, R_L);
		return true;
	}},
	{"LD A, (HL)", 1, 8, INSTR{
		INSTRUCTION.b8_load_reg_adr(R_A, R_HL);
		return true;
	}},
	{"LD A, A", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_A, R_A);
		return true;
	}},
	{"ADD A, B", 1, 4, INSTR{
		INSTRUCTION.b8_add(R_B);
		return true;
	}},
	{"ADD A, C", 1, 4, INSTR{
		INSTRUCTION.b8_add(R_C);
		return true;
	}},
	{"ADD A, D", 1, 4, INSTR{
		INSTRUCTION.b8_add(R_D);
		return true;
	}},
	{"ADD A, E", 1, 4, INSTR{
		INSTRUCTION.b8_add(R_E);
		return true;
	}},
	{"ADD A, H", 1, 4, INSTR{
		INSTRUCTION.b8_add(R_H);
		return true;
	}},
	{"ADD A, L", 1, 4, INSTR{
		INSTRUCTION.b8_add(R_L);
		return true;
	}},
	{"ADD A, (HL)", 1, 8, INSTR{
		INSTRUCTION.b8_add(cpu.memory.read_byte_from_memory(R_HL));
		return true;
	}},
	{"ADD A, A", 1, 4, INSTR{
		INSTRUCTION.b8_add(R_A);
		return true;
	}},
	{"ADC A, B", 1, 4, INSTR{
		INSTRUCTION.b8_adc(R_B);
		return true;
	}},
	{"ADC A, C", 1, 4, INSTR{
		INSTRUCTION.b8_adc(R_C);
		return true;
	}},
	{"ADC A, D", 1, 4, INSTR{
		INSTRUCTION.b8_adc(R_D);
		return true;
	}},
	{"ADC A, E", 1, 4, INSTR{
		INSTRUCTION.b8_adc(R_E);
		return true;
	}},
	{"ADC A, H", 1, 4, INSTR{
		INSTRUCTION.b8_adc(R_H);
		return true;
	}},
	{"ADC A, L", 1, 4, INSTR{
		INSTRUCTION.b8_adc(R_L);
		return true;
	}},
	{"ADC A, (HL)", 1, 8, INSTR{
		INSTRUCTION.b8_adc(cpu.memory.read_byte_from_memory(R_HL));
		return true;
	}},
	{"ADC A, A", 1, 4, INSTR{
		INSTRUCTION.b8_adc(R_A);
		return true;
	}},
	{"SUB A, B", 1, 4, INSTR{
		INSTRUCTION.b8_sub(R_B);
		return true;
	}},
	{"SUB A, C", 1, 4, INSTR{
		INSTRUCTION.b8_sub(R_C);
		return true;
	}},
	{"SUB A, D", 1, 4, INSTR{
		INSTRUCTION.b8_sub(R_D);
		return true;
	}},
	{"SUB A, E", 1, 4, INSTR{
		INSTRUCTION.b8_sub(R_E);
		return true;
	}},
	{"SUB A, H", 1, 4, INSTR{
		INSTRUCTION.b8_sub(R_H);
		return true;
	}},
	{"SUB A, L", 1, 4, INSTR{
		INSTRUCTION.b8_sub(R_L);
		return true;
	}},
	{"SUB A, (HL)", 1, 8, INSTR{
		INSTRUCTION.b8_sub(cpu.memory.read_byte_from_memory(R_HL));
		return true;
	}},
	{"SUB A, A", 1, 4, INSTR{
		INSTRUCTION.b8_sub(R_A);
		return true;
	}},
	{"SBC A, B", 1, 4, INSTR{
		INSTRUCTION.b8_sbc(R_B);
		return true;
	}},
	{"SBC A, C", 1, 4, INSTR{
		INSTRUCTION.b8_sbc(R_C);
		return true;
	}},
	{"SBC A, D", 1, 4, INSTR{
		INSTRUCTION.b8_sbc(R_D);
		return true;
	}},
	{"SBC A, E", 1, 4, INSTR{
		INSTRUCTION.b8_sbc(R_E);
		return true;
	}},
	{"SBC A, H", 1, 4, INSTR{
		INSTRUCTION.b8_sbc(R_H);
		return true;
	}},
	{"SBC A, L", 1, 4, INSTR{
		INSTRUCTION.b8_sbc(R_L);
		return true;
	}},
	{"SBC A, (HL)", 1, 8, INSTR{
		INSTRUCTION.b8_sbc(cpu.memory.read_byte_from_memory(R_HL));
		return true;
	}},
	{"SBC A, A", 1, 4, INSTR{
		INSTRUCTION.b8_sbc(R_A);
		return true;
	}},
	{"AND A, B", 1, 4, INSTR{
		INSTRUCTION.b8_and(R_B);
		return true;
	}},
	{"AND A, C", 1, 4, INSTR{
		INSTRUCTION.b8_and(R_C);
		return true;
	}},
	{"AND A, D", 1, 4, INSTR{
		INSTRUCTION.b8_and(R_D);
		return true;
	}},
	{"AND A, E", 1, 4, INSTR{
		INSTRUCTION.b8_and(R_E);
		return true;
	}},
	{"AND A, H", 1, 4, INSTR{
		INSTRUCTION.b8_and(R_H);
		return true;
	}},
	{"AND A, L", 1, 4, INSTR{
		INSTRUCTION.b8_and(R_L);
		return true;
	}},
	{"AND A, (HL)", 1, 8, INSTR{
		INSTRUCTION.b8_and(cpu.memory.read_byte_from_memory(R_HL));
		return true;
	}},
	{"AND A, A", 1, 4, INSTR{
		INSTRUCTION.b8_and(R_A);
		return true;
	}},
	{"XOR A, B", 1, 4, INSTR{
		INSTRUCTION.b8_xor(R_B);
		return true;
	}},
	{"XOR A, C", 1, 4, INSTR{
		INSTRUCTION.b8_xor(R_C);
		return true;
	}},
	{"XOR A, D", 1, 4, INSTR{
		INSTRUCTION.b8_xor(R_D);
		return true;
	}},
	{"XOR A, E", 1, 4, INSTR{
		INSTRUCTION.b8_xor(R_E);
		return true;
	}},
	{"XOR A, H", 1, 4, INSTR{
		INSTRUCTION.b8_xor(R_H);
		return true;
	}},
	{"XOR A, L", 1, 4, INSTR{
		INSTRUCTION.b8_xor(R_L);
		return true;
	}},
	{"XOR A, (HL)", 1, 8, INSTR{
		INSTRUCTION.b8_xor(cpu.memory.read_byte_from_memory(R_HL));
		return true;
	}},
	{"XOR A, A", 1, 4, INSTR{
		INSTRUCTION.b8_xor(R_A);
		return true;
	}},
	{"OR A, B", 1, 4, INSTR{
		INSTRUCTION.b8_or(R_B);
		return true;
	}},
	{"OR A, C", 1, 4, INSTR{
		INSTRUCTION.b8_or(R_C);
		return true;
	}},
	{"OR A, D", 1, 4, INSTR{
		INSTRUCTION.b8_or(R_D);
		return true;
	}},
	{"OR A, E", 1, 4, INSTR{
		INSTRUCTION.b8_or(R_E);
		return true;
	}},
	{"OR A, H", 1, 4, INSTR{
		INSTRUCTION.b8_or(R_H);
		return true;
	}},
	{"OR A, L", 1, 4, INSTR{
		INSTRUCTION.b8_or(R_L);
		return true;
	}},
	{"OR A, (HL)", 1, 8, INSTR{
		INSTRUCTION.b8_or(cpu.memory.read_byte_from_memory(R_HL));
		return true;
	}},
	{"OR A, A", 1, 4, INSTR{
		INSTRUCTION.b8_or(R_A);
		return true;
	}},
	{"CP A, B", 1, 4, INSTR{
		INSTRUCTION.b8_cp(R_B);
		return true;
	}},
	{"CP A, C", 1, 4, INSTR{
		INSTRUCTION.b8_cp(R_C);
		return true;
	}},
	{"CP A, D", 1, 4, INSTR{
		INSTRUCTION.b8_cp(R_D);
		return true;
	}},
	{"CP A, E", 1, 4, INSTR{
		INSTRUCTION.b8_cp(R_E);
		return true;
	}},
	{"CP A, H", 1, 4, INSTR{
		INSTRUCTION.b8_cp(R_H);
		return true;
	}},
	{"CP A, L", 1, 4, INSTR{
		INSTRUCTION.b8_cp(R_L);
		return true;
	}},
	{"CP A, (HL)", 1, 8, INSTR{
		INSTRUCTION.b8_cp(cpu.memory.read_byte_from_memory(R_HL));
		return true;
	}},
	{"CP A, A", 1, 4, INSTR{
		INSTRUCTION.b8_cp(R_A);
		return true;
	}},
	{"RET NZ", 1, 8, INSTR{
		if(INSTRUCTION.ret_cc(!R_F.bits.z) == gbe::branching_t::DO_BRANCH){
			cpu.cycles.increment_cycles_m(3);
			return false;
		}
		return true;
	}},
	{"POP BC", 1, 12, INSTR{
		INSTRUCTION.b16_pop(R_BC);
		return true;
	}},
	{"JP NZ, u16", 3, 12, INSTR{
		if(INSTRUCTION.jp_cc_nn(LOAD_IMMEDIATE_WORD, !R_F.bits.z) == branching_t::DO_BRANCH){
			cpu.cycles.increment_cycles_m(1);
			return false;
		}
		return true;
	}},
	{"JP u16", 3, 16, INSTR{
		INSTRUCTION.jp_nn(LOAD_IMMEDIATE_WORD);
		return false;
	}},
	{"CALL NZ, u16", 3, 12, INSTR{
		if(INSTRUCTION.call_cc_nn(LOAD_IMMEDIATE_WORD, !R_F.bits.z) == branching_t::DO_BRANCH){
			cpu.cycles.increment_cycles_m(3);
			return false;
		}
		return true;
	}},
	{"PUSH BC", 1, 16, INSTR{
		INSTRUCTION.b16_push(R_BC);
		return true;
	}},
	{"ADD A, u8", 2, 8, INSTR{
		INSTRUCTION.b8_add(LOAD_IMMEDIATE_BYTE);
		return true;
	}},
	{"RST 00h", 1, 16, INSTR{
		INSTRUCTION.rst(0x00);
		return false;
	}},
	{"RET Z", 1, 8, INSTR{
		if(INSTRUCTION.ret_cc(R_F.bits.z) == branching_t::DO_BRANCH){
			cpu.cycles.increment_cycles_m(3);
			return false;
		}
		return true;
	}},
	{"RET", 1, 16, INSTR{
		INSTRUCTION.ret();
		return false;
	}},
	{"JP Z, u16", 3, 12, INSTR{
		if(INSTRUCTION.jp_cc_nn(LOAD_IMMEDIATE_WORD, R_F.bits.z) == branching_t::DO_BRANCH){
			cpu.cycles.increment_cycles_m(1);
			return false;
		}
		return true;
	}},
	{"PREFIX CB", 2, 8, INSTR{	//	the prefix instruction itself is only a byte and 4 t-cycles.
		int next_instr = LOAD_IMMEDIATE_BYTE;	//	gets opcode.
		if(next_instr & 0x0F == 6 || next_instr & 0x0F == 0x0E){
			cpu.cycles.increment_cycles_m(1);	//	special cases that take 12 cycles all end with a lower nibble of 6 or E
		}
		byte* reg_arg;		//	stores which register to use in instruction.
		byte bit_arg = 0; 	//	stores which bit to operate on.
		byte mem_tmp;
		int next_instr_high = (next_instr & 0xF0) >> 4;
		int next_instr_low = next_instr & 0x0F;
		bool memory_reading = false;
		switch(next_instr_low){	//	sets the register to use based on the lower nibble of the opcode.
		case 0x00:
		case 0x08:
			reg_arg = &cpu.regs.b;
			break;	
		case 0x01:
		case 0x09:
			reg_arg = &cpu.regs.c;
			break;
		case 0x02:
		case 0x0A:
			reg_arg = &cpu.regs.d;
			break;
		case 0x03:
		case 0x0B:
			reg_arg = &cpu.regs.e;
			break;
		case 0x04:
		case 0x0C:
			reg_arg = &cpu.regs.h;
			break;
		case 0x05:
		case 0x0D:
			reg_arg = &cpu.regs.l;
			break;
		case 0x06:
		case 0x0E:
			mem_tmp = byte{cpu.memory.read_byte_from_memory(cpu.regs.hl)};
			reg_arg = &mem_tmp;
			memory_reading = true;
			break;
		case 0x07:
		case 0x0F:
			reg_arg = &cpu.regs.a;
			break;
		}
		//	iterates over every value of the higher nibble of the value that determines the next instruction.
		if(!next_instr_high){	//	RLC/RRC
			if(next_instr_low < 0x08)
				INSTRUCTION.misc_rlc(*reg_arg);
			else
				INSTRUCTION.misc_rrc(*reg_arg);
		} else if(next_instr_high == 0x01){	//	RL/RR
			if(next_instr_low < 0x08)
				INSTRUCTION.misc_rl(*reg_arg);
			else
				INSTRUCTION.misc_rr(*reg_arg);
		} else if(next_instr_high == 0x02){	//	SLA/SRA
			if(next_instr_low < 0x08)
				INSTRUCTION.misc_sla(*reg_arg);
			else 
				INSTRUCTION.misc_sra(*reg_arg);
		} else if(next_instr_high == 0x03){	//	SWAP/SRL
			if(next_instr_low < 0x08)
				INSTRUCTION.misc_swap(*reg_arg);
			else
				INSTRUCTION.misc_srl(*reg_arg);
		} else if(next_instr_high >= 0x04 && next_instr_high <= 0x07){	//	BIT
			bit_arg = (next_instr - 0x40) / 8;
			INSTRUCTION.b1_bit(*reg_arg, bit_arg);
		} else if(next_instr_high >= 0x08 && next_instr_high <= 0x0B){	//	RES
			bit_arg = (next_instr - 0x80) / 8;
			INSTRUCTION.b1_res(*reg_arg, bit_arg);
		} else if(next_instr_high >= 0x0C){	//	SET
			bit_arg = (next_instr - 0xC0) / 8;
			INSTRUCTION.b1_set(*reg_arg, bit_arg);
		}
		if(memory_reading){
			cpu.memory.write_byte_to_memory(cpu.regs.hl, *reg_arg);
		}
		return true;
	}},
	{"CALL Z, u16", 3, 12, INSTR{
		if(INSTRUCTION.call_cc_nn(LOAD_IMMEDIATE_WORD, R_F.bits.z) == branching_t::DO_BRANCH){
			cpu.cycles.increment_cycles_m(3);
			return false;
		}
		return true;
	}},
	{"CALL u16", 3, 24, INSTR{
		INSTRUCTION.call_nn(LOAD_IMMEDIATE_WORD);
		return false;
	}},
	{"ADC A, u8", 2, 8, INSTR{
		INSTRUCTION.b8_adc(LOAD_IMMEDIATE_BYTE);
		return true;
	}},
	{"RST 08h", 1, 16, INSTR{
		INSTRUCTION.rst(0x08);
		return false;
	}},
	{"RET NC", 1, 8, INSTR{
		if(INSTRUCTION.ret_cc(!R_F.bits.c) == branching_t::DO_BRANCH){
			cpu.cycles.increment_cycles_m(3);
			return false;
		}
		return true;
	}},
	{"POP DE", 1, 12, INSTR{
		INSTRUCTION.b16_pop(R_DE);
		return true;
	}},
	{"JP NC, u16", 3, 12, INSTR{
		if(INSTRUCTION.jp_cc_nn(LOAD_IMMEDIATE_WORD, !R_F.bits.c) == branching_t::DO_BRANCH){
			cpu.cycles.increment_cycles_m(1);
			return false;
		}
		return true;
	}},
	{"N/A", 0, 0, INSTR{
		throw gbe::gbe_error_codes::UNDEFINED_OPCODE;
	}},
	{"CALL NC, u16", 3, 12, INSTR{
		if(INSTRUCTION.call_cc_nn(LOAD_IMMEDIATE_WORD, !R_F.bits.c) == branching_t::DO_BRANCH){
			cpu.cycles.increment_cycles_m(3);
			return false;
		}
		return true;
	}},
	{"PUSH DE", 1, 16, INSTR{
		INSTRUCTION.b16_push(R_DE);
		return true;
	}},
	{"SUB A, u8", 2, 8, INSTR{
		INSTRUCTION.b8_sub(LOAD_IMMEDIATE_BYTE);
		return true;
	}},
	{"RST 10h", 1, 16, INSTR{
		INSTRUCTION.rst(0x10);
		return false;
	}},
	{"RET C", 1, 8, INSTR{
		if(INSTRUCTION.ret_cc(R_F.bits.c) == branching_t::DO_BRANCH){
			cpu.cycles.increment_cycles_m(3);
			return false;
		}
		return true;
	}},
	{"RETI", 1, 16, INSTR{
		INSTRUCTION.reti();
		return false;
	}},
	{"JP C, u16", 3, 12, INSTR{
		if(INSTRUCTION.jp_cc_nn(LOAD_IMMEDIATE_WORD, R_F.bits.c) == branching_t::DO_BRANCH){
			cpu.cycles.increment_cycles_m(1);
			return false;
		}
		return true;
	}},
	{"N/A", 0, 0, INSTR{
		throw gbe::gbe_error_codes::UNDEFINED_OPCODE;
	}},
	{"CALL C, u16", 3, 12, INSTR{
		if(INSTRUCTION.call_cc_nn(LOAD_IMMEDIATE_WORD, R_F.bits.c) == branching_t::DO_BRANCH){
			cpu.cycles.increment_cycles_m(3);
			return false;
		}
		return true;
	}},
	{"N/A", 0, 0, INSTR{
		throw gbe::gbe_error_codes::UNDEFINED_OPCODE;
	}},
	{"SBC A, u8", 2, 8, INSTR{
		INSTRUCTION.b8_sbc(LOAD_IMMEDIATE_BYTE);
		return true;
	}},
	{"RST 18h", 1, 4, INSTR{
		INSTRUCTION.rst(0x18);
		return false;
	}},
	{"LD (0xFF00+u8), A", 2, 12, INSTR{
		INSTRUCTION.b8_load_adr_reg(0xFF00+LOAD_IMMEDIATE_BYTE, R_A);
		return true;
	}},
	{"POP HL", 1, 4, INSTR{
		INSTRUCTION.b16_pop(R_HL);
		return true;
	}},
	{"LD (0xFF00+C), A", 1, 8, INSTR{
		INSTRUCTION.b8_load_adr_reg(0xFF00+R_C, R_A);
		return true;
	}},
	{"N/A", 0, 0, INSTR{
		throw gbe::gbe_error_codes::UNDEFINED_OPCODE;
	}},
	{"N/A", 0, 0, INSTR{
		throw gbe::gbe_error_codes::UNDEFINED_OPCODE;
	}},
	{"PUSH HL", 1, 16, INSTR{
		INSTRUCTION.b16_push(R_HL);
		return true;
	}},
	{"AND A, u8", 2, 8, INSTR{
		INSTRUCTION.b8_and(LOAD_IMMEDIATE_BYTE);
		return true;
	}},
	{"RST 20h", 1, 16, INSTR{
		INSTRUCTION.rst(0x20);
		return false;
	}},
	{"ADD SP, i8", 2, 16, INSTR{
		INSTRUCTION.b16_add_sp(LOAD_IMMEDIATE_BYTE);
		return true;
	}},
	{"JP HL", 1, 4, INSTR{
		INSTRUCTION.jp_hl();
		return false;
	}},
	{"LD (u16), A", 3, 16, INSTR{
		INSTRUCTION.b8_load_adr_reg(LOAD_IMMEDIATE_WORD, R_A);
		return true;
	}},
	{"N/A", 0, 0, INSTR{
		throw gbe::gbe_error_codes::UNDEFINED_OPCODE;
	}},
	{"N/A", 0, 0, INSTR{
		throw gbe::gbe_error_codes::UNDEFINED_OPCODE;
	}},
	{"N/A", 0, 0, INSTR{
		throw gbe::gbe_error_codes::UNDEFINED_OPCODE;
	}},
	{"XOR A, u8", 2, 8, INSTR{
		INSTRUCTION.b8_xor(LOAD_IMMEDIATE_BYTE);
		return true;
	}},
	{"RST 28h", 1, 16, INSTR{
		INSTRUCTION.rst(0x28);
		return false;
	}},
	{"LD A, (0xFF00+u8)", 2, 12, INSTR{
		INSTRUCTION.b8_load_reg_adr(R_A, 0xFF00+LOAD_IMMEDIATE_BYTE);
		return true;
	}},
	{"POP AF", 1, 12, INSTR{
		INSTRUCTION.b16_pop(R_AF);
		R_F.bits.unused = 0;
		return true;
	}},
	{"LD A, (0xFF00+C)", 1, 8, INSTR{
		INSTRUCTION.b8_load_reg_adr(R_A, 0xFF00+R_C);
		return true;
	}},
	{"DI", 1, 4, INSTR{
		INSTRUCTION.misc_di();
		return true;
	}},
	{"N/A", 0, 0, INSTR{
		throw gbe::gbe_error_codes::UNDEFINED_OPCODE;
	}},
	{"PUSH AF", 1, 16, INSTR{
		R_F.bits.unused = 0;
		INSTRUCTION.b16_push(R_AF);
		return true;
	}},
	{"OR A, u8", 2, 8, INSTR{
		INSTRUCTION.b8_or(LOAD_IMMEDIATE_BYTE);
		return true;
	}},
	{"RST 30h", 1, 16, INSTR{
		INSTRUCTION.rst(0x30);
		return false;
	}},
	{"LD HL, SP+i8", 2, 12, INSTR{
		INSTRUCTION.b16_load_hl_sp_e(LOAD_IMMEDIATE_BYTE);
		return true;
	}},
	{"LD SP, HL", 1, 8, INSTR{
		INSTRUCTION.b16_load_reg_val(R_SP, R_HL);
		return true;
	}},
	{"LD A, (u16)", 3, 16, INSTR{
		INSTRUCTION.b8_load_reg_adr(R_A, LOAD_IMMEDIATE_WORD);
		return true;
	}},
	{"EI", 1, 4, INSTR{
		INSTRUCTION.misc_ei();
		return true;
	}},
	{"N/A", 0, 0, INSTR{
		throw gbe::gbe_error_codes::UNDEFINED_OPCODE;
	}},
	{"N/A", 0, 0, INSTR{
		throw gbe::gbe_error_codes::UNDEFINED_OPCODE;
	}},
	{"CP A, u8", 2, 8, INSTR{
		INSTRUCTION.b8_cp(LOAD_IMMEDIATE_BYTE);
		return true;
	}},
	{"RST 38h", 1, 16, INSTR{
		INSTRUCTION.rst(0x38);
		return false;
	}}
};
int gbe::cpu_t::instruction_count{sizeof(cpu_instructions)/sizeof(*cpu_instructions)};