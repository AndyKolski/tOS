#include <system.h>
#include <stdio.h>
#include <pci/pci.h>
#include <io.h>

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

char *getDeviceType(uint8 classCode, uint8 subclass, uint8 interface) {
	switch (classCode) {
		case 0x00:
			switch (subclass) {
				case 0x00: return "Non-VGA Unspecified Device";
				case 0x01: return "VGA-Compatible Device";
				default: return "Unknown / Reserved Unspecified Device";
			}
		case 0x01:
			switch (subclass) {
				case 0x00: return "SCSI Bus Controller";
				case 0x01: return "IDE Controller";
				case 0x02: return "Floppy Disk Controller";
				case 0x03: return "IPI Bus Controller";
				case 0x04: return "RAID Controller";
				case 0x05: return "ATA Controller";
				case 0x06: return "SATA Controller";
				case 0x07: return "Serial Attached SCSI Controller";
				case 0x08: return "Non-Volatile Memory Controller";
				case 0x80: return "Other Mass Storage Controller";
				default: return "Unknown / Reserved Mass Storage Controller";
			}
		case 0x02:
			switch (subclass) {
				case 0x00: return "Ethernet Controller";
				case 0x01: return "Token Ring Controller";
				case 0x02: return "FDDI Controller";
				case 0x03: return "ATM Controller";
				case 0x04: return "ISDN Controller";
				case 0x05: return "WorldFip Controller";
				case 0x06: return "PICMG 2.14 Multi Computing";
				case 0x07: return "Infiniband Controller";
				case 0x08: return "Fabric Controller";
				case 0x80: return "Other Network Controller";
				default: return "Unknown / Reserved Network Controller";
			}
		case 0x03:
			switch (subclass) {
				case 0x00: return "VGA-Compatible Controller";
				case 0x01: return "XGA Controller";
				case 0x02: return "3D Controller (Not VGA-Compatible)";
				case 0x80: return "Other Display Controller";
				default: return "Unknown / Reserved Display Controller";
			}
		case 0x04:
			switch (subclass) {
				case 0x00: return "Video Device";
				case 0x01: return "Audio Device";
				case 0x02: return "Computer Telephony Device";
				case 0x03: return "Audio Device";
				case 0x80: return "Other Multimedia Controller";
				default: return "Unknown / Reserved Multimedia Controller";
			}
		case 0x05:
			switch (subclass) {
				case 0x00: return "RAM Controller";
				case 0x01: return "Flash Controller";
				case 0x80: return "Other Memory Controller";
				default: return "Unknown / Reserved Memory Controller";
			}
		case 0x06:
			switch (subclass) {
				case 0x00: return "Host Bridge";
				case 0x01: return "ISA Bridge";
				case 0x02: return "EISA Bridge";
				case 0x03: return "MCA Bridge";
				case 0x04: return "PCI-to-PCI Bridge";
				case 0x05: return "PCMCIA Bridge";
				case 0x06: return "NuBus Bridge";
				case 0x07: return "CardBus Bridge";
				case 0x08: return "RACEway Bridge";
				case 0x09: return "PCI-to-PCI Bridge";
				case 0x0A: return "InfiniBand-to-PCI Host Bridge";
				case 0x80: return "Other Bridge Device";
				default: return "Unknown / Reserved Bridge Device";
			}
		case 0x07:
			switch (subclass) {
				case 0x00: return "Serial Controller";
				case 0x01: return "Parallel Controller";
				case 0x02: return "Multiport Serial Controller";
				case 0x03: return "Modem";
				case 0x04: return "GPIB (IEEE 488.1/2) Controller";
				case 0x05: return "Smart Card";
				case 0x80: return "Other Communication Controller";
				default: return "Unknown / Reserved Communication Controller";
			}
		case 0x08:
			switch (subclass) {
				case 0x00: return "PIC";
				case 0x01: return "DMA Controller";
				case 0x02: return "System Timer";
				case 0x03: return "RTC Controller";
				case 0x04: return "PCI Hot-Plug Controller";
				case 0x05: return "SD Host Controller";
				case 0x06: return "IOMMU";
				case 0x80: return "Other Generic System Peripheral";
				default: return "Unknown / Reserved Generic System Peripheral";
			}
		case 0x09:
			switch (subclass) {
				case 0x00: return "Keyboard Controller";
				case 0x01: return "Digitizer (Pen)";
				case 0x02: return "Mouse Controller";
				case 0x03: return "Scanner Controller";
				case 0x04: return "Gameport Controller";
				case 0x80: return "Other Input Controller";
				default: return "Unknown / Reserved Input Controller";
			}
		case 0x0A: return "Docking Station";
		case 0x0B:
			switch (subclass) {
				case 0x00: return "386 Processor";
				case 0x01: return "486 Processor";
				case 0x02: return "Pentium Processor";
				case 0x03: return "Pentium Pro Processor";
				case 0x10: return "Alpha Processor";
				case 0x20: return "PowerPC Processor";
				case 0x30: return "MIPS Processor";
				case 0x40: return "Co-Processor";
				case 0x80: return "Other Processor";
				default: return "Unknown / Reserved Processor";
			}
		case 0x0C:
			switch (subclass) {
				case 0x00: return "IEEE 1394 FireWire Controller";
				case 0x01: return "ACCESS Bus";
				case 0x02: return "SSA";
				case 0x03:
					switch (interface) {
						case 0x00: return "UHCI USB Controller";
						case 0x10: return "OHCI USB Controller";
						case 0x20: return "EHCI (USB2) Controller";
						case 0x30: return "XHCI (USB3) Controller";
						case 0x80: return "Unspecified USB Controller";
						case 0xFE: return "USB Device?";
						default: return "Unknown USB Controller";
					}
				case 0x04: return "Fibre Channel";
				case 0x05: return "SMBus";
				case 0x06: return "InfiniBand";
				case 0x07: return "IPMI Interface";
				case 0x08: return "SERCOS Interface (IEC 61491)";
				case 0x09: return "CANbus";
				case 0x80: return "Other Serial Bus Controller";
				default: return "Unknown / Reserved Serial Bus Controller";
			}
		case 0x0D:
			switch (subclass) {
				case 0x00: return "iRDA Compatible Controller";
				case 0x01: return "Consumer IR Controller";
				case 0x10: return "RF Controller";
				case 0x11: return "Bluetooth Controller";
				case 0x12: return "Broadband Controller";
				case 0x20: return "Ethernet Controller (802.11a)";
				case 0x21: return "Ethernet Controller (802.11b)";
				case 0x80: return "Other Wireless Controller";
				default: return "Unknown / Reserved Wireless Controller";
			}
		case 0x0E: return "Intelligent I/O Controller";
		case 0x0F:
			switch (subclass) {
				case 0x01: return "Satellite TV Controller";
				case 0x02: return "Satellite Audio Controller";
				case 0x03: return "Satellite Voice Controller";
				case 0x04: return "Satellite Data Controller";
				case 0x80: return "Other Satellite Communication Controller";
				default: return "Unknown / Reserved Satellite Communication Controller";
			}
		case 0x10:
			switch (subclass) {
				case 0x00: return "Network and Computing Encryption/Decryption";
				case 0x10: return "Entertainment Encryption/Decryption";
				case 0x80: return "Other Encryption/Decryption Controller";
				default: return "Unknown / Reserved Encryption/Decryption Controller";
			}
		case 0x11:
			switch (subclass) {
				case 0x00: return "DPIO Modules";
				case 0x01: return "Performance Counters";
				case 0x10: return "Communications Synchronizer";
				case 0x20: return "Signal Processing Management";
				case 0x80: return "Other Data Acquisition/Signal Processing Controller";
				default: return "Unknown / Reserved Data Acquisition/Signal Processing Controller";
			}

		case 0x12: return "Processing Accelerator";
		case 0x13: return "Non-Essential Instrumentation";
		case 0x40: return "Co-Processor";
		case 0xFF: return "Unassigned Class";

		default: return "Unknown / Reserved";
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
					
					uint8 classCode = (pciReadConfig(bus, device, function, 0x8) >> 24) & 0xFF;
					uint8 subclassCode = (pciReadConfig(bus, device, function, 0x8) >> 16) & 0xFF;
					uint8 interfaceCode = (pciReadConfig(bus, device, function, 0x8) >> 8) & 0xFF;

					printf("Found PCI device: %02x:%02x.%x -> %04x:%04x (Class %02x, Subclass %02x, IF %02x) - %s\n", bus, device, function, vendorID, deviceID, classCode, subclassCode, interfaceCode, getDeviceType(classCode, subclassCode, interfaceCode));
				}
			}
		}
	}
}