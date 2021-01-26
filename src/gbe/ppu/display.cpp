#include"display.h"

void gbe::display_t::create_window(){
	SDL_CreateWindowAndRenderer(160*5, 144*5, SDL_WINDOW_SHOWN, &this->window_handle, &this->renderer_handle);
	SDL_SetRenderDrawColor(this->renderer_handle, 255, 255, 255, 255);
	SDL_RenderClear(this->renderer_handle);
	SDL_RenderPresent(this->renderer_handle);
}

void gbe::display_t::update_buffer(byte* buffer, int size){
	for(int i = 0; i < size*4; ++i){
		byte thing = (buffer[i/4] >> ((i%4)*2)) & 0b11;
		if(thing == 0)
			SDL_SetRenderDrawColor(this->renderer_handle, 0, 0, 0, 0);
		else if(thing == 1)
			SDL_SetRenderDrawColor(this->renderer_handle, 255/3, 255/3, 255/3, 255/3);
		else if(thing == 2)
			SDL_SetRenderDrawColor(this->renderer_handle, 255*2/3, 255*2/3, 255*2/3, 255*2/3);
		else if(thing == 3)
			SDL_SetRenderDrawColor(this->renderer_handle, 255, 255, 255, 255);
		for(int j = 0; j < 5; ++j)
			for(int k = 0; k < 5; ++k)
				SDL_RenderDrawPoint(this->renderer_handle, (i%160)*5+k, ((i/160)*5)+j);
	}
}

void gbe::display_t::render_buffer(){
	SDL_RenderPresent(this->renderer_handle);
}