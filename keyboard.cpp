#include "keyboard.h"

KeyboardDriver::KeyboardDriver(InterruptManager* manager)
:	InterruptHandler(0x21, manager),
	dataport(0x60),
	commandport(0x64)
{
	//this is remove holding keys
	while(commandport.Read() & 0x1)
		dataport.Read();
	commandport.Write(0xAE); // active interrupt
	commandport.Write(0x20); // give us currunt state

	uint8_t status = (dataport.Read() | 1) & ~0x10;	// read that, then we set right bit new state and clear fifth bit (~0x10)
	commandport.Write(0x60); // tells PIC or Keyboard to change currunt status
	dataport.Write(status); // after changing write the status

	dataport.Write(0xF4); // finally activate the keyboard
}
KeyboardDriver::~KeyboardDriver(){

}

void printf(char* str);
uint32_t KeyboardDriver::HandleInterrupt(uint32_t esp){

	uint8_t key = dataport.Read();
	if(key < 0x80){
		switch(key){
			case 0xFA: break;
			case 0x2A: break; // SHIFT
			case 0x0E: break; // BACKSPACE
			case 0x39: printf(" "); break; // space
			case 0x1C: printf("\n"); break;
			case 0x21: printf("f"); break;
			case 0x1E: printf("a"); break;
			case 0x11: printf("w"); break;
			case 0x2C: printf("z"); break;
			case 0x45: case 0xC5: break;
			default:
				char* foo = "KEYBOARD 0x00";
				char* hex = "0123456789ABCDEF";

				foo[11] = hex[(key >> 4) & 0xF];
				foo[12] = hex[key & 0xF];
				printf(foo);
				break;
		}
	}
	return esp;
}