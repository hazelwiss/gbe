#include<gbe/cpu/cpu.h>
#include<iostream>

int main(){
	
	gbe::mem_t mem;
	try{
		mem.load_ROM("roms/Link's Awakening.gb");
	}
	catch(gbe::gbe_error e){
		printf("error!\n");
	}
	return 0;
	
	gbe::cpu_t cpu;
	while(true)
		try{
			cpu.emulate_fetch_decode_execute_cycle();
		}
		catch(gbe::gbe_error e){
			printf("Error!\n");
			break;
		}
		
	return 0;
}
