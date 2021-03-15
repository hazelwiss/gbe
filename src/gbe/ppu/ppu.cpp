#include"ppu.h"
#include<cstring>
#include<thread>
#define MAX_DOTS 70224
#define DOTS_PER_SCANLINE 456
#define MAX_SCANLINES 154
#define OAM_SCAN_BASE_TIME 248
#define SCANNING_OAM_TIME 80
#define MAX_SPRITES_ON_SCANLINE 10
#define MODE_0 0
#define MODE_1 1
#define MODE_2 2
#define MODE_3 3
#define OAM_START_ADDRESS 0xFE00

byte reverse_half_row_data(byte data){
	byte fb = data;
	fb = (fb&0xF0) >> 4 | (fb&0x0F) << 4;
   	fb = (fb&0xCC) >> 2 | (fb&0x33) << 2;
   	fb = (fb&0xAA) >> 1 | (fb&0x55) << 1;
	return fb;
}

void order_array(gbe::oam_memory_t data[MAX_SPRITES_ON_SCANLINE], int count){
	gbe::oam_memory_t buffer[MAX_SPRITES_ON_SCANLINE];
	bool used_entries[MAX_SPRITES_ON_SCANLINE]{false};
	if(count > MAX_SPRITES_ON_SCANLINE)
		count = MAX_SPRITES_ON_SCANLINE;
	memcpy(buffer, data, count*sizeof(*data));
	for(int i = 0; i < count; ++i){
		int used_index;
		for(int i = 0; i < MAX_SPRITES_ON_SCANLINE; ++i)
			if(!used_entries[i]){
				used_index = i;
				break;
			}
		int cmp_val = buffer[used_index].x;
		for(int j = count-1; j; --j){
			if(buffer[j].x >= cmp_val && !used_entries[j]){
				bool duplicate = false;
				for(int k = 0; k < i; ++k)
					if(data[k].x == buffer[j].x){
						duplicate = true; 
						used_entries[j] = true;
						break;
					}
				if(!duplicate)
					cmp_val = buffer[used_index = j].x;
			}
		}
		data[i] = buffer[used_index];
		used_entries[used_index] = true;
	}
}

byte determine_colour_index(gbe::fifo_pixel pixel){
	return (pixel.palette>>(pixel.colour*2))&0b11;
}

void get_fifo_pixel_data(byte rtrn_data[PIXELS_IN_FIFO], const gbe::fifo& fifo){
	for(int i = 0; i < sizeof(fifo.pixels)/sizeof(*fifo.pixels); ++i)
		rtrn_data[i] = determine_colour_index(fifo.pixels[i]);
}

void gbe::ppu_t::set_fifo_pixels_with_tile_data(fifo& f,  byte palette, byte priority, int shift){
	byte tile_data_high = pixel_fetcher.cur_tile.tile_data_high;
	byte tile_data_low	=  pixel_fetcher.cur_tile.tile_data_low;
	if(shift>0){
		tile_data_high 	<<= shift;
		tile_data_low	<<= shift;
	} else if(shift<0){
		shift*=-1;
		tile_data_high 	>>= shift;
		tile_data_low	>>= shift;
	}

	for(int i = 0; i < 8; ++i){
		f.pixels[7-i].colour = (((tile_data_high>>i)&0b1)<<1) | ((tile_data_low>>i)&0b1);
		f.pixels[7-i].palette = palette;
		f.pixels[7-i].backround_priority = (bool)priority;
	}
}

gbe::tile gbe::ppu_t::get_bg_fifo_tile(byte x, byte y, byte palette, bool mode_9C00, bool mode_8000){
	word index_y = ((y/8)&0x1F)*32;
	word index   = (((x/8)&0x1F)+index_y)%(32*32);
	word index2  = ((((x+8)/8)&0x1F)+index_y)%(32*32);
	if(mode_9C00){
		index	+= 0x9C00;
		index2	+= 0x9C00;
	} else{
		index	+= 0x9800;
		index2	+= 0x9800;
	}
	byte tile = memory.mem[index];
	byte tile2 = memory.mem[index2];
	word address;
	word address2;
	if(mode_8000){
		address 	= 0x8000+(tile*16)+(y%8)*2;
		address2 	= 0x8000+(tile2*16)+(y%8)*2;
	} else{
		address 	= 0x9000+(((signed char)tile)*16)+(y%8)*2;
		address2 	= 0x9000+(((signed char)tile2)*16)+(y%8)*2;
	}
	gbe::tile rtrn;
	rtrn.tile_data_low	= (memory.mem[address]<<(x%8)) 	 | (memory.mem[address2]>>(8-(x%8)));
	rtrn.tile_data_high = (memory.mem[address+1]<<(x%8)) | (memory.mem[address2+1]>>(8-(x%8)));
	return rtrn;
}

gbe::tile gbe::ppu_t::get_sprite_fifo_tile(byte tile_index, byte y, bool x_flip, bool is_16_height){
	if(is_16_height)
		tile_index&=~0b1;
	word address = 0x8000+(tile_index)*16+(y%16)*2;
	gbe::tile rtrn;
	byte low  = x_flip ? reverse_half_row_data(memory.mem[address]) 	: memory.mem[address];
	byte high = x_flip ? reverse_half_row_data(memory.mem[address+1])	: memory.mem[address+1];
	rtrn.tile_data_low	= low;
	rtrn.tile_data_high = high;
	return rtrn;
}

void gbe::ppu_t::set_bg_fifo_tile(fifo& f, byte x, byte y, byte palette, bool mode_9C00, bool mode_8000){
	pixel_fetcher.cur_tile = get_bg_fifo_tile(x, y, palette, mode_9C00, mode_8000);
	set_fifo_pixels_with_tile_data(f, palette, 1, 0);
}

void gbe::ppu_t::get_background_tile(){
	auto& pf = pixel_fetcher;
	bool mode_9C00;
	bool mode_8000;
	byte x;
	byte y;
	if(lcd_control&BIT(5) && pf.x*8 >= wx-7 && ly >= wy){
		mode_9C00 = lcd_control&BIT(6);
		x = pf.x*8-(wx-7);
		y = wl;
		window_has_drawn_pixel = true;
	} else{
		mode_9C00 = lcd_control&BIT(3);
		x = scx+pf.x*8;
		y = scy+ly;
	}
	mode_8000 = lcd_control&BIT(4);
	set_bg_fifo_tile(background_fifo, x, y, bgp, mode_9C00, mode_8000);
}

void gbe::ppu_t::push_background_fifo(){
	byte row[PIXELS_IN_FIFO];
	get_fifo_pixel_data(row, background_fifo);
	display.draw_row_8(pixel_fetcher.x*8, ly, row);
}

void gbe::ppu_t::update_sprite_fifo(){
	//	see if there's a sprite and if so draw all of its non-transparent colours
	for(int i = 0; i < sprite_buffer_size; ++i){
		if((sprite_buffer[i].x - pixel_fetcher.x*8)/8 == 0){
			oam_memory_t& cur_sprite = sprite_buffer[i];
			int shift = pixel_fetcher.x*8 - cur_sprite.x;
			byte sprite_size = lcd_control&BIT(2) ? 16 : 8;
			int y = cur_sprite.flags&BIT(6) ? (sprite_size-1)-((ly-cur_sprite.y)%sprite_size) : ((ly-cur_sprite.y)%sprite_size);
			pixel_fetcher.cur_tile = get_sprite_fifo_tile(cur_sprite.tile, y, cur_sprite.flags&BIT(5), lcd_control&BIT(2));
			byte palette = cur_sprite.flags&BIT(4) ? obp1 : obp0;
			set_fifo_pixels_with_tile_data(sprite_fifo, palette, cur_sprite.flags&BIT(7), shift);
			for(int j = 0; j < 8; ++j){
				if(sprite_fifo.pixels[j].colour != 0b00){
					if(sprite_fifo.pixels[j].backround_priority == 0){
						background_fifo.pixels[j] = sprite_fifo.pixels[j];
					} else{
						if(background_fifo.pixels[j].colour == 0b00)
							background_fifo.pixels[j] = sprite_fifo.pixels[j];
					}
				}
			}
			byte row[PIXELS_IN_FIFO];
			get_fifo_pixel_data(row, background_fifo);
			display.draw_row_8(pixel_fetcher.x*8, ly, row);
		}
	}
}

void gbe::ppu_t::update(){
	if(lcd_control&BIT(7) == 0)
		return;
	if(dots < 80){
		on_oam();
	} else if(dots == 80){
		order_array(sprite_buffer, sprite_buffer_size);
		change_ppu_mode(MODE_3);
		on_oam_end();
	} else if(dots > 80 && pixel_fetcher.x < 20){
		on_draw();
	} else if(dots == 80+168){
		change_ppu_mode(MODE_0);
		on_draw_end();
	} 
	if(dots+1 >= DOTS_PER_SCANLINE){
		on_new_scanline();
	} else
		++dots;
}

void gbe::ppu_t::on_oam(){
	if(dots%2 == 1)
		return;
	byte sprite_height = lcd_control&BIT(2) ? 16 : 8;
	if(sprite_buffer_size < MAX_SPRITES_ON_SCANLINE){
		oam_memory_t cur = *reinterpret_cast<oam_memory_t*>(&memory.mem[OAM_START_ADDRESS+(dots/2)*4]);
		cur.y-=16;
		cur.x-=8;;
		if(cur.y <= ly && cur.y+sprite_height > ly && cur.x >= 0 && cur.x < 160 && cur.y >= 0 && cur.y < 144){
			sprite_buffer[sprite_buffer_size++] = cur;
		}
	}
}

void gbe::ppu_t::on_oam_end(){
	if(ly == lcy){
		lcd_status|=BIT(6);
		lcd_status|=BIT(2);
	} else{
		lcd_status&=~BIT(6);
		lcd_status&=~BIT(2);
	}
	stat_interrupt();
}

void gbe::ppu_t::on_draw(){
	if(lcd_control&BIT(0)){
		update_bg_and_wn();
	}
}

void gbe::ppu_t::on_draw_end(){
	//lcd_status|=BIT(3);
	//stat_interrupt();
}

void gbe::ppu_t::on_new_scanline(){		
	if(window_has_drawn_pixel){
		++wl;
		window_has_drawn_pixel = false;
	}
	dots = 0;
	pixel_fetcher.x = 0;
	lcd_interrupt_fired = false;
	sprite_buffer_size = 0;
	++ly;
	if(ly == 144)
		v_blank();
	if(ly == MAX_SCANLINES)
		reset();
}

void gbe::ppu_t::stat_interrupt(){
	int mode = lcd_status&0b11;
	bool should_interrupt = !lcd_interrupt_fired &&(
		lcd_status&BIT(3)	||
		lcd_status&BIT(4)	||
		lcd_status&BIT(5)	||
		lcd_status&BIT(6));
	if(should_interrupt){
		fire_lcd_interrupt();
	}
}

void gbe::ppu_t::v_blank(){
	//std::this_thread::sleep_for(std::chrono::milliseconds(10));
	change_ppu_mode(MODE_1);
	SDL_PumpEvents();	//	input
	display.render_buffer();
	memory.request_interrupt((byte)interrupt_bits::V_BLANK);
}

void gbe::ppu_t::reset(){
	wl = 0;
	ly = 0;
	lcd_status = 0;
	change_ppu_mode(MODE_2);
	reset_fifos();
}

void gbe::ppu_t::reset_fifos(){
	background_fifo = fifo();
	sprite_fifo = fifo();
}

void gbe::ppu_t::update_bg_and_wn(){
	switch(pixel_fetcher.cur_step)
	{
	case 0:
		get_background_tile();
		break;
	case 1:
		break;
	case 2:
		break;
	case 3:
		if(lcd_control&BIT(0))
			push_background_fifo();
		if(lcd_control&BIT(1))
			update_sprite_fifo();
		break;
	case 4:
		++pixel_fetcher.x;
		break;
	}
	pixel_fetcher.inc_step();
}