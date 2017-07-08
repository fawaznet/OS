#include "keyboard.h"

KeyboardDriver::KeyboardDriver(InterruptManager* manager)
:	InterruptHandler(0x21, manager),
	dataport(0x60),
	commandport(0x60)
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

void printf(char*);
uint32_t KeyboardDriver::HandleInterrupt(uint32_t esp){

	uint8_t key = dataport.Read();

	char* foo = "KEYBOARD 0x00";
	char* hex = "0123456789ABCDEF";

	foo[11] = hex[(key >> 4) & 0xF];
	foo[12] = hex[key & 0xF];
	printf(foo);

	return esp;
}