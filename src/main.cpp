#include<gbe/cpu/cpu.h>

int main(){
	gbe::cpu cpu;
	while(true)
		cpu.emulate_fetch_decode_execute();
	return 0;
}
