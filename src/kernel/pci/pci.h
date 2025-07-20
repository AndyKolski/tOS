#include <system.h>

#pragma once

typedef struct BARInfo {
	uint64_t base;
	uint64_t size;
	bool isMemory;
	bool is64Bit;        // Memory only
	bool isPrefetchable; // Memory only
} BARInfo;

typedef struct pci_device {
	uint8_t bus, device, function;
	uint16_t vendor_id, device_id;
	uint8_t class_code, subclass, prog_if;
	uint8_t irq;
	BARInfo bars[6];
} pci_device_t;

typedef struct pci_driver {
	const char *name;
	uint16_t vendor_id, device_id;
	uint8_t class_code, subclass, prog_if;

	bool (*probe)(pci_device_t *dev);
	void (*remove)(pci_device_t *dev);
} pci_driver_t;

#define PCI_ANY_ID    0xFFFF
#define PCI_ANY_CLASS 0xFF

void initPCI();

void pci_register_driver(pci_driver_t *driver);