#include "interrupts.h"


void printf(char* str);



InterruptHandler::InterruptHandler(uint8_t interruptNumber, InterruptManager* interruptManager){

	this->interruptNumber = interruptNumber;	
	this->interruptManager = interruptManager;
	interruptManager->handlers[interruptNumber] = this; // put it in the array
}

InterruptHandler::~InterruptHandler(){
	if(interruptManager->handlers[interruptNumber] == this)
		interruptManager->handlers[interruptNumber] = 0;
}

uint32_t InterruptHandler::HandleInterrupt(uint32_t esp){
	// behavier of handling interrupt to return esp
	return esp;
}

InterruptManager::GateDescriptor InterruptManager::InterruptDescriptorTable[256];

InterruptManager* InterruptManager::ActiveInterruptManager =0; // init the pointer for InterruptManager

void InterruptManager::SetInterruptDescriptorTableEntry(
	uint8_t interruptNumber,
	uint16_t codeSegmentSelectorOffset,
	void (*handler)(),
	uint8_t DescriptorPrivilegeLevel, /*access*/
	uint8_t DescriptorType /*flags*/
	)
{
	//this is set entry to the table
	const uint8_t IDT_DESC_PRESENT = 0X80;

	InterruptDescriptorTable[interruptNumber].handlerAddressLowBits = ((uint32_t)handler) & 0xFFFF;
	InterruptDescriptorTable[interruptNumber].handlerAddressHighBits = (((uint32_t)handler) >>16 ) & 0xFFFF;
	InterruptDescriptorTable[interruptNumber].gdt_codeSegmentSelector = codeSegmentSelectorOffset;
	InterruptDescriptorTable[interruptNumber].access = IDT_DESC_PRESENT | DescriptorType | ((DescriptorPrivilegeLevel&3) << 5);
	InterruptDescriptorTable[interruptNumber].reserved = 0;
}

// this constructor who will set these entries.
InterruptManager::InterruptManager(uint16_t hardwareInterruptOffset, GlobalDescriptorTable* gdt)
: 	picMasterCommand(0x20),	//Master PIC >> port 0x20
	picMasterData(0x21),	
	picSlaveCommand(0xA0),	// Slave PIC
	picSlaveData(0xA1)

{
	hardwareInterruptOffset = HardwareInterruptOffset();
	//this->hardwareInterruptOffset = HardwareInterruptOffset();

	uint16_t CodeSegment = gdt->CodeSegmentSelector();
	const uint8_t IDT_INTERRUPT_GATE = 0xE;

	for (uint16_t i = 0; i < 256; i++)
	{
		handlers[i] = 0;
		SetInterruptDescriptorTableEntry(i, CodeSegment, &IgnoreInterruptRequest, 0, IDT_INTERRUPT_GATE);	// 0 for privilledge
	}

	SetInterruptDescriptorTableEntry(0x20, CodeSegment, &HandleInterruptRequest0x00, 0, IDT_INTERRUPT_GATE);	// 0x20 + 0x00 = 0x20
	SetInterruptDescriptorTableEntry(0x21, CodeSegment, &HandleInterruptRequest0x01, 0, IDT_INTERRUPT_GATE);	// 0x21 + 0x01 = 0x21 will jump to HandleInterruptRequest0x01 in interrupts.s

	// Before CPU uses the table we will comunicate with this PICs

	picMasterCommand.Write(0x11);
	picSlaveCommand.Write(0x11);

	picMasterData.Write(0x20);// Master will ADD 20 to any Interrupt (0x20 + 0x01) >> 20 t 27
	picSlaveData.Write(0x28);// Slave will ADD 28 to any Interrupt (0x28 + 0x01) >> 28 to 30

	picMasterData.Write(0x04);	// This is tells the Master you are the master.
	picSlaveData.Write(0x02);	// This is tells the Slave you are the Slave.

	picMasterCommand.Write(0x01);
	picSlaveCommand.Write(0x01);

	picMasterCommand.Write(0x00);
	picSlaveCommand.Write(0x00);

	//this is will tell the CPU how he ganna use the table
	InterruptDescriptorTablePointer idt;
	idt.size = 256 * sizeof(GateDescriptor) - 1;
	idt.base = (uint32_t)InterruptDescriptorTable;	//pointer for the tabel
	asm volatile("lidt %0" : : "m" (idt));	//tell processor to load the idt
}

InterruptManager::~InterruptManager(){
}

uint16_t InterruptManager::HardwareInterruptOffset(){
	return hardwareInterruptOffset;
}

void InterruptManager::Activate(){
	// one table for one Active Interrupt Manager.
	if (ActiveInterruptManager != 0)
		ActiveInterruptManager->Deactivate();
	
	ActiveInterruptManager = this;	// this pointer

	asm ("sti"); // start interrupt
}

void InterruptManager::Deactivate(){
	// one table for one Active Interrupt Manager.
	if (ActiveInterruptManager == this){
		ActiveInterruptManager = 0;	// this pointer
		asm ("cli"); // clear interrupt
	}
}

uint32_t InterruptManager::HandleInterrupt(uint8_t interruptNumber, uint32_t esp){

	if (ActiveInterruptManager != 0)
	{
		return ActiveInterruptManager->DoHandleInterrupt(interruptNumber, esp);
	}

	/*
	//printf("  INTERRUPT");
	char* foo = "INTERRUPT 0x00";
	char* hex = "0123456789ABCDEF";

	foo[12] = hex[(interruptNumber >> 4) & 0xF];
	foo[13] = hex[interruptNumber & 0xF];
	printf(foo);
	*/
	return esp;
}

uint32_t InterruptManager::DoHandleInterrupt(uint8_t interruptNumber, uint32_t esp){

	if (handlers[interruptNumber] != 0)
	{
		// call HandleInterrupt
		esp = handlers[interruptNumber]->HandleInterrupt(esp); // stack pointer
	}
	else if (interruptNumber != 0x20)
	{
		char* foo = "UNHANDLED INTERRUPT 0x00";
		char* hex = "0123456789ABCDEF";

		foo[22] = hex[(interruptNumber >> 4) & 0xF];
		foo[23] = hex[interruptNumber & 0xF];
		printf(foo);
	}
	
	
	if (0x20 <= interruptNumber && interruptNumber < 0x30)
	{
		picMasterCommand.Write(0x20);
		if(0x28 <= interruptNumber)
			picSlaveCommand.Write(0x20);
	}


	return esp;
}
