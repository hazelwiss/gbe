#include<gbe/cpu/cpu.h>
#include<iostream>

//	implement LCD thing for boot rom to work as intended!
int main(){	
	SDL_Init(SDL_INIT_EVERYTHING);
	gbe::cpu_t cpu;
	cpu.create_window();
	try{
		cpu.load_ROM("roms/testing/mooneye/acceptance/if_ie_registers.gb");
	}
	catch(gbe::gbe_error e){
		printf("Error reading from file! %d\n", e);
	}
	while(true)
		try{
			cpu.emulate_fetch_decode_execute_cycle();
		}
		catch(gbe::gbe_error e){
			printf("Error during the fetch, decode and execute cycle! %d\n", e);
			cpu.output_print_to_file();
			break;
		}		
	return 0;
}
