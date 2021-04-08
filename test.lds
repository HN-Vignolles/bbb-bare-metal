MEMORY
{
    a8ram   : org = 0x402F0000, len = 0x10000
}

/*PHDRS {
    image PT_LOAD;
}*/

OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)

SECTIONS
{
        .reserved 0x402F0000 (NOLOAD) : { . += 0x400; } >a8ram :NONE
        . = 0x402F0400;
        
        .startcode :
        {
            . = ALIGN(4);
            *init.o      (.text)
        } >a8ram /*:image*/

        .text :
        {
            . = ALIGN(4);
            *(.text*)
            *(.rodata*)
        } >a8ram /*:image*/

        .data :
        {
            . = ALIGN(4);
            *(.data*)
        } >a8ram /*:image*/

        .bss :
        {
            . = ALIGN(4);
            _bss_start = .;
            *(.bss*)
            *(COMMON)
            _bss_end = .;
        } >a8ram /*:image*/
        .heap :
        {
            . = ALIGN(4);
            __end__ = .;
            end = __end__;
            __HeapBase = __end__;
            *(.heap*)
            . = . + 0x800;
            __HeapLimit = .;
        } >a8ram /*:image*/

        .stack :
        {
            . = ALIGN(4);
            __StackLimit = . ;
            *(.stack*)
            . = . + 0x4000;
            __StackTop = .;
        } >a8ram /*:NONE*/
        _stack = __StackTop;

}
