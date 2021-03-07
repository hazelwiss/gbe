#include"ppu.h"
#include<cstring>
#include<thread>
#define MAX_DOTS 70224
#define DOTS_PER_SCANLINE 456
#define MAX_SCANLINES 154
#define OAM_SCAN_BASE_TIME 248
#define SCANNING_OAM_TIME 80
#define MAX_SPRITES_ON_SCANLINE 10
#define SPRITES_IN_OAM 40
#define MODE_0 0
#define MODE_1 1
#define MODE_2 2
#define MODE_3 3

word reverse_pixel_data(word data){
	byte fb = data;
	byte sb = data>>8;
	fb = (fb&0xF0) >> 4 | (fb&0x0F) << 4;
   	fb = (fb&0xCC) >> 2 | (fb&0x33) << 2;
   	fb = (fb&0xAA) >> 1 | (fb&0x55) << 1;
   	sb = (sb&0xF0) >> 4 | (sb&0x0F) << 4;
   	sb = (sb&0xCC) >> 2 | (sb&0x33) << 2;
   	sb = (sb&0xAA) >> 1 | (sb&0x55) << 1;
	return fb | (sb<<8);
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

void gbe::ppu_t::set_fifo_pixels_with_tile_data(fifo& f, byte palette){
	byte tile_data_high = f.cur_tile.tile_data_high;
	byte tile_data_low = f.cur_tile.tile_data_low;
	for(int i = 0; i < 8; ++i){
		f.pixels[7-i].colour = (((tile_data_high>>i)&0b1)<<1) | ((tile_data_low>>i)&0b1);
		f.pixels[7-i].palette = palette;
	}
}

void gbe::ppu_t::get_background_tile(){
	auto& bf = background_fifo;
	bool mode_9C00 =  scx+bf.x*8 >= wx-7 && scy+ly >= wy && lcd_control&BIT(5) ? lcd_control&BIT(6) : lcd_control&BIT(3);
	word index = (((scx/8)+background_fifo.x)%32)+
		(((scy+ly)/8)%32)*32;
	if(mode_9C00){
		index+=0x9C00;
	} else{
		index+=0x9800;
	}
	bool mode_8000 = lcd_control&BIT(4);
	byte tile = memory.mem[index];
	word address;
	if(mode_8000)
		address = 0x8000+(tile*16)+((scy+ly)%8)*2;
	else
		address = 0x9000+(((signed char)tile)*16)+((scy+ly)%8)*2;
	bf.cur_tile.tile_index = tile;
	bf.cur_tile.vram_address = address;
	
	bf.cur_tile.tile_data_low = (memory.mem[address+1] << (scx%8)) | (memory.mem[address+17] >> (8-(scx%8)));
	bf.cur_tile.tile_data_high = (memory.mem[address] << (scx%8)) | (memory.mem[address+16] >> (8-(scx%8)));
	set_fifo_pixels_with_tile_data(bf, bgp);
}

void gbe::ppu_t::get_window_tile(){
	auto& bf = background_fifo;
	if(background_fifo.x*8 < wx-7 || ly < wl)
		return;
	word x = background_fifo.x*8-(wx-7);
	word y = ly-wl;
	bool mode_9C00 = lcd_control&BIT(6);
	word index = ((x/8)%32)+
		((y/8)%32)*32;
	if(mode_9C00){
		index+=0x9C00;
	} else{
		index+=0x9800;
	}
	bool mode_8000 = lcd_control&BIT(4);
	byte tile = memory.mem[index];
	word address;
	if(mode_8000)
		address = 0x8000+(tile*16)+(y%8)*2;
	else
		address = 0x9000+(((signed char)tile)*16)+(y%8)*2;
	bf.cur_tile.tile_index = tile;
	bf.cur_tile.vram_address = address;
	bf.cur_tile.tile_data_low = memory.mem[address+1];
	bf.cur_tile.tile_data_high = memory.mem[address];
	set_fifo_pixels_with_tile_data(bf, bgp);
}

void gbe::ppu_t::push_background_fifo(){
	byte row[PIXELS_IN_FIFO]{0};
	get_fifo_pixel_data(row, background_fifo);
	display.draw_row_8(background_fifo.x*8, ly, row);
}

void gbe::ppu_t::push_window_fifo(){
	if(ly >= wl || background_fifo.x*8 < wx-7)
		return;
	window_has_drawn_pixel = true;
	byte row[PIXELS_IN_FIFO]{0};
	get_fifo_pixel_data(row, background_fifo);
	display.draw_row_8(background_fifo.x*8, ly, row);
}

void gbe::ppu_t::update(){
	if(lcd_control&BIT(7) == 0)
		return;
	if(dots == 80-1){
		change_ppu_mode(MODE_3);
		on_oam_end();
	} else if(dots >= 80 && background_fifo.x < 20){
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
		if(lcd_control&BIT(5)){
			if(bg_drawing)
				update_bg();
			else
				update_wn();
		}
		else 
			update_bg();
	}
}

void gbe::ppu_t::on_draw_end(){

}

void gbe::ppu_t::on_new_scanline(){		
	++ly;
	if(window_has_drawn_pixel){
		++wl;
		window_has_drawn_pixel = false;
	}
	dots = 0;
	background_fifo.x = 0;
	lcd_interrupt_fired = false;
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
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	change_ppu_mode(MODE_1);
	display.render_buffer();
	memory.request_interrupt((byte)interrupt_bits::V_BLANK);
}

void gbe::ppu_t::reset(){
	wl = 0;
	ly = 0;
	change_ppu_mode(MODE_2);
	reset_fifos();
}

void gbe::ppu_t::reset_fifos(){
	background_fifo = fifo();
	sprite_fifo = fifo();
}

void gbe::ppu_t::update_bg(){
	switch(background_fifo.cur_step)
	{
	case 0:
		get_background_tile();
		break;
	case 1:
		break;
	case 2:
		break;
	case 3:
		push_background_fifo();
		break;
	case 4:
		if(lcd_control&BIT(5))
			bg_drawing = !bg_drawing;
		else 
			++background_fifo.x;
		break;
	}
	background_fifo.inc_step();
}
void gbe::ppu_t::update_wn(){
	switch(background_fifo.cur_step)
	{
	case 0:
		get_window_tile();
		break;
	case 1:
		break;
	case 2:
		break;
	case 3:
		push_window_fifo();
		break;
	case 4:
		bg_drawing = !bg_drawing;
		++background_fifo.x;
		break;
	}
	background_fifo.inc_step();
}