#include"ppu.h"
#include<cstring>
#include<thread>
#define MAX_DOTS 70224
#define DOTS_PER_SCANLINE 456
#define MAX_SCANLINES 154
void gbe::ppu_t::update(byte cycles){
	//	nintendo logo is scrolling background! tilemap in 9900 something!
	//	background fixing	
	if(!(lcd_control&BIT(7)))
		return;
	if((dots+cycles)%DOTS_PER_SCANLINE < dots){
		if(lcd_control&BIT(0)){
			update_background_in_row();
			if(lcd_control&BIT(5))
				update_window_in_row();
		}
		increment_ly();
	}
	dots = (dots+cycles)%DOTS_PER_SCANLINE;
}
void gbe::ppu_t::reset(){
	wl = 0;
}
void gbe::ppu_t::increment_ly(){
	if(ly == lcy){
		lcd_status |= BIT(2);
		stat_interrupt_check();
	} else
		lcd_status &= ~BIT(2);
	if(ly == 144){
		//	sleep properly!
		v_blank_interrupt_check();
		std::this_thread::sleep_for(std::chrono::milliseconds(15));
		display.render_buffer();
	}	
	else if(ly == 153)
		reset();
	ly = (ly+1)%MAX_SCANLINES;
}
void gbe::ppu_t::v_blank_interrupt_check(){
	memory.request_interrupt((byte)interrupt_bits::V_BLANK);
}
void gbe::ppu_t::stat_interrupt_check(){
	if(lcd_status&BIT(6))
		memory.request_interrupt((byte)interrupt_bits::LCD_STAT);
	
}
void gbe::ppu_t::update_background_in_row(){
	word tile_pixel_data; 
	byte tile_index;
	for(int x = 0; x < SCR_W/8; ++x){
		tile_index = get_tile_index((scx/8)+x, (scy+ly)/8, 
			x >= wx-7 && ly >= wy && lcd_control&BIT(5) ? lcd_control&BIT(6) : lcd_control&BIT(3));
		tile_pixel_data = get_tile_data(tile_index, scy+ly, lcd_control&BIT(4));
		display.draw_row_8(tile_pixel_data, 0, x*8, ly);
	}
	display.render_buffer();	//	tmp!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
}
void gbe::ppu_t::update_window_in_row(){
	if(ly-wy-wl < 0)
		return;
	word tile_pixel_data;
	byte tile_index;
	for(int x = (wx-7); x < SCR_W; x+=8){
		tile_index = get_tile_index((x-wx+7)/8, wl/8, lcd_control&BIT(6));
		tile_pixel_data = get_tile_data(tile_index, wl, lcd_control&BIT(4));
		display.draw_row_8(tile_pixel_data, 0, x, ly);
	}
	if(wx-7 < SCR_W)
		++wl;
}
void gbe::ppu_t::update_sprite_in_row(){
	
}
