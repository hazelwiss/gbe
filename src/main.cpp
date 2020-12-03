#include<gbe/cpu/cpu.h>
#include<iostream>

int main(){
	
	gbe::mem_t mem;
	mem.load_ROM();

	return 0;
	
	gbe::cpu_t cpu;
	while(true)
		try{
			cpu.emulate_fetch_decode_execute_cycle();
		}
		catch(gbe::gbe_exception e){
			printf("Error!\n");
			break;
		}
		
	return 0;
}
