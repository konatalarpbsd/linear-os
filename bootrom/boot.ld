ENTRY(_start)

MEMORY
{
    OCM (rwx) : ORIGIN = 0xffff0000 /* 0xFFFC0000 */, LENGTH = 256K
}

PHDRS {

text PT_LOAD FLAGS(5);

data PT_LOAD FLAGS(6);

}


SECTIONS
{
    . = ORIGIN(OCM);

    .text :
    {
        KEEP(*(.text.rom))
        *(.text*)
    } > OCM :text

    .rodata : ALIGN(4K)
    {
        *(.rodata*)
    } > OCM :text

    .data : ALIGN(4K)
    {
        *(.data*)
    } > OCM :data

    .bss : ALIGN(4K)
    {
        __bss_start = .;
        *(.bss*)
        *(COMMON)
        __bss_end = .;
    } > OCM

    __end = .;

   /DISCARD/ :
{
    *(.comment)
    *(.note*)
    *(.eh_frame)
    *(.gnu*)
}

}
