#pragma once
#include<gbe/mem/mem.h>

namespace gbe{
	struct cpu{
		void emulate_fetch_decode_execute();
	protected:
		//	Method declaration
		const ubyte& load_memory(const uword& address);
		const uword& load_memory_16bit(const uword& address);
		void write_memory(const ubyte& data, const uword& address);
		void write_memory_16bit(const uword& data, const uword& address);
		inline void write_to_16bit_register(const uword& dest, const uword src){
			this->clock_ticks+=4;
			*((uword*)&dest) = src;
		}
		inline void alu_16bit_register(const uword& dest, const int src){
			((uword&)dest)+=src;
		}
		inline uword increment_16bit_register(const uword& reg){
			return ((uword&)reg)++;
		}
		inline uword decrement_16bit_register(const uword& reg){
			return ((uword&)reg)--;
		}
		//	Flag helper functions. Didn't make them into a macro because I don't want to spam macros more than I already do.
		inline void reset_flags_all(){
			this->regs.f = 0;
		}
		inline void reset_flags_other(const ubyte& exclude){
			this->regs.f &= exclude;
		}
		inline void set_flags(const ubyte& flags){
			this->regs.f |= flags;
		}
		template<typename t>
		inline bool should_set_zero_flag(const t& v){
			return v;
		}
		template<typename t, typename _t>
		inline bool should_set_negative_flag(const t& left_operator, const _t& right_operator){
			int v1 = left_operator, v2 = right_operator;
			return (left_operator-right_operator >= 0) ? true : false;
		}
		template<typename t, typename _t>
		inline bool should_set_half_carry_flag(const t& left_operator, const _t& right_operator){
			return ((left_operator&0x0F) + (right_operator&0x0F) > 0x0F);
		}
		template<typename t, typename _t>
		inline bool should_set_carry_flag(const t& left_operator, const _t& right_operator){
			return (0xFF-left_operator < right_operator);
		}
		//	Member declarations.
		struct{
			ubyte a{0}, f{0};
			uword pc{0}, sp{0};
			ubyte b{0}, c{0}, d{0}, e{0}, h{0}, l{0};
			const uword& bc{(uword&)b}, &de{(uword&)d}, &hl{(uword&)h}; 
		} regs;
		mem_t mem;
		unsigned long long int clock_ticks{0};
		//	Nested definition of the parameters used within the instruction_t type, that is used by the cpu instructions.
		struct parameter{
			template<typename byte_t, typename word_t>
			struct reg{
				enum { BYTE = 0, WORD } TYPE_FLAG;
				inline byte_t& get_byte(){
					this->TYPE_FLAG = BYTE;
					return r.b;
				}
				inline const word_t& get_word(){
					this->TYPE_FLAG = WORD;
					return r.w;
				}
			protected:
				union {
					byte_t b;
					const word_t w;
				} r{0};
			};
			reg<ubyte, uword> dest; 
			reg<const ubyte, const uword> src;
		};
		//	Nested definiton of the instruction types and function table.
		using instruction_t = void(*)(cpu&, parameter&);
		static const instruction_t instructions[];
		static const int instruction_c;
	};
}