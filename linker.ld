/* linker.ld: Script de enlace para Multiboot */

ENTRY(_start)

SECTIONS
{
    /* El kernel arranca en 1MB (dirección estándar Multiboot) */
    . = 1M;

    /* Código con Header Multiboot al inicio */
    .text :
    {
        *(.multiboot)
        *(.text)
    }

    /* Datos de solo lectura */
    .rodata ALIGN(4K) :
    {
        *(.rodata)
        *(.rodata.*)
    }

    /* Datos inicializados */
    .data ALIGN(4K) :
    {
        *(.data)
    }

    /* Área del disco simulado */
    .disk_image ALIGN(4K) :
    {
        disk_image_start = .;
        *(.disk_image)
        disk_image_end = .;
    }

    /* BSS - datos no inicializados */
    .bss ALIGN(4K) :
    {
        *(.bss)
        *(COMMON)
    }
}