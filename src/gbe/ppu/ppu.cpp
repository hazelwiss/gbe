#include"ppu.h"
#include<cstring>
#define MAX_DOTS 70224
#define DOTS_PER_SCANLINE 456
#define MAX_SCANLINES 154

struct pixel{
	byte a:2;
	byte b:2;
	byte c:2;
	byte d:2;
};

void gbe::ppu_t::update(byte cycles){
	if(!(lcd_control & BIT(7)))
		return;
	
	//	test render oam sprites!
	pixel data[X_RES*Y_RES/4];
	memset(data, 0, sizeof(data));
	byte test[0xFE9F-0xFE00];
	memcpy(test, &memory.mem[0xFE00], sizeof(test));

	for(int i = 0xFE00; i < 0xFE9F; i+=4){
		int x = memory.mem[i+1];
		int y = memory.mem[i];
		int tile_number = memory.mem[2];
		if(x-2 < 0 || (y*160/4)-4 < 0)
			continue;
		pixel *cur_data = &data[x-2+(y*160/4)-4];
		byte row1 = memory.mem[0x8000+tile_number*16];
		byte row2 = memory.mem[0x8000+tile_number*16+1];
		(byte&)*cur_data = 0;
		for(int i = 0; i < 8; ++i, cur_data+=40){
			cur_data->a = ((row1 << 1) | row2) & 0b11;
			cur_data->b = ((row1 << 1) | row2) & 0b1100;
			cur_data->c = ((row1 << 1) | row2) & 0b1100-00;
			cur_data->d = ((row1 << 1) | row2) & 0b1100-0000;
		}
	}

	++this->ly;

	display.update_buffer((byte*)data, sizeof(data));
	display.render_buffer();
	return;
	/*
	int window_display_enabled = lcd_control & BIT(5);
	int window_and_background_display_mode = lcd_control & BIT(4);
	int sprite_size = lcd_control & BIT(2);
	int display_sprites = lcd_control & BIT(1);
	int monochrome = lcd_control & BIT(0);
	word window_tileset_start_position = 0x9C00;
	word background_tileset_start_position = 0x9C00;
	if(lcd_control & BIT(6)){	//	window tile map display select
		window_tileset_start_position = 0x9800;
	}
	if(lcd_control & BIT(3)){
		background_tileset_start_position = 0x9800;
	}
	if(ly >= 144)
		memory.request_interrupt((word)interrupt_addresses::V_BLANK);
	if(lcd_status & BIT(6))
		if(ly == lcy)
			memory.request_interrupt((word)interrupt_addresses::LCD_STAT);
	//	add the other LCD_STAT interrupts things and test them when lcd mode is implemented.
	
	
	display.update_buffer();
	if(window_display_enabled)
		display.render_buffer();
	if(!(cycles % DOTS_PER_SCANLINE) || (cycles % DOTS_PER_SCANLINE) < (dots % DOTS_PER_SCANLINE)){
		++scanline;
		if(scanline > 143){
			change_mode(1);
		}
		else{

		}
	}
	dots = (dots + cycles) % MAX_DOTS;
	*/
}

void gbe::ppu_t::mode_3(byte& cycles){

}
void gbe::ppu_t::mode_2(byte& cycles){

}
void gbe::ppu_t::mode_1(byte& cycles){

}
void gbe::ppu_t::mode_0(byte& cycles){

}