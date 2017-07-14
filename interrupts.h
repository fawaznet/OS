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
		static void HandleInterruptRequest0x02(); // Mouse Interrupt
		static void HandleInterruptRequest0x03(); // Timer Interrupt
		static void HandleInterruptRequest0x04(); // Keyboard Interrupt
		static void HandleInterruptRequest0x05(); // Mouse Interrupt
		static void HandleInterruptRequest0x06(); // Timer Interrupt
		static void HandleInterruptRequest0x07(); // Keyboard Interrupt
		static void HandleInterruptRequest0x08(); // Mouse Interrupt
		static void HandleInterruptRequest0x09(); // Timer Interrupt
		static void HandleInterruptRequest0x0A(); // Keyboard Interrupt
		static void HandleInterruptRequest0x0B(); // Mouse Interrupt
		static void HandleInterruptRequest0x0C(); // Mouse Interrupt
		static void HandleInterruptRequest0x0D(); // Keyboard Interrupt
		static void HandleInterruptRequest0x0E(); // Mouse Interrupt
		static void HandleInterruptRequest0x0F(); // Timer Interrupt
		static void HandleInterruptRequest0x31(); // Keyboard Interrupt
		

		static void InterruptIgnore();

		static void HandleException0x00();
		static void HandleException0x01();
		static void HandleException0x02();
		static void HandleException0x03();
		static void HandleException0x04();
		static void HandleException0x05();
		static void HandleException0x06();
		static void HandleException0x07();
		static void HandleException0x08();
		static void HandleException0x09();
		static void HandleException0x0A();
		static void HandleException0x0B();
		static void HandleException0x0C();
		static void HandleException0x0D();
		static void HandleException0x0E();
		static void HandleException0x0F();
		static void HandleException0x10();
		static void HandleException0x11();
		static void HandleException0x12();
		static void HandleException0x13();

	};


#endif