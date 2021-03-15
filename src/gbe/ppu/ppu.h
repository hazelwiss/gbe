#pragma once
#include<gbe/mem/mem.h>
#include<gbe/ppu/display.h>
#define PIXELS_IN_FIFO 8
#define SPRITES_IN_OAM 40

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
	};

	struct fifo_pixel{
		byte colour{0};
		byte palette{0};
		byte backround_priority{0};
	};

	struct tile{
		union{
			word tile_data{0};
			struct{
				byte tile_data_high;
				byte tile_data_low;
			};
		};
	};

	struct fetcher{
		byte cur_step{0};
		byte x = 0;
		tile cur_tile{0};
		void inc_step(){
			cur_step = (cur_step+1)%5;
		}
		void flush(){
			cur_tile.tile_data = 0;
		}
	};

	struct fifo{
		fifo_pixel pixels[PIXELS_IN_FIFO];
		void flush(){
			memset(pixels, 0, sizeof(pixels));
		}
	};

	struct ppu_t{
		ppu_t(mem_t& mem): memory{mem} {}
		void create_window(){
			display.create_window();
		}
		void update();
	protected:
		void set_fifo_pixels_with_tile_data(gbe::fifo& f, byte palette, byte priority, int shift);
		void set_bg_fifo_tile(fifo& f, byte x, byte y, byte palette, bool mode_9C00, bool mode_8000);
		tile get_sprite_fifo_tile(byte tile_index, byte y, bool x_flip, bool is_16_height);
		tile get_bg_fifo_tile(byte x, byte y, byte palette, bool mode_9C00, bool mode_8000);
		void get_background_tile();
		void push_background_fifo();
		void update_sprite_fifo();
		void update_bg_and_wn();
		void v_blank();
		void stat_interrupt();
		void reset();
		void reset_fifos();
		void change_ppu_mode(byte mode){
			this->memory.lcd_status_register &= ~0b11;
			this->memory.lcd_status_register |= mode&0b11;
		}
		void fire_lcd_interrupt(){
			if(lcd_interrupt_fired == 0){
				memory.request_interrupt((byte)interrupt_bits::LCD_STAT);
				lcd_interrupt_fired = true;
			}
		}
		void on_oam_end();
		void on_new_scanline();
		void on_draw_end();
		void on_oam();
		void on_draw();
		fifo background_fifo;
		fifo sprite_fifo;
		fetcher pixel_fetcher;
		mem_t& memory;
		display_t display; 
		oam_memory_t sprite_buffer[10];
		int sprite_buffer_size{0};
		int dots{0};
		int wl{0};
		bool lcd_interrupt_fired = false;
		bool window_has_drawn_pixel = false;
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

