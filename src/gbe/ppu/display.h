#pragma once
#include<SDL2/SDL.h>
#include<standard_lib/typedefs.h>

namespace gbe{
	struct display_t{
		void create_window();
		void update_buffer(byte* buffer, int size);
		void render_buffer();

		void fill_rect(int x, int y, int w, int h, unsigned int colour);
	protected:
		SDL_Window* 	window_handle	{nullptr};
		SDL_Renderer* 	renderer_handle	{nullptr};
		SDL_Surface* 	surface_handler	{nullptr};
	};
}


