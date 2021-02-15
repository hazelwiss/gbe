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

void gbe::display_t::update_buffer(byte* buffer, int size){
	SDL_Surface pixel_buffer;
	//pixel_buffer.pixels = buffer;
	//pixel_buffer.format = 
}

void gbe::display_t::fill_rect(int x, int y, int w, int h, unsigned int colour){
	SDL_Rect rect;
	int col;
	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;
	col = colour | (colour << 8) | (colour << 16) | (0xFF << 24);
	SDL_FillRect(this->surface_handler, &rect, col);
}

void gbe::display_t::render_buffer(){
	SDL_UpdateWindowSurface(this->window_handle);
	SDL_RenderPresent(this->renderer_handle);
}