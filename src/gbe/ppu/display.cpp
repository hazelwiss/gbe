#include"display.h"

void gbe::display_t::create_window(){
	this->window_handle 	= SDL_CreateWindow("gbe", 0, 0, 160, 144, SDL_WINDOW_SHOWN);
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

int determine_colour(byte bit1, byte bit2){
	switch ((bit1<<1)|bit2)
	{
	case 0b00:
		return 0xFFFF'FFFF;
	case 0b01:
		return 0xFF55'5555;
	case 0b10:
		return 0xFFAA'AAAA;
	case 0b11:
		return 0xFF00'0000;
	}
	return 0xFABCDE;
}

void gbe::display_t::draw_row_8(word data, int void_pixels, int x, int y){
	auto surface = this->surface_handler;
	int* pntr = (int*)surface->pixels+y*SCR_W+x;
	if(y >= 0 && y < SCR_H && x >= 0 && x < SCR_W)
		for(int i = void_pixels; i < 8 && x+i < SCR_W; ++i)
			*(pntr++) = determine_colour((data&BIT(15-i)) >> (15-i), (data&BIT(7-i)) >> (7-i));
}

void gbe::display_t::draw_blank_row(int y){
	auto surface = this->surface_handler;
	int* test_pntr = (int*)surface->pixels+y*SCR_W;
	if(y >= 0 && y < SCR_H)
		for(int x = 0; x < SCR_W; ++x)
			*(test_pntr++) = 0xFFFF'FFFF;
}

void gbe::display_t::render_buffer(){
	SDL_UpdateWindowSurface(this->window_handle);
	//SDL_RenderPresent(this->renderer_handle);
}