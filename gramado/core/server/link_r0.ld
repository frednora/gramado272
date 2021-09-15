
OUTPUT_FORMAT("elf64-x86-64")
ENTRY(main)

/* Essa aplicação está usando o diretório do processo kernel. */

SECTIONS
{ 
    .text 0x30A01000: 
    {
        code = .; 
        _code = .; 
        __code = .;

        *(.head_x86)
        *(.text)

       . = ALIGN(4096);
    }

    .data :
    {
        data = .; 
        _data = .; 
        __data = .;

        *(.data)

        . = ALIGN(4096);
    }

    .bss :
    {
        bss = .; 
        _bss = .; 
        __bss = .;

        *(.bss)

        . = ALIGN(4096);
    }

    end = .; 
    _end = .; 
    __end = .;
}

