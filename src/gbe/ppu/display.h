#pragma once
#include<SDL2/SDL.h>
#include<standard_lib/typedefs.h>
#define SCR_W 160
#define SCR_H 144

namespace gbe{
	struct display_t{
		void create_window();
		void render_buffer();
		void draw_blank_row(int y);
		void draw_row_8(word data, int void_pixels, int x, int y);
	protected:
		int background_buffer[SCR_W*SCR_H];
		int window_buffer[SCR_W*SCR_H];
		int sprite_buffer[SCR_W*SCR_H];
		SDL_Window* 	window_handle	{nullptr};
		SDL_Renderer* 	renderer_handle	{nullptr};
		SDL_Surface* 	surface_handler	{nullptr};
	};
}


