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