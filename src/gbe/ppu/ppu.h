#pragma once
#include<gbe/mem/mem.h>
#include<gbe/ppu/display.h>
#define X_RES 160
#define Y_RES 144

namespace gbe{
	/*
	struct gbe_clock{
		typedef std::chrono::nanoseconds 			duration;
		typedef duration::rep						rep;
		typedef duration::period					period;
		typedef std::chrono::time_point<gbe_clock>	time_point;
		static const bool is_steady =				false;
		unsigned long long int cycles{0};
		static time_point now() noexcept{
			using namespace std::chrono;
			//return(
			//	duration_cast<duration>(system_clock::now());
			//)
		}
	};
	*/
	struct ppu_t{
		ppu_t(mem_t& mem): memory{mem} {
			//memory.lcd_status_register = 0b10;
			ly = 144;
			memory.lcd_status_register = 0b11;
			//sleep_dur = std::chrono::high_resolution_clock::now();
		}
		void create_window(){
			display.create_window();
		}
		void update(byte cycles);
	protected:
		void mode_3(byte& cycles);
		void mode_2(byte& cycles);
		void mode_1(byte& cycles);
		void mode_0(byte& cycles);
		void change_mode(byte mode){
			this->memory.lcd_status_register = 0b11;
			this->memory.lcd_status_register |= mode & 0b11;
		}
		mem_t& memory;
		display_t display; 
		word dots{0};			//	current dot
		byte screen[5760]{0};	//	bytes to send to sdl for rendering the window.
		byte oam_buffer[1]{0};
		//typedef std::chrono::duration<unsigned long long int, std::ratio<238, 1000000000>> gb_cycle_time;
		//std::chrono::high_resolution_clock sleep_dur;
		byte& lcd_control{memory.mem.fetch_from_address((word)reserved_memory_locations_enum::LCD_CONTROL_REGISTER)};
		byte& lcd_status{memory.mem.fetch_from_address((word)reserved_memory_locations_enum::LCD_STATUS_REGISTER)};
		byte& scy{memory.mem.fetch_from_address(0xFF42)};
		byte& ly{memory.mem.fetch_from_address(0xFF44)};
		byte& lcy{memory.mem.fetch_from_address(0xFF45)};
		byte& wy{memory.mem.fetch_from_address(0xFF4A)};
	};
}

