#include <formatting.h>
#include <io.h>
#include <pci/pci.h>
#include <pci/pcitypes.h>
#include <stdio.h>
#include <system.h>

// Config bit layout:
// Bit 31: Enable
// Bits 30-24: Reserved
// Bits 23-16: Bus number
// Bits 15-11: Device number
// Bits 10-8: Function number
// Bits 7-2: Register number
// Bits 1-0: Reserved

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA    0xCFC

uint32 enforceConfigReservedBits(uint32 config) {
	if (config & 0x3) {
		printf("WARNING: PCI config address has reserved bits set: 0x%x\n", config);
	}
	if (config & 0x7f000000) {
		printf("WARNING: PCI config address has reserved bits set: 0x%x\n", config);
	}
	return config;
}

void pciWrite(uint32 address, uint32 data) {
	outl(PCI_CONFIG_ADDRESS, enforceConfigReservedBits(address));
	outl(PCI_CONFIG_DATA, data);
}

uint32 pciRead(uint32 address) {
	outl(PCI_CONFIG_ADDRESS, enforceConfigReservedBits(address));
	return inl(PCI_CONFIG_DATA);
}

uint32 pciReadConfig(uint8 bus, uint8 device, uint8 function, uint8 reg) {
	uint32 address = 0x80000000 | (((uint32)bus) << 16) | (((uint32)device) << 11) | (((uint32)function) << 8) | (((uint32)reg) << 2);
	return pciRead(address);
}

void pciWriteConfig(uint8 bus, uint8 device, uint8 function, uint8 reg, uint32 data) {
	uint32 address = 0x80000000 | (((uint32)bus) << 16) | (((uint32)device) << 11) | (((uint32)function) << 8) | (((uint32)reg) << 2);
	pciWrite(address, data);
}

typedef struct BARInfo {
	uint64 base;
	uint64 size;
	bool isMemory;
	bool is64Bit;        // Memory only
	bool isPrefetchable; // Memory only
} BARInfo;

BARInfo getBARInfo(uint8 bus, uint8 device, uint8 function, uint8 BAR) {
	BARInfo info = {0};

	uint32 initialBARState = pciReadConfig(bus, device, function, BAR + 4);

	info.isMemory = !(initialBARState & 0b1);

	if (info.isMemory) {
		info.is64Bit = (initialBARState >> 2) & 0b1;
		info.isPrefetchable = (initialBARState >> 3) & 0b1;

		if (info.is64Bit) {
			uint32 upperAddressBAR = pciReadConfig(bus, device, function, BAR + 4 + 1);
			info.base = ((uint64)initialBARState & 0xFFFFFFF0) | ((uint64)upperAddressBAR << 32);

			pciWriteConfig(bus, device, function, BAR + 4, 0xFFFFFFFF);
			uint32 size = pciReadConfig(bus, device, function, BAR + 4);
			pciWriteConfig(bus, device, function, BAR + 4, initialBARState);

			info.size = (~(size & 0xFFFFFFF0)) + 1;
		} else { // 32-bit
			info.base = initialBARState & 0xFFFFFFF0;

			pciWriteConfig(bus, device, function, BAR + 4, 0xFFFFFFFF);
			uint32 size = pciReadConfig(bus, device, function, BAR + 4);
			pciWriteConfig(bus, device, function, BAR + 4, initialBARState);

			info.size = (~(size & 0xFFFFFFF0)) + 1;
		}
	} else { // I/O
		info.base = initialBARState & 0xFFFFFFFC;
	}

	return info;
}

typedef struct CommonHeader {
	uint16 vendorID;
	uint16 deviceID;

	uint16 command;
	uint16 status;

	uint8 revisionID;
	uint8 progIF;
	uint8 subclass;
	uint8 class;

	uint8 cacheLineSize;
	uint8 latencyTimer;
	uint8 headerType;
	uint8 BIST;
} CommonHeader;

COMPILE_TIME_ASSERT(sizeof(CommonHeader) == 16, PCI_COMMON_HEADER_MUST_BE_16_BYTES);

void probePCIDevice(uint8 bus, uint8 device, uint8 function) {
	printf("Probing PCI device %x:%x:%x\n", bus, device, function);

	uint32 config[4];

	for (uint32 reg = 0; reg < 4; reg++) {
		config[reg] = pciReadConfig(bus, device, function, reg);

		printf("reg 0x%x : %04x %04x\n", reg, (config[reg] >> 16) & 0xFFFF, config[reg] & 0xFFFF);
	}

	CommonHeader *header = (CommonHeader *)config;

	if (header->headerType == 0) {
		for (uint8 i = 0; i < 6; i++) {
			BARInfo info = getBARInfo(bus, device, function, i);

			printf("BAR%i : Base: %lx, Size: %li %s, Type: %s", i, info.base, numBytesToHuman(info.size), numBytesToUnit(info.size), info.isMemory ? "Mem" : "I/O");
			if (info.isMemory) {
				printf(", %s", info.is64Bit ? "64-bit" : "32-bit");
				if (info.isPrefetchable) {
					printf(", Prefetchable");
				}
			}
			printf("\n");

			if (info.is64Bit) {
				i++; // Skip the upper 32 bits of this BAR
			}
		}
	}
}

void enumeratePCIDevices() {
	printf("Enumerating PCI devices...\n");
	for (uint8 bus = 0; bus < 255; bus++) {
		for (uint8 device = 0; device < 32; device++) {
			for (uint8 function = 0; function < 8; function++) {
				uint16 vendorID = pciReadConfig(bus, device, function, 0) & 0xFFFF;
				if (vendorID != 0xFFFF) {
					uint16 deviceID = (pciReadConfig(bus, device, function, 0) >> 16) & 0xFFFF;

					uint32 register2 = pciReadConfig(bus, device, function, 0x2);

					uint8 classCode = (register2 >> 24) & 0xFF;
					uint8 subclassCode = (register2 >> 16) & 0xFF;
					uint8 interfaceCode = (register2 >> 8) & 0xFF;

					printf("Found PCI device: %02x:%02x.%x -> %04x:%04x (Class %02x, Subclass %02x, IF %02x) - %s\n", bus, device, function, vendorID, deviceID, classCode, subclassCode, interfaceCode, getDeviceType(classCode, subclassCode, interfaceCode));
				}
			}
		}
	}
}
