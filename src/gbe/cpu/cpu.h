#pragma once
#include<gbe/mem/mem.h>
#include<gbe/cpu/general_instructions.h>

namespace gbe{
	struct cpu_t{
		//	Public method declaration.
		void emulate_fetch_decode_execute_cycle();
	protected:
		struct{
			byte a{0};
			union{
				struct{
					byte z: 1;
					byte n: 1;
					byte h: 1;
					byte c: 1;
				} bits;
				byte all_bits;
			} f{0};
			byte b{0}, c{0}, d{0}, e{0}, h{0}, l{0};
			word& bc{(word&)b}, &de{(word&)d}, &hl{(word&)h}, &af{(word&)a}; 
			word sp{0}, pc{0};
		} regs;
		struct{
			inline void increment_cycles_m(unsigned long long int m_cycles){
				t_cycles+=m_cycles*4;
				m_cycles+=m_cycles;
			}
			unsigned long long t_cycles{0};
			unsigned long long m_cycles{0};
		} cycles;
		struct{
			inline void increment_timer_and_divider(){
				//	DO NOT FORGET TO IMPLEMENT
			}
			byte timer{0};
			byte divider{0};
		} timer_and_divider;
		mem_t memory;
		//	Instructions definiton.
		struct instruction_t{
			const char* mnemonic;
			int byte_length;
			int t_cycles;
			void(*func)(cpu_t&) noexcept;
		};
		static instruction_t cpu_instructions[];
		static int instruction_count;
		friend struct INSTR_STRUCT_NAME;
		INSTR_STRUCT_NAME instructions {*this};
	};
}