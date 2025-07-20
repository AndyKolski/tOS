#include <formatting.h>
#include <io.h>
#include <pci/devices.h>
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

#define MAX_PCI_DEVICES 32
pci_device_t pci_devices[MAX_PCI_DEVICES];
uint32 pci_device_count;

#define MAX_PCI_DRIVERS 16
pci_driver_t *pci_drivers[MAX_PCI_DRIVERS];
uint32 pci_driver_count = 0;

void pci_register_driver(pci_driver_t *driver) {
	if (pci_driver_count < MAX_PCI_DRIVERS) {
		pci_drivers[pci_driver_count++] = driver;
	} else {
		printf("PCI driver registry full\n");
	}
}

bool check_driver_matches_device(pci_driver_t *driver, pci_device_t *device) {
	return (driver->vendor_id == PCI_ANY_ID || driver->vendor_id == device->vendor_id) &&       // Vendor ID
	       (driver->device_id == PCI_ANY_ID || driver->device_id == device->device_id) &&       // Device ID
	       (driver->class_code == PCI_ANY_CLASS || driver->class_code == device->class_code) && // Class
	       (driver->subclass == PCI_ANY_CLASS || driver->subclass == device->subclass) &&       // Subclass
	       (driver->prog_if == PCI_ANY_CLASS || driver->prog_if == device->prog_if);            // Programming interface
}

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

typedef struct commandRegister {
	uint16 ioSpace                  : 1;
	uint16 memorySpace              : 1;
	uint16 busMaster                : 1;
	uint16 specialCycles            : 1; // Deprecated in PCIe, should always be 0
	uint16 memoryWriteAndInvalidate : 1; // Deprecated in PCIe, should always be 0
	uint16 vgaPaletteSnoop          : 1; // Deprecated in PCIe, should always be 0
	uint16 parityErrorResponse      : 1;
	uint16 reserved1                : 1; // Deprecated in PCIe, should always be 0
	uint16 serrEnable               : 1;
	uint16 fastBackToBack           : 1; // Read only
	uint16 interruptDisable         : 1;
	uint16 reserved2                : 4;
} commandRegister;

COMPILE_TIME_ASSERT(sizeof(commandRegister) == 2, PCI_COMMAND_REGISTER_MUST_BE_2_BYTES);

typedef struct statusRegister {
	uint16 reserved1           : 3;
	uint16 interruptStatus     : 1;
	uint16 capabilitiesList    : 1; // PCIe devices are required to have this bit set
	uint16 is66MHzCapable      : 1; // Deprecated in PCIe, should always be 0
	uint16 reserved2           : 1;
	uint16 fastBackToBack      : 1; // Deprecated in PCIe, should always be 0
	uint16 dataParityError     : 1;
	uint16 devselTiming        : 2; // Deprecated in PCIe, should always be 0
	uint16 signaledTargetAbort : 1;
	uint16 receivedTargetAbort : 1;
	uint16 receivedMasterAbort : 1;
	uint16 signaledSystemError : 1;
	uint16 detectedParityError : 1;
} statusRegister;

COMPILE_TIME_ASSERT(sizeof(statusRegister) == 2, PCI_STATUS_REGISTER_MUST_BE_2_BYTES);

typedef struct CommonHeader {
	uint16 vendorID;
	uint16 deviceID;

	commandRegister command;
	statusRegister status;

	uint8 revisionID;
	uint8 progIF;
	uint8 subclass;
	uint8 class;

	uint8 cacheLineSize; // Deprecated in PCIe, should always be 0
	uint8 latencyTimer;  // Deprecated in PCIe, should always be 0
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

	if (header->headerType & 0x80) {
		printf("Device is a multi-function device\n");
	}

	if ((header->headerType & 0x7f) == 0) { // header type 0 - general device

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
					pci_device_t thisDevice;

					uint16 deviceID = (pciReadConfig(bus, device, function, 0) >> 16) & 0xFFFF;

					uint32 register2 = pciReadConfig(bus, device, function, 0x2);

					uint8 classCode = (register2 >> 24) & 0xFF;
					uint8 subclassCode = (register2 >> 16) & 0xFF;
					uint8 interfaceCode = (register2 >> 8) & 0xFF;

					thisDevice.bus = bus;
					thisDevice.device = device;
					thisDevice.function = function;

					thisDevice.vendor_id = vendorID;
					thisDevice.device_id = deviceID;

					thisDevice.class_code = classCode;
					thisDevice.subclass = subclassCode;
					thisDevice.prog_if = interfaceCode;

					thisDevice.irq = pciReadConfig(bus, device, function, 0xf) & 0xFF;

					for (uint8 i = 0; i < 6; i++) {
						BARInfo info = getBARInfo(bus, device, function, i);
						thisDevice.bars[i] = info;

						if (info.is64Bit) {
							i++; // Skip the upper 32 bits of this BAR
						}
					}

					if (pci_device_count < MAX_PCI_DEVICES) {
						pci_devices[pci_device_count++] = thisDevice;
					} else {
						printf("Unable to register PCI device\n");
					}

					// printf("Found PCI device: %02x:%02x.%x -> %04x:%04x (Class %02x, Subclass %02x, IF %02x) - %s\n", bus, device, function, vendorID, deviceID, classCode, subclassCode,
					// interfaceCode, getDeviceType(thisDevice));
				}
			}
		}
	}
}

void initPCI() {
	// Check if the PCI bus is present
	if (pciReadConfig(0, 0, 0, 0) == 0xFFFFFFFF) {
		printf("No PCI bus found.\n");
		return;
	}

	register_pci_drivers();

	enumeratePCIDevices();

	for (uint32_t i = 0; i < pci_device_count; i++) {
		for (size_t j = 0; j < pci_driver_count; j++) {
			if (check_driver_matches_device(pci_drivers[j], &pci_devices[i])) {
				if (pci_drivers[j]->probe(&pci_devices[i])) {
					break; // this driver works, no need to continue
				}
			}
		}
	}
}
