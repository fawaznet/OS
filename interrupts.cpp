#include "interrupts.h"


void printf(char* str);

InterruptManager::GateDescriptor InterruptManager::InterruptDescriptorTable[256];

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
InterruptManager::InterruptManager(GlobalDescriptorTable* gdt)
: 	picMasterCommand(0x20),	//Master PIC >> port 0x20
	picMasterData(0x21),	
	picSlaveCommand(0xA0),	// Slave PIC
	picSlaveData(0xA1)

{
	uint16_t CodeSegment = gdt->CodeSegmentSelector();
	const uint8_t IDT_INTERRUPT_GATE = 0xE;

	for (uint16_t i = 0; i < 256; i++)
	{
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

void InterruptManager::Activate(){
	asm ("sti"); // start interrupt
}
	

uint32_t InterruptManager::handleInterrupt(uint8_t interruptNumber, uint32_t esp){

	printf("  INTERRUPT");

	return esp;
}
