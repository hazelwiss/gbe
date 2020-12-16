#pragma once
#include<gbe/cpu/cpu.h>

namespace gbe{
	struct instructions{
		inline instructions(gbe::mem_t& mem): mem{mem}, cpu{cpu} {}
		gbe::mem_t& mem;
		gbe::cpu_t& cpu;

		// 8-Bit-Loads
		void b8_load(word adr, byte& reg);

		// 16-Bit-Loads
		void b16_load(word, word&);
		//	give the offset and not the offset + sp or the sp register. 
		void b16_load_sp_e(byte, word&);	//	16 bit load but with flags. Only really one type of instruction does this

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
		void b8_dec(byte&);

		//	16-Bit-Arithmetic
		void b16_add(word);
		void b16_add_sp(byte);
		void b16_inc(word&);
		void b16_dec(word&);

		//	Miscellaneous
		void misc_swap();
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
		void misc_rlca();
		void misc_rla();
		void misc_rrca();
		void misc_rra();
		void misc_rlc();
		void misc_rl();
		void misc_rrc();
		void misc_rr();
		void misc_sla();
		void misc_srl();

		//	Bit opcodes
		void b1_bit();
		void b1_set();
		void b1_res();

		//	Jumps
		void jp_nn();
		void jp_cc_nn();
		void jp_hl();
		void jp_e();
		void jp_cc_e();

		//	Calls
		void call_nn();
		void call_cc_nn();

		//	Restarts
		void rst();

		//	Returns
		void ret();
		void ret_cc();
		void ret_i();
	protected:		
		/*	
			bit excessive with these functions but... well... I am quite excessive, so that's my excuse.
		*/
		inline void reset_all_flags(){
			this->cpu.regs.f.all_bits = 0;
		}
		inline void set_z_flag(int val){
			this->cpu.regs.f.bits.z = (val != 0);
		}
		inline void set_n_flag(int val){
			this->cpu.regs.f.bits.n = (val != 0);
		}
		inline void set_h_flag(int val){
			this->cpu.regs.f.bits.h = (val != 0);
		}
		inline void set_c_flag(int val){
			this->cpu.regs.f.bits.c = (val != 0);
		}
	};
}



