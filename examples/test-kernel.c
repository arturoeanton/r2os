// test-kernel.c - Kernel mínimo para debugging
#include <stdint.h>

// Constantes VGA
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
static uint16_t *const VGA_BUFFER = (uint16_t *)0xB8000;

// Función simple para escribir directamente a VGA
void write_string(const char* str) {
    static int pos = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        VGA_BUFFER[pos++] = (uint16_t)str[i] | 0x0F00;  // Blanco sobre negro
        if (pos >= VGA_WIDTH * VGA_HEIGHT) pos = 0;
    }
}

// Punto de entrada del kernel
void kernel_main(void) {
    // Limpiar pantalla
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        VGA_BUFFER[i] = 0x0F20;  // Espacio blanco sobre negro
    }
    
    // Escribir mensaje de prueba
    write_string("KERNEL TEST - FUNCIONANDO!");
    write_string("\nEste es un kernel de prueba minimo.");
    write_string("\nSi ves este mensaje, VGA funciona.");
    
    // Loop infinito
    while (1) {
        // Hacer nada - solo mantener el kernel vivo
        __asm__ volatile ("hlt");
    }
}