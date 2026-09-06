## Linear OS source code

Linear OS is a hobby operating system for xlnx-zcu102!

Made on shitty redmi 12c with partially broken hardware and barely survived for years😭

Features:

Full boot process(EL3 to EL0 and replaces default firmwares i guess or might not... and replaces fsbl)

Under development:

SATA support, NIC/Ethernet support, USB support, NAND/eMMC, etc.

Future plans:

Other board support, HALs, Hypervisor, More dynamic code instead of hardcoded


I recommend to use qemu amd fork to boot linear OS instead of mainline qemu(qemu-system-aarch64 and qemu-system-microblazeel)
and SystemC if you wanna run those qemus(remote-port) but im still building my own pmufw 
