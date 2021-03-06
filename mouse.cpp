#include "mouse.h"

MouseDriver::MouseDriver(InterruptManager* manager)
:	InterruptHandler(0x2C, manager),
	dataport(0x60),
	commandport(0x64)
{
	uint16_t* VideoMemory = (uint16_t*)0xb8000;
	offset = 0; //maybe 0 maybe not
	buttons = 0;
	x = 40;
	y = 12;
	
	VideoMemory[80*y+x] = ((VideoMemory[80*y+x] & 0xF000) >> 4)
							| ((VideoMemory[80*y+x] & 0x0F00) << 4)
							| ((VideoMemory[80*y+x] & 0x00FF));

	//this is remove holding keys
	commandport.Write(0xAB); // active mouse interrupt
	commandport.Write(0x20); // give us currunt state, command 0x60 = read controller command byte

	uint8_t status = dataport.Read() | 2;	// second bit to true
	commandport.Write(0x60); // tells PIC or Keyboard to change currunt status
	dataport.Write(status); // after changing write the status, command 0x60 = set controller command byte

	commandport.Write(0xD4);
	dataport.Write(0xF4); // finally activate the mouse
	dataport.Read();
}
MouseDriver::~MouseDriver(){

}


uint32_t MouseDriver::HandleInterrupt(uint32_t esp){
	
	uint8_t status = commandport.Read();
	if (!(status & 0x20))
		return esp;


	buffer[offset] = dataport.Read();
	offset = (offset + 1) % 3;

	if (offset == 0)
	{
		if (buffer[1] != 0 || buffer[2] != 0)
		{
			static uint16_t* VideoMemory = (uint16_t*)0xb8000;

			VideoMemory[80*y+x] = ((VideoMemory[80*y+x] & 0xF000) >> 4)
								| ((VideoMemory[80*y+x] & 0x0F00) << 4)
								| ((VideoMemory[80*y+x] & 0x00FF));

			x += buffer[1];
			if(x < 0) x = 0;
			if(x >= 80) x = 79;

			y -= buffer[2];
			if(y < 0) y = 0;
			if(y >= 25)y = 24;

			VideoMemory[80*y+x] = ((VideoMemory[80*y+x] & 0xF000) >> 4)
								| ((VideoMemory[80*y+x] & 0x0F00) << 4)
								| ((VideoMemory[80*y+x] & 0x00FF));
		}
	}
	return esp;
}