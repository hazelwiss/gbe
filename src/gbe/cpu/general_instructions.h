#pragma once
#include<standard_lib/typedefs.h>
#define INSTR_STRUCT_NAME general_instructions_t

namespace gbe{
	struct cpu_t;
	struct mem_t;
	struct INSTR_STRUCT_NAME{
		INSTR_STRUCT_NAME(gbe::cpu_t& cpu);

		// 8-Bit-Loads
		void b8_load_reg_val(byte&, byte);
		void b8_load_reg_adr(byte&, word);
		void b8_load_adr_val(word, byte);

		// 16-Bit-Loads
		void b16_load_reg_val(word&, word);
		void b16_load_reg_adr(word&, word);
		void b16_load_adr_val(word, word);
		//	give the offset and not the offset + sp or the sp register. 
		void b16_load_sp_e(word&, byte);	//	16 bit load but with flags. Only really one type of instruction does this
		void b16_push(word);
		void b16_pop(word&);

		//	8-Bit-Alu
		void b8_add(byte);
		void b8_adc(byte);
		void b8_sub(byte);
		void b8_sbc(byte);
		void b8_and(byte);
		void b8_or(byte);
		void b8_xor(byte);
		void b8_cp(byte);
		void b8_inc(byte&);
		void b8_inc_adr(word);
		void b8_dec(byte&);
		void b8_dec_adr(word);

		//	16-Bit-Arithmetic
		void b16_add(word);
		void b16_add_sp(signed char);
		void b16_inc(word&);
		void b16_dec(word&);

		//	Miscellaneous
		void misc_swap(byte&);
		void misc_daa();
		void misc_cpl();
		void misc_ccf();
		void misc_scf();
		void misc_nop();
		void misc_halt();
		void misc_stop();
		void misc_di();
		void misc_ei();

		//	Rotates And Shifts
		void misc_rlc(byte&);
		void misc_rl(byte&);
		void misc_rrc(byte&);
		void misc_rr(byte&);
		void misc_sla(byte&);
		void misc_sra(byte&);
		void misc_srl(byte&);

		//	Bit opcodes
		void b1_bit();
		void b1_set();
		void b1_res();

		//	Jumps
		void jp_nn(word);
		void jp_cc_nn(word, byte);
		void jp_hl();
		void jr(signed char);
		void jr_cc(signed char, byte);

		//	Calls
		void call_nn(word);
		void call_cc_nn(word, byte);

		//	Restarts
		void rst(byte);

		//	Returns
		void ret();
		void ret_cc(byte);
		void reti();
	protected:		
		/*	
			bit excessive with these functions but... well... I am quite excessive, so that's my excuse.
		*/
		gbe::cpu_t& cpu;
		gbe::mem_t& mem;
		inline void reset_all_flags();
		inline void set_z_flag(int val);
		inline void set_n_flag(int val);
		inline void set_h_flag(int val);
		inline void set_c_flag(int val);
		void push_to_stack(word val);
		word pop_from_stack();
	};
}



