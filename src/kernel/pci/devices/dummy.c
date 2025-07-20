#include <pci/pci.h>
#include <pci/pcitypes.h>
#include <system.h>
#include <stdio.h>

bool dummy_probe(pci_device_t *dev) {
	printf("Dummy driver matched device %04x:%04x (%s)\n", dev->vendor_id, dev->device_id, getDeviceType(dev));
	return true;
}

pci_driver_t dummy_driver = {
	.name = "dummy",
	.vendor_id = PCI_ANY_ID,
	.device_id = PCI_ANY_ID,
	.class_code = PCI_ANY_CLASS,
	.subclass = PCI_ANY_CLASS,
	.prog_if = PCI_ANY_CLASS,
	.probe = dummy_probe,
};

void register_pci_dummy_driver() {
	pci_register_driver(&dummy_driver);
}
