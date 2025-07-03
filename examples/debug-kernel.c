// debug-kernel.c - Kernel que escribe tanto a VGA como a puerto serial
#include <stdint.h>

// Constantes VGA
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
static uint16_t *const VGA_BUFFER = (uint16_t *)0xB8000;

// Puerto serial COM1
#define SERIAL_PORT 0x3F8

// Función para leer desde puerto
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// Función para escribir a puerto
static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

// Función para escribir a puerto serial
static void serial_putchar(char c) {
    // Esperar hasta que el puerto esté listo
    while ((inb(SERIAL_PORT + 5) & 0x20) == 0);
    // Enviar carácter
    outb(SERIAL_PORT, c);
}

// Inicializar puerto serial
static void serial_init() {
    outb(SERIAL_PORT + 1, 0x00);    // Disable all interrupts
    outb(SERIAL_PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    outb(SERIAL_PORT + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
    outb(SERIAL_PORT + 1, 0x00);    //                  (hi byte)
    outb(SERIAL_PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
    outb(SERIAL_PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
    outb(SERIAL_PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set
}

// Función para escribir string a serial
void serial_print(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        serial_putchar(str[i]);
    }
}

// Función simple para escribir directamente a VGA
void vga_print(const char* str) {
    static int pos = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '\n') {
            pos = ((pos / VGA_WIDTH) + 1) * VGA_WIDTH;
        } else {
            VGA_BUFFER[pos++] = (uint16_t)str[i] | 0x0F00;  // Blanco sobre negro
        }
        if (pos >= VGA_WIDTH * VGA_HEIGHT) pos = 0;
    }
}

// Función que escribe a ambos sitios
void debug_print(const char* str) {
    vga_print(str);
    serial_print(str);
}

// Punto de entrada del kernel
void kernel_main(void) {
    // Inicializar puerto serial
    serial_init();
    
    // Limpiar pantalla VGA
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        VGA_BUFFER[i] = 0x0F20;  // Espacio blanco sobre negro
    }
    
    // Escribir mensajes de prueba a ambos sitios
    debug_print("=== KERNEL DEBUG TEST ===\n");
    debug_print("Kernel iniciado correctamente!\n");
    debug_print("VGA y Serial funcionando.\n");
    debug_print("Si ves esto, el kernel esta vivo!\n");
    debug_print("\nPresiona Ctrl+A luego X para salir de QEMU.\n");
    
    // Loop infinito
    while (1) {
        __asm__ volatile ("hlt");
    }
}