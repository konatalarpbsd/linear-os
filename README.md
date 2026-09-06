## Linear OS source code

Linear OS is a hobby operating system for xlnx-zcu102!

Made on shitty redmi 12c with partially broken hardware and barely survived for years😭

Features:

Full boot process(EL3 to EL0 and replaces default firmwares maybe... and replaces fsbl, PMUFW)

Under development:

SATA support, NIC/Ethernet support, USB support, NAND/eMMC, etc.

Future plans:

Other board support, HALs, Hypervisor, More dynamic code instead of hardcoded


I recommend to use qemu amd fork to boot linear OS instead of mainline qemu
You need to compile both qemu-system-aarch64(Main OS) and qemu-system-microblazeel(for PMUFW)
and use SystemC to run those qemus with remote-port and emulate Programmable Logic, etc.

Im broke to buy xlnx-zcu102😐😐😐 so im currently testing it on qemu amd fork with systemc/verilator

Currently building bootrom(also called loader) which is replacement for fsbl
