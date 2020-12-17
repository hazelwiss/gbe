#include"cpu.h"
#define INSTR [](gbe::cpu_t& cpu) noexcept

void gbe::cpu_t::emulate_fetch_decode_execute_cycle(){
	throw gbe_error_codes::UNKNOWN;
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
gbe::cpu_t::instruction_t gbe::cpu_t::cpu_instructions[]{
	{"NOP", 1, 4, INSTR{
		INSTRUCTION.misc_nop();
	}},
	{"LD (BC), u16", 3, 12, INSTR{
		INSTRUCTION.b16_load_adr_val(R_BC, LOAD_IMMEDIATE_WORD);
	}},
	{"LD (BC), A", 1, 8, INSTR{
		INSTRUCTION.b8_load_adr_val(R_BC, LOAD_IMMEDIATE_BYTE);
	}},
	{"INC BC", 1, 8, INSTR{
		INSTRUCTION.b16_inc(R_BC);
	}},
	{"INC B", 1, 4, INSTR{
		INSTRUCTION.b8_inc(R_B);
	}},
	{"DEC B", 1, 4, INSTR{ 
		INSTRUCTION.b8_dec(R_B);
	}},
	{"LD B, u8", 2, 8, INSTR{
		INSTRUCTION.b8_load_reg_val(R_B, LOAD_IMMEDIATE_BYTE);
	}},
	{"RLCA", 1, 4, INSTR{
		INSTRUCTION.misc_rlc(R_A);
	}},
	{"LD (u16), SP", 3, 20, INSTR{
		INSTRUCTION.b16_load_adr_val(LOAD_IMMEDIATE_WORD, R_SP);
	}},
	{"ADD HL, BC", 1, 8, INSTR{
		INSTRUCTION.b16_add(R_BC);
	}},
	{"LD A, (BC)", 1, 8, INSTR{
		INSTRUCTION.b8_load_reg_adr(R_A, R_BC);
	}},
	{"DEC BC", 1, 8, INSTR{
		INSTRUCTION.b16_dec(R_BC);
	}},
	{"INC C", 1, 4, INSTR{
		INSTRUCTION.b8_inc(R_C);
	}},
	{"DEC C", 1, 4, INSTR{
		INSTRUCTION.b8_dec(R_C);
	}},
	{"LD C, u8", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_C, LOAD_IMMEDIATE_BYTE);
	}},
	{"RRCA", 1, 4, INSTR{
		INSTRUCTION.misc_rrc(R_A);
	}},
	{"STOP", 2, 4, INSTR{
		INSTRUCTION.misc_stop();
	}},
	{"LD DE, u16", 3, 12, INSTR{
		INSTRUCTION.b16_load_reg_adr(R_DE, LOAD_IMMEDIATE_WORD);
	}},
	{"LD (DE), A", 1, 8, INSTR{
		INSTRUCTION.b8_load_adr_val(R_DE, R_A);
	}},
	{"INC DE", 1, 8, INSTR{
		INSTRUCTION.b16_inc(R_DE);
	}},
	{"INC D", 1, 4, INSTR{
		INSTRUCTION.b8_inc(R_D);
	}},
	{"DEC D", 1, 4, INSTR{
		INSTRUCTION.b8_dec(R_D);
	}},
	{"LD D, u8", 2, 8, INSTR{
		INSTRUCTION.b8_load_reg_val(R_D, LOAD_IMMEDIATE_BYTE);
	}},
	{"RLA", 1, 4, INSTR{
		INSTRUCTION.misc_rl(R_A);
	}},
	{"JR i8", 2, 12, INSTR{
		INSTRUCTION.jr((signed char)LOAD_IMMEDIATE_BYTE);
	}},
	{"ADD HL, DE", 1, 8, INSTR{
		INSTRUCTION.b16_add(R_DE);
	}},
	{"LD A, (DE)", 1, 8, INSTR{
		INSTRUCTION.b8_load_reg_adr(R_A, R_DE);
	}},
	{"DEC DE", 1, 8, INSTR{
		INSTRUCTION.b16_dec(R_DE);
	}},
	{"INC E", 1, 4, INSTR{
		INSTRUCTION.b8_inc(R_E);
	}},
	{"DEC E", 1, 4, INSTR{
		INSTRUCTION.b8_dec(R_E);
	}},
	{"LD E, u8", 2, 8, INSTR{
		INSTRUCTION.b8_load_reg_val(R_E, LOAD_IMMEDIATE_BYTE);
	}},
	{"RRA", 1, 4, INSTR{
		INSTRUCTION.misc_rr(R_A);
	}},
	{"JR NZ, i8", 2, 8, INSTR{
		INSTRUCTION.jr_cc(LOAD_IMMEDIATE_BYTE, !R_F.bits.z);
	}},
	{"LD HL, u16", 3, 12, INSTR{
		INSTRUCTION.b16_load_reg_val(R_HL, LOAD_IMMEDIATE_WORD);
	}},
	{"LD (HL+), A", 1, 8, INSTR{
		INSTRUCTION.b8_load_adr_val(R_HL++, R_A);
	}},
	{"INC HL", 1, 8, INSTR{
		INSTRUCTION.b16_inc(R_HL);
	}},
	{"INC H", 1, 4, INSTR{
		INSTRUCTION.b8_inc(R_H);
	}},
	{"DEC H", 1, 4, INSTR{
		INSTRUCTION.b8_dec(R_H);
	}},
	{"LD H, u8", 2, 8, INSTR{
		INSTRUCTION.b8_load_reg_val(R_H, LOAD_IMMEDIATE_BYTE);
	}},
	{"DAA", 1, 4, INSTR{
		INSTRUCTION.misc_daa();
	}},
	{"JR Z, i8", 2, 8, INSTR{
		INSTRUCTION.jr_cc((signed char)LOAD_IMMEDIATE_BYTE, R_F.bits.z);
	}},
	{"ADD HL, HL", 1, 8, INSTR{
		INSTRUCTION.b16_add(R_HL);
	}},
	{"LD A, (HL+)", 1, 8, INSTR{
		INSTRUCTION.b8_load_reg_adr(R_A, R_HL++);
	}},
	{"DEC HL", 1, 8, INSTR{
		INSTRUCTION.b16_dec(R_HL);
	}},
	{"INC L", 1, 4, INSTR{
		INSTRUCTION.b8_inc(R_L);
	}},
	{"DEC L", 1, 4, INSTR{
		INSTRUCTION.b8_dec(R_L);
	}},
	{"LD L, u8", 2, 8, INSTR{
		INSTRUCTION.b8_load_reg_val(R_L, LOAD_IMMEDIATE_BYTE);
	}},
	{"CPL", 1, 4, INSTR{
		INSTRUCTION.misc_cpl();
	}},
	{"JR NC, i8", 2, 8, INSTR{
		INSTRUCTION.jr_cc((signed char)LOAD_IMMEDIATE_BYTE, !R_F.bits.c);
	}},
	{"LD SP, u16", 3, 12, INSTR{
		INSTRUCTION.b16_load_reg_val(R_SP, LOAD_IMMEDIATE_WORD);
	}},
	{"LD (HL-), A", 1, 8, INSTR{
		INSTRUCTION.b8_load_adr_val(R_HL--, R_A);
	}},
	{"INC SP", 1, 8, INSTR{
		INSTRUCTION.b16_inc(R_SP);
	}},
	{"INC (HL)", 1, 12, INSTR{
		INSTRUCTION.b8_inc_adr(R_HL);
	}},
	{"DEC (HL)", 1, 4, INSTR{
		INSTRUCTION.b8_dec_adr(R_HL);
	}},
	{"LD (HL), u8", 2, 12, INSTR{
		INSTRUCTION.b8_load_adr_val(R_HL, LOAD_IMMEDIATE_BYTE);
	}},
	{"SCF", 1, 4, INSTR{
		INSTRUCTION.misc_scf();
	}},
	{"JR C, i8", 2, 8, INSTR{
		INSTRUCTION.jr_cc((signed char)LOAD_IMMEDIATE_BYTE, R_F.bits.c);
	}},
	{"ADD HL, SP", 1, 8, INSTR{
		INSTRUCTION.b16_add(R_SP);
	}},
	{"LD A, (HL-)", 1, 8, INSTR{
		INSTRUCTION.b8_load_reg_adr(R_A, R_HL--);
	}},
	{"DEC SP", 1, 8, INSTR{
		INSTRUCTION.b16_dec(R_SP);
	}},
	{"INC A", 1, 4, INSTR{
		INSTRUCTION.b8_inc(R_A);
	}},
	{"DEC A", 1, 4, INSTR{
		INSTRUCTION.b8_dec(R_A);
	}},
	{"LD A, u8", 2, 8, INSTR{
		INSTRUCTION.b8_load_reg_val(R_A, LOAD_IMMEDIATE_BYTE);
	}},
	{"CCF", 1, 4, INSTR{
		INSTRUCTION.misc_ccf();
	}},
	{"LD B, B", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_B, R_B);
	}},
	{"LD B, C", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_B, R_C);
	}},
	{"LD B, D", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_B, R_D);
	}},
	{"LD B, E", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_B, R_E);
	}},
	{"LD B, H", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_B, R_H);
	}},
	{"LD B, L", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_B, R_L);
	}},
	{"LD B, (HL)", 1, 8, INSTR{
		INSTRUCTION.b8_load_reg_adr(R_B, R_HL);
	}},
	{"LD B, A", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_B, R_A);
	}},
	{"LD C, B", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_C, R_B);
	}},
	{"LD C, C", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_C, R_C);
	}},
	{"LD C, D", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_C, R_D);
	}},
	{"LD C, E", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_C, R_E);
	}},
	{"LD C, H", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_C, R_H);
	}},
	{"LD C, L", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_C, R_L);
	}},
	{"LD C, (HL)", 1, 8, INSTR{
		INSTRUCTION.b8_load_reg_adr(R_C, R_HL);
	}},
	{"LD C, A", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_C, R_A);
	}},
	{"LD D, B", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_D, R_B);
	}},
	{"LD D, C", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_D, R_C);
	}},
	{"LD D, D", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_D, R_D);
	}},
	{"LD D, E", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_D, R_E);
	}},
	{"LD D, H", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_D, R_H);
	}},
	{"LD D, L", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_D, R_L);
	}},
	{"LD D, (HL)", 1, 8, INSTR{
		INSTRUCTION.b8_load_reg_adr(R_D, R_HL);
	}},
	{"LD D, A", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_D, R_A);
	}},
	{"LD E, B", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_E, R_B);
	}},
	{"LD E, C", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_E, R_C);
	}},
	{"LD E, D", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_E, R_D);
	}},
	{"LD E, E", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_E, R_E);
	}},
	{"LD E, H", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_E, R_H);
	}},
	{"LD E, L", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_E, R_L);
	}},
	{"LD E, (HL)", 1, 8, INSTR{
		INSTRUCTION.b8_load_reg_adr(R_E, R_HL);
	}},
	{"LD E, A", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_E, R_A);
	}},
	{"LD H, B", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_H, R_B);
	}},
	{"LD H, C", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_H, R_C);
	}},
	{"LD H, D", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_H, R_D);
	}},
	{"LD H, E", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_H, R_E);
	}},
	{"LD H, H", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_H, R_H);
	}},
	{"LD H, L", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_H, R_L);
	}},
	{"LD H, (HL)", 1, 8, INSTR{
		INSTRUCTION.b8_load_reg_adr(R_H, R_HL);
	}},
	{"LD H, A", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_H, R_A);
	}},
	{"LD L, B", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_L, R_B);
	}},
	{"LD L, C", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_L, R_C);
	}},
	{"LD L, D", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_L, R_D);
	}},
	{"LD L, E", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_L, R_E);
	}},
	{"LD L, H", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_L, R_H);
	}},
	{"LD L, L", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_L, R_L);
	}},
	{"LD L, (HL)", 1, 8, INSTR{
		INSTRUCTION.b8_load_reg_adr(R_L, R_HL);
	}},
	{"LD L, A", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_L, R_A);
	}},
	{"LD (HL), B", 1, 8, INSTR{
		INSTRUCTION.b8_load_adr_val(R_HL, R_B);
	}},
	{"LD (HL), C", 1, 8, INSTR{
		INSTRUCTION.b8_load_adr_val(R_HL, R_C);
	}},
	{"LD (HL), D", 1, 8, INSTR{
		INSTRUCTION.b8_load_adr_val(R_HL, R_D);
	}},
	{"LD (HL), E", 1, 8, INSTR{
		INSTRUCTION.b8_load_adr_val(R_HL, R_E);
	}},
	{"LD (HL), H", 1, 8, INSTR{
		INSTRUCTION.b8_load_adr_val(R_HL, R_H);
	}},
	{"LD (HL), L", 1, 8, INSTR{
		INSTRUCTION.b8_load_adr_val(R_HL, R_L);
	}},
	{"HALT", 1, 4, INSTR{
		INSTRUCTION.misc_halt();
	}},
	{"LD (HL), A", 1, 8, INSTR{
		INSTRUCTION.b8_load_adr_val(R_HL, R_A);
	}},
	{"LD A, B", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_A, R_B);
	}},
	{"LD A, C", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_A, R_C);
	}},
	{"LD A, D", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_A, R_D);
	}},
	{"LD A, E", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_A, R_E);
	}},
	{"LD A, H", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_A, R_H);
	}},
	{"LD A, L", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_A, R_L);
	}},
	{"LD A, (HL)", 1, 8, INSTR{
		INSTRUCTION.b8_load_reg_adr(R_A, R_HL);
	}},
	{"LD A, A", 1, 4, INSTR{
		INSTRUCTION.b8_load_reg_val(R_A, R_A);
	}},
	{"ADD A, B", 1, 4, INSTR{
		INSTRUCTION.b8_add(R_B);
	}},
	{"ADD A, C", 1, 4, INSTR{
		INSTRUCTION.b8_add(R_C);
	}},
	{"ADD A, D", 1, 4, INSTR{
		INSTRUCTION.b8_add(R_D);
	}},
	{"ADD A, E", 1, 4, INSTR{
		INSTRUCTION.b8_add(R_E);
	}},
	{"ADD A, H", 1, 4, INSTR{
		INSTRUCTION.b8_add(R_H);
	}},
	{"ADD A, L", 1, 4, INSTR{
		INSTRUCTION.b8_add(R_L);
	}},
	{"ADD A, (HL)", 1, 8, INSTR{
		INSTRUCTION.b8_add(cpu.memory.read_byte_from_memory(R_HL));
	}},
	{"ADD A, A", 1, 4, INSTR{
		INSTRUCTION.b8_add(R_A);
	}},
	{"ADC A, B", 1, 4, INSTR{
		INSTRUCTION.b8_adc(R_B);
	}},
	{"ADC A, C", 1, 4, INSTR{
		INSTRUCTION.b8_adc(R_C);
	}},
	{"ADC A, D", 1, 4, INSTR{
		INSTRUCTION.b8_adc(R_D);
	}},
	{"ADC A, E", 1, 4, INSTR{
		INSTRUCTION.b8_adc(R_E);
	}},
	{"ADC A, H", 1, 4, INSTR{
		INSTRUCTION.b8_adc(R_H);
	}},
	{"ADC A, L", 1, 4, INSTR{
		INSTRUCTION.b8_adc(R_L);
	}},
	{"ADC A, (HL)", 1, 8, INSTR{
		INSTRUCTION.b8_adc(cpu.memory.read_byte_from_memory(R_HL));
	}},
	{"ADC A, A", 1, 4, INSTR{
		INSTRUCTION.b8_adc(R_A);
	}},
	{"SUB A, B", 1, 4, INSTR{
		INSTRUCTION.b8_sub(R_B);
	}},
	{"SUB A, C", 1, 4, INSTR{
		INSTRUCTION.b8_sub(R_C);
	}},
	{"SUB A, D", 1, 4, INSTR{
		INSTRUCTION.b8_sub(R_D);
	}},
	{"SUB A, E", 1, 4, INSTR{
		INSTRUCTION.b8_sub(R_E);
	}},
	{"SUB A, H", 1, 4, INSTR{
		INSTRUCTION.b8_sub(R_H);
	}},
	{"SUB A, L", 1, 4, INSTR{
		INSTRUCTION.b8_sub(R_L);
	}},
	{"SUB A, (HL)", 1, 8, INSTR{
		INSTRUCTION.b8_sub(cpu.memory.read_byte_from_memory(R_HL));
	}},
	{"SUB A, A", 1, 4, INSTR{
		INSTRUCTION.b8_sub(R_A);
	}},
	{"SBC A, B", 1, 4, INSTR{
		INSTRUCTION.b8_sbc(R_B);
	}},
	{"SBC A, C", 1, 4, INSTR{
		INSTRUCTION.b8_sbc(R_C);
	}},
	{"SBC A, D", 1, 4, INSTR{
		INSTRUCTION.b8_sbc(R_D);
	}},
	{"SBC A, E", 1, 4, INSTR{
		INSTRUCTION.b8_sbc(R_E);
	}},
	{"SBC A, H", 1, 4, INSTR{
		INSTRUCTION.b8_sbc(R_H);
	}},
	{"SBC A, L", 1, 4, INSTR{
		INSTRUCTION.b8_sbc(R_L);
	}},
	{"SBC A, (HL)", 1, 8, INSTR{
		INSTRUCTION.b8_sbc(cpu.memory.read_byte_from_memory(R_HL));
	}},
	{"SBC A, A", 1, 4, INSTR{
		INSTRUCTION.b8_sbc(R_A);
	}},
	{"AND A, B", 1, 4, INSTR{
		INSTRUCTION.b8_and(R_B);
	}},
	{"AND A, C", 1, 4, INSTR{
		INSTRUCTION.b8_and(R_C);
	}},
	{"AND A, D", 1, 4, INSTR{
		INSTRUCTION.b8_and(R_D);
	}},
	{"AND A, E", 1, 4, INSTR{
		INSTRUCTION.b8_and(R_E);
	}},
	{"AND A, H", 1, 4, INSTR{
		INSTRUCTION.b8_and(R_H);
	}},
	{"AND A, L", 1, 4, INSTR{
		INSTRUCTION.b8_and(R_L);
	}},
	{"AND A, (HL)", 1, 8, INSTR{
		INSTRUCTION.b8_and(cpu.memory.read_byte_from_memory(R_HL));
	}},
	{"AND A, A", 1, 4, INSTR{
		INSTRUCTION.b8_and(R_A);
	}},
	{"XOR A, B", 1, 4, INSTR{
		INSTRUCTION.b8_xor(R_B);
	}},
	{"XOR A, C", 1, 4, INSTR{
		INSTRUCTION.b8_xor(R_C);
	}},
	{"XOR A, D", 1, 4, INSTR{
		INSTRUCTION.b8_xor(R_D);
	}},
	{"XOR A, E", 1, 4, INSTR{
		INSTRUCTION.b8_xor(R_E);
	}},
	{"XOR A, H", 1, 4, INSTR{
		INSTRUCTION.b8_xor(R_H);
	}},
	{"XOR A, L", 1, 4, INSTR{
		INSTRUCTION.b8_xor(R_L);
	}},
	{"XOR A, (HL)", 1, 8, INSTR{
		INSTRUCTION.b8_xor(cpu.memory.read_byte_from_memory(R_HL));
	}},
	{"XOR A, A", 1, 4, INSTR{
		INSTRUCTION.b8_xor(R_A);
	}},
	{"OR A, B", 1, 4, INSTR{
		INSTRUCTION.b8_or(R_B);
	}},
	{"OR A, C", 1, 4, INSTR{
		INSTRUCTION.b8_or(R_C);
	}},
	{"OR A, D", 1, 4, INSTR{
		INSTRUCTION.b8_or(R_D);
	}},
	{"OR A, E", 1, 4, INSTR{
		INSTRUCTION.b8_or(R_E);
	}},
	{"OR A, H", 1, 4, INSTR{
		INSTRUCTION.b8_or(R_H);
	}},
	{"OR A, L", 1, 4, INSTR{
		INSTRUCTION.b8_or(R_L);
	}},
	{"OR A, (HL)", 1, 8, INSTR{
		INSTRUCTION.b8_or(cpu.memory.read_byte_from_memory(R_HL));
	}},
	{"OR A, A", 1, 4, INSTR{
		INSTRUCTION.b8_or(R_A);
	}},
	{"CP A, B", 1, 4, INSTR{
		INSTRUCTION.b8_cp(R_B);
	}},
	{"CP A, C", 1, 4, INSTR{
		INSTRUCTION.b8_cp(R_C);
	}},
	{"CP A, D", 1, 4, INSTR{
		INSTRUCTION.b8_cp(R_D);
	}},
	{"CP A, E", 1, 4, INSTR{
		INSTRUCTION.b8_cp(R_E);
	}},
	{"CP A, H", 1, 4, INSTR{
		INSTRUCTION.b8_cp(R_H);
	}},
	{"CP A, L", 1, 4, INSTR{
		INSTRUCTION.b8_cp(R_L);
	}},
	{"CP A, (HL)", 1, 8, INSTR{
		INSTRUCTION.b8_cp(cpu.memory.read_byte_from_memory(R_HL));
	}},
	{"CP A, A", 1, 4, INSTR{
		INSTRUCTION.b8_cp(R_A);
	}},
	{"RET NZ", 1, 8, INSTR{
		INSTRUCTION.ret_cc(!R_F.bits.z);
	}},
	{"POP BC", 1, 12, INSTR{
		INSTRUCTION.b16_pop(R_BC);
	}},
	{"JP NZ, u16", 3, 12, INSTR{
		INSTRUCTION.jp_cc_nn(LOAD_IMMEDIATE_WORD, !R_F.bits.z);
	}},
	{"JP u16", 3, 16, INSTR{
		INSTRUCTION.jp_nn(LOAD_IMMEDIATE_WORD);
	}},
	{"CALL NZ, u16", 3, 12, INSTR{
		INSTRUCTION.call_cc_nn(LOAD_IMMEDIATE_WORD, !R_F.bits.z);
	}},
	{"PUSH BC", 1, 16, INSTR{
		INSTRUCTION.b16_push(R_BC);
	}},
	{"ADD A, u8", 2, 8, INSTR{
		INSTRUCTION.b8_add(LOAD_IMMEDIATE_BYTE);
	}},
	{"RST 00h", 1, 16, INSTR{
		INSTRUCTION.rst(0x00);
	}},
	{"RET Z", 1, 8, INSTR{
		INSTRUCTION.ret_cc(R_F.bits.z);
	}},
	{"RET", 1, 16, INSTR{
		INSTRUCTION.ret();
	}},
	{"JP Z, u16", 3, 12, INSTR{
		INSTRUCTION.jp_cc_nn(LOAD_IMMEDIATE_WORD, R_F.bits.z);
	}},
	{"PREFIX CB", 1, 4, INSTR{
		//	SPECIAL CASE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	}},
	{"CALL Z, u16", 3, 12, INSTR{
		INSTRUCTION.call_cc_nn(LOAD_IMMEDIATE_WORD, R_F.bits.z);
	}},
	{"CALL u16", 3, 24, INSTR{
		INSTRUCTION.call_nn(LOAD_IMMEDIATE_WORD);
	}},
	{"ADC A, u8", 2, 8, INSTR{
		INSTRUCTION.b8_adc(LOAD_IMMEDIATE_BYTE);
	}},
	{"RST 08h", 1, 16, INSTR{
		INSTRUCTION.rst(0x08);
	}},
	{"RET NC", 1, 8, INSTR{
		INSTRUCTION.ret_cc(!R_F.bits.c);
	}},
	{"POP DE", 1, 12, INSTR{
		INSTRUCTION.b16_pop(R_DE);
	}},
	{"JP NC, u16", 3, 12, INSTR{
		INSTRUCTION.jp_cc_nn(LOAD_IMMEDIATE_WORD, !R_F.bits.c);
	}},
	{"N/A", 0, 0, INSTR{

	}},
	{"CALL NC, u16", 3, 12, INSTR{
		INSTRUCTION.call_cc_nn(LOAD_IMMEDIATE_WORD, !R_F.bits.c);
	}},
	{"PUSH DE", 1, 16, INSTR{
		INSTRUCTION.b16_push(R_DE);
	}},
	{"SUB A, u8", 2, 8, INSTR{
		INSTRUCTION.b8_sub(LOAD_IMMEDIATE_BYTE);
	}},
	{"RST 10h", 1, 16, INSTR{
		INSTRUCTION.rst(0x10);
	}},
	{"RET C", 1, 8, INSTR{
		INSTRUCTION.ret_cc(R_F.bits.c);
	}},
	{"RETI", 1, 16, INSTR{
		INSTRUCTION.reti();
	}},
	{"JP C, u16", 3, 12, INSTR{
		INSTRUCTION.jp_cc_nn(LOAD_IMMEDIATE_WORD, R_F.bits.c);
	}},
	{"N/A", 0, 0, INSTR{

	}},
	{"CALL C, u16", 3, 12, INSTR{
		INSTRUCTION.call_cc_nn(LOAD_IMMEDIATE_WORD, R_F.bits.c);
	}},
	{"N/A", 0, 0, INSTR{

	}},
	{"SBC A, u8", 2, 8, INSTR{
		INSTRUCTION.b8_sbc(LOAD_IMMEDIATE_BYTE);
	}},
	{"RST 18h", 1, 4, INSTR{
		INSTRUCTION.rst(0x18);
	}},
	{"LD (0xFF00+u8), A", 2, 12, INSTR{
		INSTRUCTION.b8_load_adr_val(0xFF00+LOAD_IMMEDIATE_BYTE, R_A);
	}},
	{"POP HL", 1, 4, INSTR{
		INSTRUCTION.b16_pop(R_HL);
	}},
	{"LD (0xFF00+C), A", 1, 8, INSTR{
		INSTRUCTION.b8_load_adr_val(0xFF00+R_F.bits.c, R_A);
	}},
	{"N/A", 0, 0, INSTR{

	}},
	{"N/A", 0, 0, INSTR{

	}},
	{"PUSH HL", 1, 16, INSTR{
		INSTRUCTION.b16_push(R_HL);
	}},
	{"AND A, u8", 2, 8, INSTR{
		INSTRUCTION.b8_and(LOAD_IMMEDIATE_BYTE);
	}},
	{"RST 20h", 1, 16, INSTR{
		INSTRUCTION.rst(0x20);
	}},
	{"ADD SP, i8", 2, 16, INSTR{
		INSTRUCTION.b16_add_sp((signed char)LOAD_IMMEDIATE_BYTE);
	}},
	{"JP HL", 1, 4, INSTR{
		INSTRUCTION.jp_hl();
	}},
	{"LD (u16), A", 3, 16, INSTR{
		INSTRUCTION.b16_load_adr_val(LOAD_IMMEDIATE_WORD, R_A);
	}},
	{"N/A", 0, 0, INSTR{

	}},
	{"N/A", 0, 0, INSTR{

	}},
	{"N/A", 0, 0, INSTR{

	}},
	{"XOR A, u8", 2, 8, INSTR{
		INSTRUCTION.b8_xor(LOAD_IMMEDIATE_BYTE);
	}},
	{"RST 28h", 1, 16, INSTR{
		INSTRUCTION.rst(0x28);
	}},
	{"LD A, (0xFF00+u8)", 2, 12, INSTR{
		INSTRUCTION.b8_load_reg_adr(R_A, 0xFF00+LOAD_IMMEDIATE_BYTE);
	}},
	{"POP AF", 1, 12, INSTR{
		INSTRUCTION.b16_pop(R_AF);
	}},
	{"LD A, (0xFF00+C)", 1, 8, INSTR{
		INSTRUCTION.b8_load_reg_adr(R_A, 0xFF00+R_F.bits.c);
	}},
	{"DI", 1, 4, INSTR{
		INSTRUCTION.misc_di();
	}},
	{"N/A", 0, 0, INSTR{

	}},
	{"PUSH AF", 1, 16, INSTR{
		INSTRUCTION.b16_push(R_AF);
	}},
	{"OR A, u8", 2, 8, INSTR{
		INSTRUCTION.b8_or(LOAD_IMMEDIATE_BYTE);
	}},
	{"RST 30h", 1, 16, INSTR{
		INSTRUCTION.rst(0x30);
	}},
	{"LD HL, SP+i8", 2, 12, INSTR{
		INSTRUCTION.b16_load_reg_val(R_HL, R_SP + (signed char)LOAD_IMMEDIATE_BYTE);
	}},
	{"LD SP, HL", 1, 8, INSTR{
		INSTRUCTION.b16_load_reg_val(R_SP, R_HL);
	}},
	{"LD A, (u16)", 3, 16, INSTR{
		INSTRUCTION.b8_load_reg_adr(R_A, LOAD_IMMEDIATE_WORD);
	}},
	{"EI", 1, 4, INSTR{
		INSTRUCTION.misc_ei();
	}},
	{"N/A", 0, 0, INSTR{

	}},
	{"N/A", 0, 0, INSTR{

	}},
	{"CP A, u8", 2, 8, INSTR{
		INSTRUCTION.b8_cp(LOAD_IMMEDIATE_BYTE);
	}},
	{"RST 38h", 1, 16, INSTR{
		INSTRUCTION.rst(0x38);
	}}
};
int gbe::cpu_t::instruction_count{sizeof(cpu_instructions)/sizeof(*cpu_instructions)};