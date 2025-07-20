#include <pci/pci.h>

#include <pci/devices/dummy.h>

void register_pci_drivers() {
	register_pci_dummy_driver();
}