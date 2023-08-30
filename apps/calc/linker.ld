ENTRY(_Start)
SECTIONS
{
    .text : { *(.text) }

    .data : { *(.data) }
        
    .bss : { *(.bss) }

}
