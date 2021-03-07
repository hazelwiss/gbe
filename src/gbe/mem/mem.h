#pragma once
#include"banks.h"
#define internal_memory_size (0xFFFF - 0x7FFF - (0xBFFF-0xA000))

namespace gbe{
	enum class reserved_memory_locations_enum : word{
		CARTRIDGE_TYPE = 0x0147,
		ROM_SIZE = 0x0148,
		RAM_SIZE = 0x0149,
		DIVIDER_REGISTER = 0xFF04,
		TIMER_COUNTER = 0xFF05,
		TIMER_MODULO = 0xFF06,
		TIMER_CONTROL = 0xFF07,
		DISABLE_BOOT_ROM = 0xFF50,
		LCD_CONTROL_REGISTER = 0xFF40,
		LCD_STATUS_REGISTER = 0xFF41,
		CGB_MODE_ONLY =  0xFF4D,
		DMA_TRANSFER = 0xFF46,
		INTERRUPT_FLAG = 0xFF0F,
		INTERRUPT_ENABLE = 0xFFFF
	};
	enum class interrupt_bits: byte{
		V_BLANK = 0b1,
		LCD_STAT = 0b10,
		TIMER = 0b100,
		SERIAL = 0b1000,
		Joypad = 0b1'0000
	};
	struct rom_type_info_t{
		rom_type_info_t(){}
		rom_type_info_t(int mode, int rom_size, int ram_size): mode{mode}, rom_size{rom_size}, ram_size{ram_size}{}
		int mode;
		int rom_size;
		int ram_size;
	};
	struct mem_t{
		void load_ROM(const char*);
		void write_byte_to_memory(word adr, byte value);
		byte read_byte_from_memory(word adr);
		void write_word_to_memory(word adr, word value);
		word read_word_from_memory(word adr);
		void dma_transfer(byte cycles);
		void set_interrupt_flag(byte val){
			this->interrupt_flag = val;
		}
		byte get_interrupt_flag(){
			return this->interrupt_flag;
		}
		void set_interrupt_enable(byte val){
			this->interrupt_enable = val;
		}
		byte get_interrupt_enable(){
			return this->interrupt_enable;
		}
		void enable_ime(){
			this->ime = 1;
		}
		void disable_ime(){
			this->ime = 0;
		}
		byte get_ime(){
			return this->ime;
		}
		void mount_boot_rom(){
			is_boot_rom_mounted = true;
		}
		void unmount_boot_rom(){
			is_boot_rom_mounted = false;
			mem[(int)gbe::reserved_memory_locations_enum::DISABLE_BOOT_ROM] = 1;;
		}
		bool get_boot_rom_mount_status(){
			return is_boot_rom_mounted;
		}
		void increment_timer(unsigned long long int& cycles);
		void request_interrupt(byte interrupt){
			this->interrupt_flag |= interrupt;
		}
		rom_type_info_t get_rom_info(){
			return rom_info;
		}
	protected: 
		rom_type_info_t rom_info;
		//	hinders cpu to read during certain scanline periods.
		bool is_ppu_blocking(const word& adr);
		bool is_dma_transferring_blocking(word& adr){
			return false;
			return is_dma_transfer && !(adr >= 0xFF80 && adr <= 0xFFFE);
		}
		//	handles specific cases when writing outside of the memory banks. 
		//	Otherwise just write the value to whatever given adress.
		void write_to_internal_memory(const word& adr, byte value);
		byte read_byte_from_internal_memory(word adr);
		word read_word_from_internal_memory(word adr){
			return read_byte_from_internal_memory(adr) | (read_byte_from_internal_memory(adr+1) << 8);
		}
		struct mem_controller_t{
			virtual ~mem_controller_t(){
				if(bank_controller)
					delete bank_controller;
			}
			void write_byte(const word& adr, byte val){
				this->bank_controller->write_mem(adr, val);
			}
			void write_word(const word& adr, word val){
				this->bank_controller->write_mem(adr, val);
				this->bank_controller->write_mem(adr+1, val >> 8);
			}
			byte read_byte(const word& adr){
				return this->bank_controller->read_mem(adr);
			}
			word read_word(const word& adr){
				word rtrn = this->bank_controller->read_mem(adr) | (this->bank_controller->read_mem(adr+1) << 8);
				return rtrn;
			}
			void copy_rom(byte* rom, int size){
				this->bank_controller->copy_rom(rom, size);
			}
			void set_bank_type(const int& type, const int& rom_size, const int& ram_size);
		protected:
			int type{0};
			memory_bank_controller_t* bank_controller{0};
		} mem_bank_controller;
		struct{
			byte& fetch_from_address(int index) const{
				if(index > 0xA000)
					index -= (0xBFFF-0xA000);
				index -= 0x7FFF;
				return (byte&)mem[index];
			}			
			byte& operator[](int index) const{
				return fetch_from_address(index);
			}
		protected:
			byte mem[internal_memory_size+1]{0};	//	Because we're not emulating GBC, we don't have to make WRAM exist in a bank but rather as internal memory.
		} mem;
		bool is_boot_rom_mounted{false};
		bool is_dma_transfer{false};
		word dma_transfer_source_beg{0};
		word dma_transfer_source_end{0};
		byte increment_divider_tmp_value{0};
		byte increment_timer_tmp_value{0};
		bool timer_overflow = false;
		byte ime{0};
		static byte boot_rom[];
		byte& interrupt_enable{mem[static_cast<int>(reserved_memory_locations_enum::INTERRUPT_ENABLE)]};
		byte& interrupt_flag{mem[static_cast<int>(reserved_memory_locations_enum::INTERRUPT_FLAG)]};
		byte& divider_reg{mem[static_cast<int>(reserved_memory_locations_enum::DIVIDER_REGISTER)]};
		byte& timer_counter{mem[static_cast<int>(reserved_memory_locations_enum::TIMER_COUNTER)]};
		byte& timer_control{mem[static_cast<int>(reserved_memory_locations_enum::TIMER_CONTROL)]};
		byte& timer_modulo{mem[static_cast<int>(reserved_memory_locations_enum::TIMER_MODULO)]};
		byte& lcd_control_register{mem[static_cast<int>(reserved_memory_locations_enum::LCD_CONTROL_REGISTER)]};
		byte& lcd_status_register{mem[static_cast<int>(reserved_memory_locations_enum::LCD_STATUS_REGISTER)]};
		friend struct ppu_t;
	};
}