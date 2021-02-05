#pragma once
#include<gbe/mem/mem.h>
#include<gbe/ppu/ppu.h>
#include<gbe/cpu/general_instructions.h>
#include<iostream>
#include<thread>

namespace gbe{
	struct cpu_t{
		//	Public method declaration.
		void emulate_fetch_decode_execute_cycle();
		void create_window(){
			this->ppu.create_window();
		}
		void load_ROM(const char* file);
	protected:
		void check_interrupt_status();
		struct{
			union{
				struct{
					byte unused: 4;
					byte c: 1;
					byte h: 1;
					byte n: 1;
					byte z: 1;
				} bits;
				byte all_bits;
			} f{0};
			byte a{0};
			byte c{0}, b{0}, e{0}, d{0}, l{0}, h{0};
			word& bc{(word&)c}, &de{(word&)e}, &hl{(word&)l}, &af{(word&)f}; 
			word sp{0}, pc{0};
		} regs;
		struct{
			inline void increment_cycles_m(unsigned long long int m_cycles){
				this->t_cycles+=m_cycles*4;
				this->m_cycles+=m_cycles;
			}
			inline void increment_cycles_t(unsigned long long int t_cycles){
				this->t_cycles+=t_cycles;
				this->m_cycles+=(t_cycles/4);
			}
			unsigned long long t_cycles{0};
			unsigned long long m_cycles{0};
		} cycles;
		mem_t memory;
		ppu_t ppu{memory};
		std::chrono::_V2::system_clock::time_point sync_time;
		//	Instructions definiton.
		struct instruction_t{
			const char* mnemonic;
			int byte_length;
			int t_cycles;
			bool(*func)(cpu_t&);	//	returns whether or not the pc should be incremented.
		};
		static instruction_t cpu_instructions[];
		static int instruction_count;
		friend struct INSTR_STRUCT_NAME;
		INSTR_STRUCT_NAME instructions {*this};
		void print_regs(gbe::cpu_t::instruction_t&);	// tmp
	};
}