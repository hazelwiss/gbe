#include"cpu.h"

#define INSTR [](gbe::cpu& cpu, gbe::cpu::parameter& param)
#define PARAM_DEST_TO_BYTE param.dest.get_byte()
#define PARAM_DEST_TO_WORD param.dest.get_word()
#define PARAM_SRC_TO_BYTE param.src.get_byte()
#define PARAM_SRC_TO_WORD param.src.get_word()
#define LOAD_MEMORY(adr) cpu.load_memory(adr)
#define WRITE_MEMORY(adr, value) cpu.write_memory(value, adr)
#define LOAD_IMMEDIATE_BYTE LOAD_MEMORY(cpu.regs.pc+1)
#define LOAD_IMMEDIATE_WORD cpu.load_memory_16bit(cpu.regs.pc+1)
#define INCREMENT_WORD_REGISTER(reg) cpu.increment_16bit_register(reg)
#define DECREMENT_WORD_REGISTER(reg) cpu.decrement_16bit_register(reg)
#define WORD_LD(dest, src) cpu.write_to_16bit_register(dest, src)
#define WORD_ALU(dest, src) cpu.alu_16bit_register(dest, src)
#define FLAG_Z 0b1000-0000
#define FLAG_N 0b0100-0000
#define FLAG_H 0b0010-0000
#define FLAG_C 0b0001-0000
#define SET_FLAGS(flags) cpu.set_flags(flags)
#define RESET_FLAGS_ALL cpu.reset_flags_all()
#define RESET_FLAGS_OTHER(exclude) cpu.reset_flags_other(exclude)
#define SET_CONDITIONAL_ZERO_FLAG(val) if(cpu.should_set_zero_flag(val)) SET_FLAGS(FLAG_Z)
#define SET_CONDITIONAL_NEGATIVE_FLAG(left_operator, right_operator) if(cpu.should_set_negative_flag(left_operator, right_operator)) SET_FLAGS(FLAG_N)
#define SET_CONDITIONAL_HALF_CARRY_FLAG(left_operator, right_operator) if(cpu.should_set_half_carry_flag(left_operator, right_operator)) SET_FLAGS(FLAG_H)
#define SET_CONFITIONAL_CARRY_FLAG(left_operator, right_operator) if(cpu.should_set_carry_flag(left_operator, right_operator)) SET_FLAGS(FLAG_C)
#define SET_CONDITIONAL_NOT_HALF_CARRY_FLAG(left_operator, right_operator) if(!cpu.should_set_half_carry_flag(left_operator, right_operator)) SET_FLAGS(FLAG_H)
#define SET_CONDITIONAL_NOT_CARRY_FLAG(left_operator, right_operator) if(!cpu.should_set_carry_flag(left_operator, right_operator)) SET_FLAGS(FLAG_C)

/*
	Cycle emulation implementation.
*/
void gbe::cpu::emulate_fetch_decode_execute(){
	//	Nothing yet!
	//	Call function here!
	parameter param;
	this->clock_ticks+=4;
	instructions[this->regs.pc++%cpu::instruction_c](*this, param);
}

/*
	Memory functions
*/
const ubyte& gbe::cpu::load_memory(const uword& address){
	this->clock_ticks+=4;
}
const uword& gbe::cpu::load_memory_16bit(const uword& address){
	this->clock_ticks+=8;
}
void gbe::cpu::write_memory(const ubyte& data, const uword& address){
	this->clock_ticks+= 4;
}
void gbe::cpu::write_memory_16bit(const uword& data, const uword& address){
	this->clock_ticks+= 8;
}

/*
	Instructions implementation
*/
const gbe::cpu::instruction_t gbe::cpu::instructions[]{
	/*
		8-BIT LOAD INSTRUCTIONS
	*/
	INSTR{	//	LD nn, n : 8 cycles, 
		PARAM_DEST_TO_BYTE = LOAD_IMMEDIATE_BYTE;	//	Or maybe it's -1?!
	},
	INSTR{	//	LD r1, r2 : 4-8 cycles
		PARAM_DEST_TO_BYTE = PARAM_SRC_TO_BYTE;
	},
	INSTR{	//	LD A, n : 4, 8 - 16 cycles
		cpu.regs.a = PARAM_SRC_TO_BYTE;
	},
	INSTR{	// LD n, A : 4, 8 - 16
		PARAM_DEST_TO_BYTE = cpu.regs.a;
	},
	INSTR{	//	LD A, (0xFF00+C) : 8 cycles
		cpu.regs.a = LOAD_MEMORY(0xFF00+cpu.regs.c);
	},
	INSTR{	//	LD (0xFF00+C), A : 8 cycles
		WRITE_MEMORY(0xFF00+cpu.regs.c, cpu.regs.a);
	},
	INSTR{	//	LDD A, (HL) : 8 cycles
		cpu.regs.a = LOAD_MEMORY(DECREMENT_WORD_REGISTER(cpu.regs.hl));
	},
	INSTR{	//	LDD (HL), A : 8 cycles
		WRITE_MEMORY(DECREMENT_WORD_REGISTER(cpu.regs.hl), cpu.regs.a);
	},
	INSTR{	//	LDI A, (HL) : 8 cycles
		cpu.regs.a = LOAD_MEMORY(INCREMENT_WORD_REGISTER(cpu.regs.hl));
	},
	INSTR{	//	LDI (HL), A : 8 cycles
		WRITE_MEMORY(INCREMENT_WORD_REGISTER(cpu.regs.hl), cpu.regs.a);
	},
	INSTR{	//	LDH (n), A : 12 cycles
		WRITE_MEMORY(0xFF00+LOAD_IMMEDIATE_BYTE, cpu.regs.a);
	},
	INSTR{	//	LDH A, (n) : 12 cycles
		cpu.regs.a = LOAD_MEMORY(0xFF00+LOAD_IMMEDIATE_BYTE);
	},
	/*
		16-BIT LOAD INSTRUCTIONS
	*/
	INSTR{	//	LD n, nn : 12 cycles
		WORD_LD(PARAM_DEST_TO_WORD, PARAM_SRC_TO_WORD);
	},
	INSTR{	//	LD SP, HL : 8 cycles
		WORD_LD(cpu.regs.sp, cpu.regs.hl);
	},
	INSTR{	//	LDHL SP, n : 12 cycles
		WORD_LD(cpu.regs.hl, cpu.regs.sp+LOAD_IMMEDIATE_BYTE);
	},
	INSTR{	//	LD (nn), SP : 20 cycles
		cpu.write_memory_16bit(LOAD_IMMEDIATE_WORD, cpu.regs.sp);
	},
	INSTR{	//	PUSH nn : 16 cycles
		WORD_ALU(cpu.regs.sp, -2);
		cpu.write_memory_16bit(cpu.regs.sp, PARAM_DEST_TO_WORD);
	},
	INSTR{	//	POP nn : 12 cycles
		//	Not very happy with this solution? The POP instruction seems to be like two byte memloads with the stack pointer incrementing after each one?
		((uword&)PARAM_DEST_TO_WORD) = 0;
		for(int i{1}; i; --i){
			((uword&)PARAM_DEST_TO_WORD) &= LOAD_MEMORY(cpu.regs.sp) & (0xFF << (8*i));
			cpu.increment_16bit_register(cpu.regs.sp);
		}
	},
	/*
		8-BIT ALU
	*/
	INSTR{	//	ADD A, n : 4 - 8 cycles
		RESET_FLAGS_ALL;
		SET_CONDITIONAL_HALF_CARRY_FLAG(cpu.regs.a, PARAM_SRC_TO_BYTE);
		SET_CONFITIONAL_CARRY_FLAG(cpu.regs.a, PARAM_SRC_TO_BYTE);
		cpu.regs.a += PARAM_SRC_TO_BYTE;
		SET_CONDITIONAL_ZERO_FLAG(cpu.regs.a);
		//	Add flag stuff! half carry and carry flag!
	},
	INSTR{	//	ADC A, n : 4 - 8 cycles
		RESET_FLAGS_ALL;
		ubyte val = PARAM_SRC_TO_BYTE + ((cpu.regs.f & FLAG_C) ? 1 : 0);
		SET_CONDITIONAL_HALF_CARRY_FLAG(cpu.regs.a, val);
		SET_CONFITIONAL_CARRY_FLAG(cpu.regs.a, val);
	},
	INSTR{	//	SUB A, n : 4 - 8 cycles	
		RESET_FLAGS_ALL;
		SET_FLAGS(FLAG_N);
		SET_CONDITIONAL_NOT_HALF_CARRY_FLAG(cpu.regs.a, PARAM_SRC_TO_BYTE);
		SET_CONDITIONAL_NOT_CARRY_FLAG(cpu.regs.a, PARAM_SRC_TO_BYTE);
		cpu.regs.a -= PARAM_SRC_TO_BYTE;
		SET_CONDITIONAL_ZERO_FLAG(cpu.regs.a);
	},
	INSTR{	//	SBC A, n : 4 - 8 cycles
		RESET_FLAGS_ALL;
		SET_FLAGS(FLAG_N);
		SET_CONDITIONAL_NOT_HALF_CARRY_FLAG(cpu.regs.a, PARAM_SRC_TO_BYTE);
		SET_CONDITIONAL_NOT_CARRY_FLAG(cpu.regs.a, PARAM_SRC_TO_BYTE);
		cpu.regs.a -= PARAM_SRC_TO_BYTE;
		SET_CONDITIONAL_ZERO_FLAG(cpu.regs.a);
	},
	INSTR{	//	AND A, n : 4 - 8 cycles
		RESET_FLAGS_ALL;
		SET_FLAGS(FLAG_H);
		cpu.regs.a &= PARAM_SRC_TO_BYTE;
		SET_CONDITIONAL_ZERO_FLAG(cpu.regs.a);
	},
	INSTR{	//	OR A, n : 4 - 8 cycles
		RESET_FLAGS_ALL;
		cpu.regs.a |= PARAM_SRC_TO_BYTE;
		SET_CONDITIONAL_ZERO_FLAG(cpu.regs.a);
	},
	INSTR{	//	XOR A, n : 4 - 8 cycles
		RESET_FLAGS_ALL;
		cpu.regs.a ^= PARAM_SRC_TO_BYTE;
		SET_CONDITIONAL_ZERO_FLAG(cpu.regs.a);
	},
	INSTR{	//	CP A, n : 4 - 8 cycles
		RESET_FLAGS_ALL;
		ubyte tmp_cp_val = cpu.regs.a - PARAM_SRC_TO_BYTE;
		SET_FLAGS(FLAG_Z);
		SET_CONDITIONAL_ZERO_FLAG(tmp_cp_val);
		if(0b0001-1111-cpu.regs.a < PARAM_SRC_TO_BYTE)
			SET_FLAGS(FLAG_H);
		if(cpu.regs.a < PARAM_SRC_TO_BYTE)
			SET_FLAGS(FLAG_C);
	},
	INSTR{	//	INC A, n : 4 - 12
		
	}
};
const int gbe::cpu::instruction_c{sizeof(gbe::cpu::instructions)/sizeof(gbe::cpu::instruction_t)};