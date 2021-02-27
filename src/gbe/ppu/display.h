#pragma once
#include<SDL2/SDL.h>
#include<standard_lib/typedefs.h>
#define SCR_W 160
#define SCR_H 144

namespace gbe{
	struct display_t{
		void create_window();
		void render_buffer();
		void draw_row_8(word data, byte palette, int x, int y, bool background_priority, bool sprite);
	protected:
		int sprite_buffer[SCR_W*SCR_H]{0};
		int background_buffer[SCR_W*SCR_H]{0};
		int determine_colour(byte bit1, byte bit2);
		int determine_colour(byte bits);
		unsigned int colour_array[4]{
			0xFFFF'FFFF,
			0xFF55'5555,
		 	0xFFAA'AAAA,
			0xFF00'0000
		};
		SDL_Window* 	window_handle	{nullptr};
		SDL_Renderer* 	renderer_handle	{nullptr};
		SDL_Surface* 	surface_handler	{nullptr};
	};
}


