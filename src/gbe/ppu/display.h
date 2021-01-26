#pragma once
#include<SDL2/SDL.h>
#include<standard_lib/typedefs.h>

namespace gbe{
	struct display_t{
		void create_window();
		void update_buffer(byte* buffer, int size);
		void render_buffer();
	protected:
		SDL_Window* window_handle{0};
		SDL_Renderer* renderer_handle{0};
	};
}


