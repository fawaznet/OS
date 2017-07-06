#ifndef __INTERRUPTS_H
#define __INTERRUPTS_H

#include "types.h"
#include "ports.h"
#include "gdt.h"

	class InterruptManager{
	protected:
		struct GateDescriptor
		{
			uint16_t handlerAddressLowBits;
			uint8_t gdt_codeSegmentSelector;
			uint8_t reserved;
			uint8_t access;
			uint16_t handlerAddressHighBits
		}; __attribute__((packed))

		// array with 256 Entries
		static GateDescriptor interrupDescriptorTable[256];

		static SetinterrupDescriptorTableEntry(
			uint8_t interruptNumber,
			uint16_t codeSegmentSelectorOffset,
			void (*handler)(),
			uint8_t DescriptorPrivilegeLevel, /*access*/
			uint8_t DescriptorType /*flags*/
			);

	public:
		InterruptManager(GlobalDescriptionTable* gdt);
		~InterruptManager();

		static uint32_t handleInterrupt(uint8_t interruptNumber, uint32_t esp);	//esp is stack pointer

		static void IgnoreInterruptRequest();
		static void HandleInterruptRequest0x00(); // Timer Interrupt
		static void HandleInterruptRequest0x01(); // Keyboard Interrupt

	};


#endif