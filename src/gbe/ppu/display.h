#pragma once
#include<SDL2/SDL.h>
#include<standard_lib/typedefs.h>
#define SCR_W 160
#define SCR_H 144

namespace gbe{
	struct display_t{
		void create_window();
		void render_buffer();
		void draw_row_8(int x, int y, byte row[8]);
		void clear_buffer(){
			memset(this->surface_handler->pixels, colour_array[3], SCR_H*SCR_W*sizeof(int));
		}
	protected:
		int determine_colour(byte bit1, byte bit2);
		int determine_colour(byte bits);
		unsigned int colour_array[4]{
			0xFFFF'FFFF,
		 	0xFFAA'AAAA,
			0xFF55'5555,
			0xFF00'0000
		};
		SDL_Window* 	window_handle	{nullptr};
		SDL_Renderer* 	renderer_handle	{nullptr};
		SDL_Surface* 	surface_handler	{nullptr};
	};
}


