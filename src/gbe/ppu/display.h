#pragma once
#include<SDL2/SDL.h>
#include<standard_lib/typedefs.h>
#define SCR_W 160
#define SCR_H 144

namespace gbe{
	enum class lcd_colour: unsigned int{ 
		WHITE 		= 0xFFFF'FFFF,
		LIGHT_GREY 	= 0xFFAA'AAAA,
		DARK_GREY	= 0xFF55'5555,
		BLACK		= 0xFF00'0000 
	};

	struct display_t{
		void create_window();
		void render_buffer();
		void draw_row_8(int x, int y, byte row[8]);
	protected:
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


