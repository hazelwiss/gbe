#pragma once
#include<gbe/mem/mem.h>
#include<gbe/ppu/display.h>

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
	struct oam_memory_t{
		byte y;
		byte x;
		byte tile;
		byte flags;
		byte is_x16{false};
	};

	struct ppu_t{
		ppu_t(mem_t& mem): memory{mem} {}
		void create_window(){
			display.create_window();
		}
		void update(byte cycles);
	protected:
		void update_background_in_row();
		void update_window_in_row();
		void update_sprite_in_row();
		void increment_ly();
		void v_blank_interrupt();
		void stat_interrupt();
		void reset();
		byte get_tile_index(byte x, byte y, bool mode_9C00){
			return memory.mem[
				(mode_9C00 ? 0x9C00 : 0x9800)+
				(x%32)+
				((y%32)*32)];
		}
		word get_tile_data(byte tile_index, byte y, bool mode_8000){
			if(mode_8000)
				return
					(memory.mem[0x8000+tile_index*16 + (y%8)*2] << 8) | 
					memory.mem[0x8000+tile_index*16 + (y%8)*2 + 1];
			return
				(memory.mem[0x9000+((signed char)tile_index)*16 + (y%8)*2] << 8) | 
				memory.mem[0x9000+((signed char)tile_index)*16 + (y%8)*2 + 1];
		}
		void change_ppu_mode(byte mode){
			this->memory.lcd_status_register = this->memory.lcd_status_register&(~0b11);
			this->memory.lcd_status_register |= mode & 0b11;
		}
		//	returns amount of sprite oam data in a scanline/row.
		byte get_sprites_in_row(oam_memory_t data_reference[40], byte y);	
		mem_t& memory;
		display_t display; 
		int dots{0};
		int wl{0};
		int scanline_oam_time_delay{0};
		//typedef std::chrono::duration<unsigned long long int, std::ratio<238, 1000000000>> gb_cycle_time;
		//std::chrono::high_resolution_clock sleep_dur;
		byte& lcd_control{memory.mem.fetch_from_address((word)reserved_memory_locations_enum::LCD_CONTROL_REGISTER)};
		byte& lcd_status{memory.mem.fetch_from_address((word)reserved_memory_locations_enum::LCD_STATUS_REGISTER)};
		byte& scy{memory.mem.fetch_from_address(0xFF42)};
		byte& scx{memory.mem.fetch_from_address(0xFF43)};
		byte& ly{memory.mem.fetch_from_address(0xFF44)};
		byte& lcy{memory.mem.fetch_from_address(0xFF45)};
		byte& wy{memory.mem.fetch_from_address(0xFF4A)};
		byte& wx{memory.mem.fetch_from_address(0xFF4B)};
		byte& bgp{memory.mem[0xFF47]};
		byte& obp0{memory.mem[0xFF48]};
		byte& obp1{memory.mem[0xFF49]};
	};
}

