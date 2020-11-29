#pragma once
#include<gbe/mem/mem.h>

namespace gbe{
	struct cpu_t{
		//	Public method declaration.
		void emulate_fetch_decode_execute_cycle();
	protected:
		struct{
			byte a{0}, f{0};
			byte b{0}, c{0}, d{0}, e{0}, h{0}, l{0};
			word& bc{(word&)b}, &de{(word&)d}, &hl{(word&)h}, &af{(word&)a}; 
		} regs;
		struct{
			inline const byte& read_byte(){}
			inline const word& read_word(){}
			inline void write_byte(){}
			inline void write_word(){}
		protected:
			mem_t memory;
		} mem_hander;
		struct{
			inline void update_cycles_m(unsigned long long int m_cycles){
				t_cycles+=m_cycles*4;
				m_cycles+=m_cycles;
			}
			unsigned long long t_cycles{0};
			unsigned long long m_cycles{0};
		} cycles;

		//	Instructions definiton.
		struct instruction_t{
			const char* mnemonic;
			int byte_length;
			int t_cycles;
			void(*func)(cpu_t&) noexcept;
		};
		static instruction_t instructions[];
		static int instruction_count;
	};
}