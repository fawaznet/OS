#ifndef __INTERRUPTS_H
#define __INTERRUPTS_H

#include "types.h"
#include "gdt.h"
#include "port.h"

	
	class InterruptManager;


	class InterruptHandler
	{
	protected:
		uint8_t interruptNumber;
		InterruptManager* interruptManager; //pointer for the InterruptManager to connect to.

		InterruptHandler(uint8_t interruptNumber, InterruptManager* interruptManager);
		~InterruptHandler();

	public:
		virtual uint32_t HandleInterrupt(uint32_t esp);
		
	};

	class InterruptManager{
	friend class InterruptHandler;
	protected:
		// one Active InterruptManager
		static InterruptManager* ActiveInterruptManager;

		// this is something like Interrupt Descriptor Table but in a higher level.
		InterruptHandler* handlers[256];

		struct GateDescriptor
		{
			uint16_t handlerAddressLowBits;
			uint16_t gdt_codeSegmentSelector;
			uint8_t reserved;
			uint8_t access;
			uint16_t handlerAddressHighBits;

		} __attribute__((packed));

		// array with 256 Entries
		static GateDescriptor InterruptDescriptorTable[256];

		// this is will create the table
		struct InterruptDescriptorTablePointer
		{
			uint16_t size;
			uint32_t base;	// the address of the tabel
		}__attribute__((packed));

		uint16_t hardwareInterruptOffset;
		

		static void SetInterruptDescriptorTableEntry(
			uint8_t interruptNumber,
			uint16_t codeSegmentSelectorOffset,
			void (*handler)(),
			uint8_t DescriptorPrivilegeLevel, 
			uint8_t DescriptorType 
			);

		static uint32_t HandleInterrupt(uint8_t interruptNumber, uint32_t esp);	//esp is stack pointer

		Port8BitSlow picMasterCommand;
		Port8BitSlow picMasterData;
		Port8BitSlow picSlaveCommand;
		Port8BitSlow picSlaveData;

	public:
		InterruptManager(uint16_t hardwareInterruptOffset, GlobalDescriptorTable* gdt);
		~InterruptManager();
		uint16_t HardwareInterruptOffset();
		void Activate();
		void Deactivate();

		uint32_t DoHandleInterrupt(uint8_t interruptNumber, uint32_t esp);	//esp is stack pointer


		
		static void HandleInterruptRequest0x00(); // Timer Interrupt
		static void HandleInterruptRequest0x01(); // Keyboard Interrupt
		static void HandleInterruptRequest0x0C(); // Mouse Interrupt

		static void InterruptIgnore();

		static void HandleException0x00();
		static void HandleException0x01();

	};


#endif