#include"display.h"

void gbe::display_t::create_window(){
	this->window_handle 	= SDL_CreateWindow("gbe", 0, 0, 160, 144, 
		SDL_WINDOW_SHOWN 	|
		SDL_WINDOW_OPENGL);
	
	this->surface_handler 	= SDL_GetWindowSurface(this->window_handle);
	this->renderer_handle	= SDL_GetRenderer(this->window_handle);
	SDL_SetRenderDrawColor(this->renderer_handle, 255, 255, 255, 255);
	SDL_RenderClear(this->renderer_handle);
	SDL_RenderPresent(this->renderer_handle);

	SDL_Rect rect;
	int col = 0xFFFFFFFF;
	rect.x=0;rect.y=0;rect.w=160;rect.h=144;
	SDL_FillRect(this->surface_handler, &rect, col);
	SDL_UpdateWindowSurface(this->window_handle);
}

int gbe::display_t::determine_colour(byte bit1, byte bit2){
	return this->colour_array[(bit1<<1)|bit2];
}
int gbe::display_t::determine_colour(byte bits){
	return this->colour_array[bits];
}

void gbe::display_t::draw_row_8(word data, byte palette, int x, int y, bool background_priority, bool sprite){
	int* pntr;
	if(sprite)
		pntr = this->sprite_buffer+y*SCR_W+x;
	else  
		pntr = this->background_buffer+y*SCR_W+x;
	if(y >= 0 && y < SCR_H && x >= 0 && x < SCR_W)
		if(background_priority)
			for(int i = 0; i < 8 && x+i < SCR_W; ++i, ++pntr){
				byte bit1 = (data&BIT(15-i)) >> (15-i);
				byte bit2 = (data&BIT(7-i)) >> (7-i);
				byte bits = (bit1<<1)|bit2;
				if(sprite && !bits)
					(*pntr) = 0;
				else
					if(*(this->background_buffer+y*SCR_W+x+i) == determine_colour(palette&0b11))	
						*pntr = determine_colour((palette&(0b11<<(bits*2)))>>(bits*2));
			}
		else 
			for(int i = 0; i < 8 && x+i < SCR_W; ++i, ++pntr){
				byte bit1 = (data&BIT(15-i)) >> (15-i);
				byte bit2 = (data&BIT(7-i)) >> (7-i);
				byte bits = (bit1<<1)|bit2;
				if(sprite && !bits)
					*pntr = 0;
				else 
					*pntr = determine_colour((palette&(0b11<<(bits*2)))>>(bits*2));
			}
}

void gbe::display_t::render_buffer(){
	auto& surface = *this->surface_handler;
	for(int i = 0; i < SCR_W*SCR_H; ++i){	
		((int*)surface.pixels)[i] = sprite_buffer[i] ? sprite_buffer[i] : background_buffer[i];
	}
	SDL_UpdateWindowSurface(this->window_handle);
	//SDL_RenderPresent(this->renderer_handle);
}