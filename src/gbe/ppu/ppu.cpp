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

void gbe::ppu_t::update(byte cycles){
	//	nintendo logo is scrolling background! tilemap in 9900 something!
	//	background fixing	
	if(!(lcd_control&BIT(7)))
		return;
	if(dots < 80 && (dots+cycles) > SCANNING_OAM_TIME){
		//	scanning oam.
		change_ppu_mode(MODE_3);
	}
	if((dots+cycles)%(OAM_SCAN_BASE_TIME+scanline_oam_time_delay) < dots%OAM_SCAN_BASE_TIME){
		//	h-blanking
		if(ly == lcy){
			lcd_status |= BIT(2);
			lcd_status |= BIT(6);
		} else{
			lcd_status &= ~BIT(2);
			lcd_status &= ~BIT(6);
		}
		stat_interrupt();
		change_ppu_mode(MODE_0);
	} 
	if((dots+cycles)%DOTS_PER_SCANLINE < dots){
		//	new scanline
		if(lcd_control&BIT(0) && ly < SCR_H){
			update_background_in_row();
			if(lcd_control&BIT(5))
				update_window_in_row();
		}
		if(lcd_control&BIT(1))
			update_sprite_in_row();
		increment_ly();
	}
	dots = (dots+cycles)%DOTS_PER_SCANLINE;
}
void gbe::ppu_t::reset(){
	wl = 0;
	scanline_oam_time_delay = 0;
	change_ppu_mode(MODE_2);
}
void gbe::ppu_t::increment_ly(){
	if(ly == 144){
		//	v-blanking
		//	sleep properly!
		v_blank_interrupt();
		//std::this_thread::sleep_for(std::chrono::milliseconds(15));
		display.render_buffer();
		change_ppu_mode(MODE_1);
	}	
	else if(ly == 153)
		reset();
	ly = (ly+1)%MAX_SCANLINES;
}
void gbe::ppu_t::v_blank_interrupt(){
	memory.request_interrupt((byte)interrupt_bits::V_BLANK);
}
void gbe::ppu_t::stat_interrupt(){
	if(lcd_status&BIT(6))
		memory.request_interrupt((byte)interrupt_bits::LCD_STAT);
}
byte gbe::ppu_t::get_sprites_in_row(oam_memory_t data_reference[MAX_SPRITES_ON_SCANLINE], byte y){
	int sprites[MAX_SPRITES_ON_SCANLINE];
	int array_size{0};
	int sprite_height = lcd_status&BIT(2) ? 16 : 8;
	for(int i = 0; i < SPRITES_IN_OAM; ++i){
		int indx = 0xFE00+i*4;
		int sprite_y = memory.mem[indx]-16;
		if(y < sprite_y+sprite_height && y >= sprite_y && memory.mem[indx+1] > 0){
			sprites[array_size++] = 
				(sprite_y<<24) 					| 
				((memory.mem[indx+1]-8)<<16) 	| 
				(memory.mem[indx+2]<<8) 		| 
				memory.mem[indx+3];
		}
		if(array_size >= MAX_SPRITES_ON_SCANLINE)
			break;
	}
	int loop = array_size;
	while(loop--){
		auto& sprite = sprites[loop];
		data_reference[loop].y 		= (sprite >> 24);
		data_reference[loop].x 		= (sprite >> 16);
		data_reference[loop].tile	= (sprite >> 8);
		data_reference[loop].flags 	= sprite;
	}
	order_array(data_reference, array_size);
	return array_size;
}
void gbe::ppu_t::update_background_in_row(){
	word tile_pixel_data; 
	byte tile_index;
	for(int x = 0; x < SCR_W/8; ++x){
		tile_index = get_tile_index((scx/8)+x, (scy+ly)/8, 
			x >= wx-7 && ly >= wy && lcd_control&BIT(5) ? lcd_control&BIT(6) : lcd_control&BIT(3));
		tile_pixel_data = get_tile_data(tile_index, scy+ly, lcd_control&BIT(4));
		display.draw_row_8(tile_pixel_data, bgp, x*8, ly, false, false);
	}
}
void gbe::ppu_t::update_window_in_row(){
	if(ly-wy-wl < 0)
		return;
	word tile_pixel_data;
	byte tile_index;
	for(int x = (wx-7); x < SCR_W; x+=8){
		tile_index = get_tile_index((x-wx+7)/8, wl/8, lcd_control&BIT(6));
		tile_pixel_data = get_tile_data(tile_index, wl, lcd_control&BIT(4));
		display.draw_row_8(tile_pixel_data, bgp, x, ly, false, false);
	}
	if(wx-7 < SCR_W)
		++wl;
}
void gbe::ppu_t::update_sprite_in_row(){
	oam_memory_t sprites[MAX_SPRITES_ON_SCANLINE];
	byte amount_of_sprites = get_sprites_in_row(sprites, ly);
	byte tile_pixel_data;
	int vert_pos;
	int flip_offset = lcd_status&BIT(2) ? 15 : 7;
	for(int i = 0; i < amount_of_sprites; ++i){
		const auto& sprite = sprites[i];
		if(sprite.flags&BIT(6))	//	vertical mirror
			vert_pos = sprite.y+7-ly;
		else
			vert_pos = ly-sprite.y;
		int tile_number = sprite.tile;
		if(lcd_status&BIT(2)){
			if(vert_pos%16 > 7)
				tile_number&=~0b1;
			else
				tile_number|=0b1;
		}
		tile_pixel_data = get_tile_data(sprite.tile, vert_pos, true);
		if(sprite.flags&BIT(5))
			tile_pixel_data = reverse_pixel_data(tile_pixel_data);
		display.draw_row_8(tile_pixel_data,
			(sprite.flags&BIT(4) ? obp0 : obp1), 
			sprite.x, ly, sprite.flags&BIT(7), true);
	}
}
