#include<gbe/cpu/cpu.h>

int main(){
	gbe::cpu_t cpu;
	while(true)
		cpu.emulate_fetch_decode_execute_cycle();
	return 0;
}
