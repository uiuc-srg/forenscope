cmd_drivers/serial/built-in.o :=  ld -m elf_i386 -m elf_i386  -r -o drivers/serial/built-in.o drivers/serial/serial_core.o drivers/serial/8250.o drivers/serial/8250_pnp.o drivers/serial/8250_pci.o
