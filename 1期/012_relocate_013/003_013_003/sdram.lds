SECTIONS {
   .text   0  : { *(.text) }
   .rodata  : { *(.rodata) }
   .data 0x30000000 : AT(0x800) { *(.data) }
   .bss  : { *(.bss) *(.COMMON) }
}