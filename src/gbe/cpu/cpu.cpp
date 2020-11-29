#include"cpu.h"
#define INSTR [](gbe::cpu_t& cpu) noexcept

void gbe::cpu_t::emulate_fetch_decode_execute_cycle(){
	throw gbe_exception(gbe_error_codes::UNKNOWN);
}

gbe::cpu_t::instruction_t gbe::cpu_t::instructions[]{
	{"NOP", 1, 4, INSTR{

	}},
	{"LD (BC), u16", 3, 12, INSTR{

	}},
	{"LD (BC), A", 1, 8, INSTR{

	}},
	{"INC BC", 1, 8, INSTR{

	}},
	{"INC B", 1, 4, INSTR{

	}},
	{"DEC B", 1, 4, INSTR{ 

	}},
	{"LD B, u8", 2, 8, INSTR{

	}},
	{"RLCA", 1, 4, INSTR{

	}},
	{"LD (u16), SP", 3, 20, INSTR{

	}},
	{"ADD HL, BC", 1, 8, INSTR{

	}},
	{"LD A, (BC)", 1, 8, INSTR{

	}},
	{"DEC BC", 1, 8, INSTR{

	}},
	{"INC C", 1, 4, INSTR{

	}},
	{"DEC C", 1, 4, INSTR{

	}},
	{"LD C, u8", 1, 4, INSTR{

	}},
	{"RRCA", 1, 4, INSTR{

	}},
	{"STOP", 2, 4, INSTR{

	}},
	{"LD DE, u16", 3, 12, INSTR{

	}},
	{"LD (DE), A", 1, 8, INSTR{

	}},
	{"INC DE", 1, 8, INSTR{

	}},
	{"INC D", 1, 4, INSTR{

	}},
	{"DEC D", 1, 4, INSTR{

	}},
	{"LD D, u8", 2, 8, INSTR{

	}},
	{"RLA", 1, 4, INSTR{

	}},
	{"JR i8", 2, 12, INSTR{

	}},
	{"ADD HL, DE", 1, 8, INSTR{

	}},
	{"LD A, (DE)", 1, 8, INSTR{

	}},
	{"DEC DE", 1, 8, INSTR{

	}},
	{"INC E", 1, 4, INSTR{

	}},
	{"DEC E", 1, 4, INSTR{

	}},
	{"LD E, u8", 2, 8, INSTR{

	}},
	{"RRA", 1, 4, INSTR{

	}},
	{"JR NZ, i8", 2, 8, INSTR{

	}},
	{"LD HL, u16", 3, 12, INSTR{

	}},
	{"LD (HL+), A", 1, 8, INSTR{

	}},
	{"INC HL", 1, 8, INSTR{

	}},
	{"INC H", 1, 4, INSTR{

	}},
	{"DEC H", 1, 4, INSTR{

	}},
	{"LD H, u8", 2, 8, INSTR{

	}},
	{"DAA", 1, 4, INSTR{

	}},
	{"JR Z, i8", 2, 8, INSTR{

	}},
	{"ADD HL, HL", 1, 8, INSTR{

	}},
	{"LD A, (HL+)", 1, 8, INSTR{

	}},
	{"DEC HL", 1, 8, INSTR{

	}},
	{"INC L", 1, 4, INSTR{

	}},
	{"DEC L", 1, 4, INSTR{

	}},
	{"LD L, u8", 2, 8, INSTR{

	}},
	{"CPL", 1, 4, INSTR{

	}},
	{"JR NC, i8", 2, 8, INSTR{

	}},
	{"LD SP, u16", 3, 12, INSTR{

	}},
	{"LD (HL-), A", 1, 8, INSTR{

	}},
	{"INC SP", 1, 8, INSTR{

	}},
	{"INC (HL)", 1, 12, INSTR{

	}},
	{"DEC (HL)", 1, 4, INSTR{

	}},
	{"LD (HL), u8", 2, 12, INSTR{

	}},
	{"SCF", 1, 4, INSTR{

	}},
	{"JR C, i8", 2, 8, INSTR{

	}},
	{"ADD HL, SP", 1, 8, INSTR{

	}},
	{"LD A, (HL-)", 1, 8, INSTR{

	}},
	{"DEC SP", 1, 8, INSTR{

	}},
	{"INC A", 1, 4, INSTR{

	}},
	{"DEC A", 1, 4, INSTR{

	}},
	{"LD A, u8", 2, 8, INSTR{

	}},
	{"CCF", 1, 4, INSTR{

	}},
	{"LD B, B", 1, 4, INSTR{

	}},
	{"LD B, C", 1, 4, INSTR{

	}},
	{"LD B, D", 1, 4, INSTR{

	}},
	{"LD B, E", 1, 4, INSTR{

	}},
	{"LD B, H", 1, 4, INSTR{

	}},
	{"LD B, L", 1, 4, INSTR{

	}},
	{"LD B, (HL)", 1, 8, INSTR{

	}},
	{"LD B, A", 1, 4, INSTR{

	}},
	{"LD C, B", 1, 4, INSTR{

	}},
	{"LD C, C", 1, 4, INSTR{

	}},
	{"LD C, D", 1, 4, INSTR{

	}},
	{"LD C, E", 1, 4, INSTR{

	}},
	{"LD C, H", 1, 4, INSTR{

	}},
	{"LD C, L", 1, 4, INSTR{

	}},
	{"LD C, (HL)", 1, 8, INSTR{

	}},
	{"LD C, A", 1, 4, INSTR{

	}},
	{"LD D, B", 1, 4, INSTR{

	}},
	{"LD D, C", 1, 4, INSTR{

	}},
	{"LD D, D", 1, 4, INSTR{

	}},
	{"LD D, E", 1, 4, INSTR{

	}},
	{"LD D, H", 1, 4, INSTR{

	}},
	{"LD D, L", 1, 4, INSTR{

	}},
	{"LD D, (HL)", 1, 8, INSTR{

	}},
	{"LD D, A", 1, 4, INSTR{

	}},
	{"LD E, B", 1, 4, INSTR{

	}},
	{"LD E, C", 1, 4, INSTR{

	}},
	{"LD E, D", 1, 4, INSTR{

	}},
	{"LD E, E", 1, 4, INSTR{

	}},
	{"LD E, H", 1, 4, INSTR{

	}},
	{"LD E, L", 1, 4, INSTR{

	}},
	{"LD E, (HL)", 1, 8, INSTR{

	}},
	{"LD E, A", 1, 4, INSTR{

	}},
	{"LD H, B", 1, 4, INSTR{

	}},
	{"LD H, C", 1, 4, INSTR{

	}},
	{"LD H, D", 1, 4, INSTR{

	}},
	{"LD H, E", 1, 4, INSTR{

	}},
	{"LD H, H", 1, 4, INSTR{

	}},
	{"LD H, L", 1, 4, INSTR{

	}},
	{"LD H, (HL)", 1, 8, INSTR{

	}},
	{"LD H, A", 1, 4, INSTR{

	}},
	{"LD L, B", 1, 4, INSTR{

	}},
	{"LD L, C", 1, 4, INSTR{

	}},
	{"LD L, D", 1, 4, INSTR{

	}},
	{"LD L, E", 1, 4, INSTR{

	}},
	{"LD L, H", 1, 4, INSTR{

	}},
	{"LD L, L", 1, 4, INSTR{

	}},
	{"LD L, (HL)", 1, 8, INSTR{

	}},
	{"LD L, A", 1, 4, INSTR{

	}},
	{"LD (HL), B", 1, 8, INSTR{

	}},
	{"LD (HL), C", 1, 8, INSTR{

	}},
	{"LD (HL), D", 1, 8, INSTR{

	}},
	{"LD (HL), E", 1, 8, INSTR{

	}},
	{"LD (HL), H", 1, 8, INSTR{

	}},
	{"LD (HL), L", 1, 8, INSTR{

	}},
	{"HALT", 1, 4, INSTR{

	}},
	{"LD (HL), A", 1, 8, INSTR{

	}},
	{"LD A, B", 1, 4, INSTR{

	}},
	{"LD A, C", 1, 4, INSTR{

	}},
	{"LD A, D", 1, 4, INSTR{

	}},
	{"LD A, E", 1, 4, INSTR{

	}},
	{"LD A, H", 1, 4, INSTR{

	}},
	{"LD A, L", 1, 4, INSTR{

	}},
	{"LD A, (HL)", 1, 8, INSTR{

	}},
	{"LD A, A", 1, 4, INSTR{

	}},
	{"ADD A, B", 1, 4, INSTR{

	}},
	{"ADD A, C", 1, 4, INSTR{

	}},
	{"ADD A, D", 1, 4, INSTR{

	}},
	{"ADD A, E", 1, 4, INSTR{

	}},
	{"ADD A, H", 1, 4, INSTR{

	}},
	{"ADD A, L", 1, 4, INSTR{

	}},
	{"ADD A, (HL)", 1, 8, INSTR{

	}},
	{"ADD A, A", 1, 4, INSTR{

	}},
	{"ADC A, B", 1, 4, INSTR{

	}},
	{"ADC A, C", 1, 4, INSTR{

	}},
	{"ADC A, D", 1, 4, INSTR{

	}},
	{"ADC A, E", 1, 4, INSTR{

	}},
	{"ADC A, H", 1, 4, INSTR{

	}},
	{"ADC A, L", 1, 4, INSTR{

	}},
	{"ADC A, (HL)", 1, 8, INSTR{

	}},
	{"ADC A, A", 1, 4, INSTR{

	}},
	{"SUB A, B", 1, 4, INSTR{

	}},
	{"SUB A, C", 1, 4, INSTR{

	}},
	{"SUB A, D", 1, 4, INSTR{

	}},
	{"SUB A, E", 1, 4, INSTR{

	}},
	{"SUB A, H", 1, 4, INSTR{

	}},
	{"SUB A, L", 1, 4, INSTR{

	}},
	{"SUB A, (HL)", 1, 8, INSTR{

	}},
	{"SUB A, A", 1, 4, INSTR{

	}},
	{"SBC A, B", 1, 4, INSTR{

	}},
	{"SBC A, C", 1, 4, INSTR{

	}},
	{"SBC A, D", 1, 4, INSTR{

	}},
	{"SBC A, E", 1, 4, INSTR{

	}},
	{"SBC A, H", 1, 4, INSTR{

	}},
	{"SBC A, L", 1, 4, INSTR{

	}},
	{"SBC A, (HL)", 1, 8, INSTR{

	}},
	{"SBC A, A", 1, 4, INSTR{

	}},
	{"AND A, B", 1, 4, INSTR{

	}},
	{"AND A, C", 1, 4, INSTR{

	}},
	{"AND A, D", 1, 4, INSTR{

	}},
	{"AND A, E", 1, 4, INSTR{

	}},
	{"AND A, H", 1, 4, INSTR{

	}},
	{"AND A, L", 1, 4, INSTR{

	}},
	{"AND A, (HL)", 1, 8, INSTR{

	}},
	{"AND A, A", 1, 4, INSTR{

	}},
	{"XOR A, B", 1, 4, INSTR{

	}},
	{"XOR A, C", 1, 4, INSTR{

	}},
	{"XOR A, D", 1, 4, INSTR{

	}},
	{"XOR A, E", 1, 4, INSTR{

	}},
	{"XOR A, H", 1, 4, INSTR{

	}},
	{"XOR A, L", 1, 4, INSTR{

	}},
	{"XOR A, (HL)", 1, 8, INSTR{

	}},
	{"XOR A, A", 1, 4, INSTR{

	}},
	{"OR A, B", 1, 4, INSTR{

	}},
	{"OR A, C", 1, 4, INSTR{

	}},
	{"OR A, D", 1, 4, INSTR{

	}},
	{"OR A, E", 1, 4, INSTR{

	}},
	{"OR A, H", 1, 4, INSTR{

	}},
	{"OR A, L", 1, 4, INSTR{

	}},
	{"OR A, (HL)", 1, 8, INSTR{

	}},
	{"OR A, A", 1, 4, INSTR{

	}},
	{"CP A, B", 1, 4, INSTR{

	}},
	{"CP A, C", 1, 4, INSTR{

	}},
	{"CP A, D", 1, 4, INSTR{

	}},
	{"CP A, E", 1, 4, INSTR{

	}},
	{"CP A, H", 1, 4, INSTR{

	}},
	{"CP A, L", 1, 4, INSTR{

	}},
	{"CP A, (HL)", 1, 8, INSTR{

	}},
	{"CP A, A", 1, 4, INSTR{

	}},
	{"RET NZ", 1, 8, INSTR{

	}},
	{"POP BC", 1, 12, INSTR{

	}},
	{"JP NZ, u16", 3, 12, INSTR{

	}},
	{"JP u16", 3, 16, INSTR{

	}},
	{"CALL NZ, u16", 3, 12, INSTR{

	}},
	{"PUSH BC", 1, 16, INSTR{

	}},
	{"ADD A, u8", 2, 8, INSTR{

	}},
	{"RST 00h", 1, 16, INSTR{

	}},
	{"RET Z", 1, 8, INSTR{

	}},
	{"RET", 1, 16, INSTR{

	}},
	{"JP Z, u16", 3, 12, INSTR{

	}},
	{"PREFIX CB", 1, 4, INSTR{

	}},
	{"CALL Z, u16", 3, 12, INSTR{

	}},
	{"CALL u16", 3, 24, INSTR{

	}},
	{"ADC A, u8", 2, 8, INSTR{
		
	}},
	{"RST 08h", 1, 16, INSTR{

	}},
	{"RET NC", 1, 8, INSTR{

	}},
	{"POP DE", 1, 12, INSTR{

	}},
	{"JP NC, u16", 3, 12, INSTR{

	}},
	{"N/A", 0, 0, INSTR{

	}},
	{"CALL NC, u16", 3, 12, INSTR{

	}},
	{"PUSH DE", 1, 16, INSTR{

	}},
	{"SUB A, u8", 2, 8, INSTR{

	}},
	{"RST 10h", 1, 16, INSTR{

	}},
	{"RET C", 1, 8, INSTR{

	}},
	{"RETI", 1, 16, INSTR{

	}},
	{"JP C, u16", 3, 12, INSTR{

	}},
	{"N/A", 0, 0, INSTR{

	}},
	{"CALL C, u16", 3, 12, INSTR{

	}},
	{"N/A", 0, 0, INSTR{

	}},
	{"SBC A, u8", 2, 8, INSTR{

	}},
	{"RST 18h", 1, 4, INSTR{

	}},
	{"LD (0xFF00+u8), A", 2, 12, INSTR{

	}},
	{"POP HL", 1, 4, INSTR{

	}},
	{"LD (0xFF00+C), A", 1, 8, INSTR{

	}},
	{"N/A", 0, 0, INSTR{

	}},
	{"N/A", 0, 0, INSTR{

	}},
	{"PUSH HL", 1, 16, INSTR{

	}},
	{"AND A, u8", 2, 8, INSTR{

	}},
	{"RST 20h", 1, 16, INSTR{

	}},
	{"ADD SP, i8", 2, 16, INSTR{

	}},
	{"JP HL", 1, 4, INSTR{

	}},
	{"LD (u16), A", 3, 16, INSTR{

	}},
	{"N/A", 0, 0, INSTR{

	}},
	{"N/A", 0, 0, INSTR{

	}},
	{"N/A", 0, 0, INSTR{

	}},
	{"XOR A, u8", 2, 8, INSTR{

	}},
	{"RST 28h", 1, 16, INSTR{

	}},
	{"LD A, (0xFF00+u8)", 2, 12, INSTR{

	}},
	{"POP AF", 1, 12, INSTR{

	}},
	{"LD A, (0xFF00+C)", 1, 8, INSTR{

	}},
	{"DI", 1, 4, INSTR{

	}},
	{"N/A", 0, 0, INSTR{

	}},
	{"PUSH AF", 1, 16, INSTR{

	}},
	{"OR A, u8", 2, 8, INSTR{

	}},
	{"RST 30h", 1, 16, INSTR{

	}},
	{"LD HL, SP+i8", 2, 12, INSTR{

	}},
	{"LD SP, HL", 1, 8, INSTR{

	}},
	{"LD A, (u16)", 3, 16, INSTR{

	}},
	{"EI", 1, 4, INSTR{

	}},
	{"N/A", 0, 0, INSTR{

	}},
	{"N/A", 0, 0, INSTR{

	}},
	{"CP A, u8", 2, 8, INSTR{

	}},
	{"RST 38h", 1, 16, INSTR{

	}}
};
int gbe::cpu_t::instruction_count{sizeof(instructions)/sizeof(*instructions)};