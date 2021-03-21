#pragma once
#include<gbe/mem/mem.h>
#include<gbe/ppu/ppu.h>
#include<gbe/cpu/general_instructions.h>
#include<iostream>
#include<thread>
#include<fstream>
#include<sstream>

struct to_hex_t{
	to_hex_t(byte v): _v{v}{};
	byte _v;
};

inline to_hex_t hex(byte v){
	return to_hex_t(v);
}

inline std::ostream& operator<<(std::ostream& o, const to_hex_t& hex){
	o << std::hex << std::uppercase;
	if(hex._v & 0xF0) 
		o << (int)hex._v;
	else
		o << 0 << (int)hex._v;
	return o;
}

namespace gbe{
	struct cpu_t{
		cpu_t(){
			keyboard = SDL_GetKeyboardState(NULL);
		}
		//	Public method declaration.
		void emulate_fetch_decode_execute_cycle();
		void create_window(){
			this->ppu.create_window();
		}
		void load_ROM(const char* file);
		void print_rom_info();
		std::ofstream output_stream_test{"test-log.txt", std::ofstream::trunc};
		std::stringstream stream_test;
		void print_regs(){
			stream_test << "A: ";
			stream_test << hex(regs.a);
			stream_test << " ";
			stream_test << "F: ";
			stream_test << hex(regs.f.all_bits);
			stream_test << " ";
			stream_test << "B: ";
			stream_test << hex(regs.b);
			stream_test << " ";
			stream_test << "C: ";
			stream_test << hex(regs.c);
			stream_test << " ";
			stream_test << "D: ";
			stream_test << hex(regs.d);
			stream_test << " ";
			stream_test << "E: ";
			stream_test << hex(regs.e);
			stream_test << " ";
			stream_test << "H: ";
			stream_test << hex(regs.h);
			stream_test << " ";
			stream_test << "L: ";
			stream_test << hex(regs.l);
			stream_test << " ";
			stream_test << "SP: ";
			stream_test << hex(regs.sp >> 8) << hex(regs.sp);
			stream_test << " ";
			stream_test << "PC: 00:";
			stream_test << hex(regs.pc >> 8) << hex(regs.pc&0xFF);
			stream_test << " (";
			stream_test << hex(memory.read_byte_from_memory(regs.pc));
			stream_test << " ";
			stream_test << hex(memory.read_byte_from_memory(regs.pc+1));
			stream_test << " ";
			stream_test << hex(memory.read_byte_from_memory(regs.pc+2));
			stream_test << " ";
			stream_test << hex(memory.read_byte_from_memory(regs.pc+3));
			stream_test << ")";
			stream_test << " " << cpu_instructions[memory.read_byte_from_memory(regs.pc)].mnemonic;
			stream_test << "\n";
		}
		void output_print_to_file(){
			output_stream_test << stream_test.rdbuf();
			output_stream_test.close();
		}
		void flush_print_stream_to_console_and_empty(){
			std::cout << stream_test.rdbuf();
			stream_test.str(std::string());
			stream_test.clear();
		}
	protected:
		void check_interrupt_status();
		void check_joypad_input();
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
		int cur_cycles{0};
		int cur_instruction_cycles{0};
		bool halted = false;
		bool stopped = false;
		const byte* keyboard;
		mem_t memory;
		ppu_t ppu{memory};
		bool request_e_interrupt{0}, request_d_interrupt{0}, request_handle{0};
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
	};
}