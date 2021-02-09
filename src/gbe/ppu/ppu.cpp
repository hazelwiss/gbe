#include"ppu.h"
#include<cstring>
#define MAX_DOTS 70224
#define DOTS_PER_SCANLINE 456
#define MAX_SCANLINES 154

struct pixel{
	byte a:2;
	byte b:2;
	byte c:2;
	byte d:2;
};

void gbe::ppu_t::update(byte cycles){
	if(!(lcd_control & BIT(7)))
		return;

	++this->ly;
	//if(this->ly == 140)
		//memory.request_interrupt((byte)interrupt_bits::V_BLANK);
	return;

	//byte pixel[160*144];

	byte test[] = {
		0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B, 0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D,
 		0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E, 0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99,
		0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC, 0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E
	};

	byte tex_data[0x9FFF-0x8000];
	byte* tt = tex_data;
	memcpy(tex_data, &memory.mem[0x8000], sizeof(tex_data));

	constexpr int adr_start = 0;//0x8000; 
	constexpr int adr_end   = 32;//0x9FFF;
	constexpr int adr_range = adr_end-adr_start;

	byte test_tex[]{
		0x00, 0x01, 0x10, 0x11, 0x00, 0x01, 0x10, 0x11,
		0x00, 0x01, 0x10, 0x11, 0x00, 0x01, 0x10, 0x11,
		0x00, 0x01, 0x10, 0x11, 0x00, 0x01, 0x10, 0x11,
		0x00, 0x01, 0x10, 0x11, 0x00, 0x01, 0x10, 0x11,
	};

	for(int i = adr_start; i < adr_end; i+=2){
		byte rowb_1 = test_tex[i];//memory.mem[i];
		byte rowb_2 = test_tex[i+1];//memory.mem[i+1];
		int x = ((i-adr_start)/16)*8;
		int y = ((i-adr_start)/2%8) + (x/160)*8;
		x%=160;
		for(int b = 7; b >= 0; --b){
			switch (((rowb_1 << 1) & (1 << (b+1))) | (rowb_2 & (1 << b)))
			{
			case 0b00:
				display.fill_rect(x+7-b, y, 1, 1, 0xFF);
				break;
			case 0b01:
				display.fill_rect(x+7-b, y, 1, 1, 0x80);
				break;
			case 0b10:
				display.fill_rect(x+7-b, y, 1, 1, 0x40);
				break;
			case 0b11:
				display.fill_rect(x+7-b, y, 1, 1, 0x00);
				break;
			}
		}
		if(y == 145)
			display.render_buffer();
	}

	//display.update_buffer((byte*)data, sizeof(data));
	display.render_buffer();
	return;
}

void gbe::ppu_t::mode_3(byte& cycles){

}
void gbe::ppu_t::mode_2(byte& cycles){

}
void gbe::ppu_t::mode_1(byte& cycles){

}
void gbe::ppu_t::mode_0(byte& cycles){

}