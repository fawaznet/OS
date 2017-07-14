
#include "types.h"
#include "gdt.h"
#include "interrupts.h"
#include "keyboard.h"


void printf(char* str){
	static uint16_t* VideoMemory = (uint16_t*) 0xb8000;

	static uint8_t x = 0, y = 0; //the curser

	for (int i = 0; str[i] != '\0'; ++i){

		switch(str[i]){
			case '\n':
				y++;
				x=0;
				break;
			default:
				VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | str[i];
				x++;
				break;
		}
		if (x >= 80)
		{
			y++;
			x=0;
		}

		if (y >= 25)
		{
			for(y=0; y < 25; y++)
				for(x=0; x < 80; x++)
					VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | ' ';

			x=0;
			y=0;
		}
	}
}

typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void callConstructors(){
	for (constructor* i = &start_ctors; i != &end_ctors; i++)
	{
		(*i)();
	}
}


extern "C" void kernelMain(void* multiboot_structure, uint32_t magicNumber){
	printf("Hello World, I'm Fawaz,this my OS, what is your name?\n");
	//printf("this is my Operating System (FawazOS).");

	GlobalDescriptorTable gdt;
	InterruptManager interrupts(0x20, &gdt); //&gdt pointer to gdt

	KeyboardDriver keyboard(&interrupts); //&interrupt pointer to interrupt manager


	interrupts.Activate();

	while(1);
}