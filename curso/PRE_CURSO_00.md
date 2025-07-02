# PRE-CURSO 00: Fundamentos de C y Ensamblador

## 📚 Introducción

Este capítulo cubre los conocimientos mínimos de programación en C y ensamblador x86 necesarios para seguir el curso de sistemas operativos. Si ya tienes experiencia en estos lenguajes, puedes usar este material como referencia rápida.

## 🎯 Objetivos de Aprendizaje

Al completar este pre-curso, deberás ser capaz de:

1. **Escribir y entender** programas básicos en C freestanding
2. **Leer y modificar** código ensamblador x86 de 32 bits
3. **Comprender** la interacción entre C y ensamblador
4. **Usar** punteros, estructuras y memoria de manera eficiente
5. **Aplicar** conceptos de bajo nivel en programación de sistemas

---

## 🔧 Parte I: Fundamentos de C para Sistemas

### 1.1 C Freestanding vs C Estándar

#### Diferencias Clave

```c
// ❌ NO DISPONIBLE en C freestanding (kernel space)
#include <stdio.h>    // No hay printf, scanf, etc.
#include <stdlib.h>   // No hay malloc, free, exit
#include <string.h>   // No hay strcpy, strlen, etc.

// ✅ DISPONIBLE en C freestanding
#include <stdint.h>   // Tipos enteros estándar
#include <stddef.h>   // size_t, NULL, offsetof
#include <stdbool.h>  // bool, true, false
#include <limits.h>   // Límites de tipos
```

#### Implementación Propia de Funciones Básicas

```c
// Implementamos nuestras propias funciones de string
int strlen(const char *str) {
    int len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

void strcpy(char *dest, const char *src) {
    int i = 0;
    while (src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

int strcmp(const char *str1, const char *str2) {
    int i = 0;
    while (str1[i] != '\0' && str2[i] != '\0') {
        if (str1[i] != str2[i]) {
            return str1[i] - str2[i];
        }
        i++;
    }
    return str1[i] - str2[i];
}

// Funciones de memoria
void *memcpy(void *dest, const void *src, size_t n) {
    char *d = (char *)dest;
    const char *s = (const char *)src;
    
    for (size_t i = 0; i < n; i++) {
        d[i] = s[i];
    }
    return dest;
}

void *memset(void *ptr, int value, size_t n) {
    char *p = (char *)ptr;
    for (size_t i = 0; i < n; i++) {
        p[i] = (char)value;
    }
    return ptr;
}
```

### 1.2 Manejo de Memoria y Punteros

#### Punteros Básicos

```c
// Declaración y uso básico
int valor = 42;
int *puntero = &valor;          // Puntero a la dirección de 'valor'
int contenido = *puntero;       // Desreferenciación: contenido = 42

// Aritmética de punteros
char buffer[10] = "Hello";
char *ptr = buffer;             // Apunta al primer elemento
ptr++;                          // Ahora apunta a 'e'
ptr += 2;                       // Ahora apunta a 'l' (tercer carácter)
```

#### Punteros en Sistemas Operativos

```c
// Direcciones de memoria específicas (mapeo de hardware)
#define VGA_MEMORY 0xB8000
uint16_t *vga_buffer = (uint16_t *)VGA_MEMORY;

// Escribir directamente en memoria de video
void escribir_caracter(int x, int y, char c, uint8_t color) {
    int offset = y * 80 + x;
    vga_buffer[offset] = (uint16_t)c | ((uint16_t)color << 8);
}

// Punteros a funciones (callbacks y handlers)
typedef void (*interrupt_handler_t)(void);

interrupt_handler_t handlers[256];  // Tabla de handlers

void registrar_handler(int irq, interrupt_handler_t handler) {
    handlers[irq] = handler;
}
```

### 1.3 Estructuras y Tipos de Datos

#### Estructuras Básicas

```c
// Estructura simple
typedef struct {
    uint32_t id;
    char nombre[32];
    uint8_t activo;
} proceso_t;

// Uso de la estructura
proceso_t proc;
proc.id = 1;
strcpy(proc.nombre, "init");
proc.activo = 1;
```

#### Estructuras Alineadas para Hardware

```c
// Estructura que debe coincidir exactamente con formato de hardware
typedef struct {
    uint8_t  bootable;       // 1 byte
    uint8_t  start_head;     // 1 byte  
    uint16_t start_sector;   // 2 bytes
    uint8_t  type;           // 1 byte
    uint8_t  end_head;       // 1 byte
    uint16_t end_sector;     // 2 bytes
    uint32_t start_lba;      // 4 bytes
    uint32_t size;           // 4 bytes
} __attribute__((packed)) partition_entry_t;  // Sin padding automático

// Verificar tamaño en tiempo de compilación
_Static_assert(sizeof(partition_entry_t) == 16, 
               "Partition entry debe ser exactamente 16 bytes");
```

#### Uniones para Manipulación de Bits

```c
// Unión para acceder a diferentes representaciones del mismo dato
typedef union {
    uint32_t valor_completo;
    struct {
        uint8_t byte0;
        uint8_t byte1; 
        uint8_t byte2;
        uint8_t byte3;
    } bytes;
    struct {
        uint16_t word_bajo;
        uint16_t word_alto;
    } words;
} registro_32_t;

// Uso práctico
registro_32_t reg;
reg.valor_completo = 0x12345678;
printf("Byte 0: 0x%02x\n", reg.bytes.byte0);  // 0x78
printf("Word alto: 0x%04x\n", reg.words.word_alto);  // 0x1234
```

### 1.4 Manipulación de Bits

#### Operaciones Básicas

```c
// Definición de bits específicos
#define BIT_0   (1 << 0)    // 0x01
#define BIT_1   (1 << 1)    // 0x02
#define BIT_7   (1 << 7)    // 0x80

// Operaciones comunes
uint8_t flags = 0;

// Establecer bit
flags |= BIT_1;             // Activar bit 1

// Limpiar bit  
flags &= ~BIT_1;            // Desactivar bit 1

// Verificar bit
if (flags & BIT_1) {        // ¿Está activo el bit 1?
    // El bit está activo
}

// Toggle bit
flags ^= BIT_1;             // Cambiar estado del bit 1
```

#### Máscaras y Campos de Bits

```c
// Extraer campos específicos
#define GET_BITS(value, mask, shift) (((value) & (mask)) >> (shift))
#define SET_BITS(value, mask, shift, new_bits) \
    ((value) = ((value) & ~(mask)) | (((new_bits) << (shift)) & (mask)))

// Ejemplo: Registro de control con múltiples campos
uint32_t control_reg = 0x12345678;

// Campo de 4 bits en posiciones 8-11
#define FIELD_MASK  0x00000F00
#define FIELD_SHIFT 8

uint8_t campo = GET_BITS(control_reg, FIELD_MASK, FIELD_SHIFT);
SET_BITS(control_reg, FIELD_MASK, FIELD_SHIFT, 0xA);
```

### 1.5 Arrays y Gestión de Memoria Estática

#### Arrays de Tamaño Fijo

```c
// Buffer estático para kernel
static char kernel_buffer[4096];  // 4KB buffer
static uint32_t buffer_index = 0;

void agregar_al_buffer(char c) {
    if (buffer_index < sizeof(kernel_buffer) - 1) {
        kernel_buffer[buffer_index++] = c;
        kernel_buffer[buffer_index] = '\0';  // Mantener null-terminated
    }
}
```

#### Simulación de Memoria Dinámica

```c
// Pool de memoria estática para simular malloc
#define MEMORY_POOL_SIZE 65536  // 64KB
static uint8_t memory_pool[MEMORY_POOL_SIZE];
static uint32_t next_free = 0;

void *simple_malloc(size_t size) {
    // Alineación a 4 bytes
    size = (size + 3) & ~3;
    
    if (next_free + size > MEMORY_POOL_SIZE) {
        return NULL;  // Sin memoria
    }
    
    void *ptr = &memory_pool[next_free];
    next_free += size;
    return ptr;
}

// Nota: Este malloc simple no tiene free()
// En un OS real implementaríamos un heap manager completo
```

---

## ⚙️ Parte II: Fundamentos de Ensamblador x86

### 2.1 Registros del Procesador

#### Registros de 32 bits (Modo Protegido)

```nasm
; Registros de propósito general
; EAX - Acumulador (operaciones aritméticas)
; EBX - Base (direccionamiento)
; ECX - Contador (loops)
; EDX - Datos (operaciones I/O)
; ESI - Source Index (operaciones de string)
; EDI - Destination Index (operaciones de string)
; EBP - Base Pointer (stack frames)
; ESP - Stack Pointer (tope del stack)

; Ejemplo de uso básico
mov eax, 42        ; EAX = 42
mov ebx, eax       ; EBX = EAX (EBX = 42)
add eax, ebx       ; EAX = EAX + EBX (EAX = 84)
```

#### Registros de Segmento

```nasm
; Registros de segmento
; CS - Code Segment
; DS - Data Segment  
; ES - Extra Segment
; FS, GS - General purpose segments
; SS - Stack Segment

; Cargar selector de segmento
mov ax, 0x10       ; Selector de segmento de datos
mov ds, ax         ; Cargar en DS
mov es, ax         ; Cargar en ES
```

#### Registro de Flags (EFLAGS)

```nasm
; Flags importantes:
; CF - Carry Flag (acarreo)
; ZF - Zero Flag (resultado cero)
; SF - Sign Flag (resultado negativo)
; IF - Interrupt Flag (interrupciones habilitadas)

; Manipulación de flags
pushfd             ; Guardar EFLAGS en stack
cli                ; Clear Interrupt Flag (deshabilitar interrupciones)
sti                ; Set Interrupt Flag (habilitar interrupciones)
popfd              ; Restaurar EFLAGS desde stack
```

### 2.2 Modos de Direccionamiento

#### Direccionamiento Directo

```nasm
; Direccionamiento directo
mov eax, 0x12345678    ; Inmediato
mov eax, [0x400000]    ; Directo (desde dirección absoluta)
mov [0x400000], eax    ; Directo (a dirección absoluta)
```

#### Direccionamiento Indirecto

```nasm
; Direccionamiento indirecto
mov eax, [ebx]         ; EAX = contenido de la dirección en EBX
mov [ebx], eax         ; Contenido de dirección EBX = EAX

; Con desplazamiento
mov eax, [ebx + 4]     ; EAX = contenido de (EBX + 4)
mov eax, [ebp - 8]     ; EAX = contenido de (EBP - 8)

; Indexado
mov eax, [ebx + ecx*2] ; EAX = contenido de (EBX + ECX*2)
mov eax, [ebx + ecx*4 + 8] ; EAX = contenido de (EBX + ECX*4 + 8)
```

### 2.3 Instrucciones Básicas

#### Movimiento de Datos

```nasm
; Movimiento básico
mov eax, ebx           ; EAX = EBX
mov [memoria], eax     ; memoria = EAX
mov eax, [memoria]     ; EAX = memoria

; Intercambio
xchg eax, ebx          ; Intercambiar EAX y EBX

; Transferencia condicional
cmov eax, ebx          ; Mover si condición se cumple
```

#### Operaciones Aritméticas

```nasm
; Suma y resta
add eax, ebx           ; EAX = EAX + EBX
sub eax, ebx           ; EAX = EAX - EBX
inc eax                ; EAX = EAX + 1
dec eax                ; EAX = EAX - 1

; Multiplicación y división
mul ebx                ; EDX:EAX = EAX * EBX (unsigned)
imul ebx               ; EDX:EAX = EAX * EBX (signed)
div ebx                ; EAX = EDX:EAX / EBX, EDX = remainder

; Operaciones lógicas
and eax, ebx           ; EAX = EAX AND EBX
or  eax, ebx           ; EAX = EAX OR EBX
xor eax, ebx           ; EAX = EAX XOR EBX
not eax                ; EAX = NOT EAX

; Desplazamientos
shl eax, 2             ; EAX = EAX << 2 (multiplicar por 4)
shr eax, 1             ; EAX = EAX >> 1 (dividir por 2)
```

### 2.4 Control de Flujo

#### Saltos Condicionales

```nasm
; Comparación
cmp eax, ebx           ; Comparar EAX con EBX (afecta flags)

; Saltos condicionales
je  label              ; Jump if Equal (ZF = 1)
jne label              ; Jump if Not Equal (ZF = 0)
jg  label              ; Jump if Greater (signed)
jl  label              ; Jump if Less (signed)
ja  label              ; Jump if Above (unsigned)
jb  label              ; Jump if Below (unsigned)
jz  label              ; Jump if Zero (igual a JE)
jnz label              ; Jump if Not Zero (igual a JNE)

; Ejemplo de loop
mov ecx, 10            ; Contador
loop_start:
    ; Cuerpo del loop
    dec ecx            ; Decrementar contador
    jnz loop_start     ; Continuar si no es cero
```

#### Llamadas a Funciones

```nasm
; Llamada a función
call function_name     ; Guardar EIP y saltar a función
ret                    ; Retornar (restaurar EIP)

; Llamada con parámetros en stack
push param2            ; Último parámetro primero
push param1            ; Primer parámetro último
call function
add esp, 8             ; Limpiar parámetros (2 * 4 bytes)
```

### 2.5 Manejo del Stack

#### Operaciones Básicas del Stack

```nasm
; Push y Pop
push eax               ; Guardar EAX en stack (ESP = ESP - 4)
pop  ebx               ; Restaurar desde stack a EBX (ESP = ESP + 4)

; Múltiples registros
pushad                 ; Guardar todos los registros generales
popad                  ; Restaurar todos los registros generales

; Guardar/restaurar flags
pushfd                 ; Guardar EFLAGS
popfd                  ; Restaurar EFLAGS
```

#### Stack Frames

```nasm
; Prólogo de función estándar
function_start:
    push ebp           ; Guardar frame pointer anterior
    mov  ebp, esp      ; Establecer nuevo frame pointer
    sub  esp, 16       ; Reservar 16 bytes para variables locales
    
    ; Cuerpo de la función
    ; Variables locales en [ebp-4], [ebp-8], etc.
    ; Parámetros en [ebp+8], [ebp+12], etc.
    
    ; Epílogo de función
    mov  esp, ebp      ; Restaurar stack pointer
    pop  ebp           ; Restaurar frame pointer anterior
    ret                ; Retornar
```

---

## 🔄 Parte III: Interacción C y Ensamblador

### 3.1 Funciones en Ensamblador desde C

#### Declaración y Uso

```c
// Declaración en C
extern void asm_function(int param1, int param2);
extern int asm_return_value(void);

// Uso
int main() {
    asm_function(42, 100);
    int result = asm_return_value();
    return 0;
}
```

```nasm
; Implementación en ensamblador (archivo .s)
section .text
global asm_function
global asm_return_value

asm_function:
    push ebp
    mov  ebp, esp
    
    ; Parámetros en [ebp+8] y [ebp+12]
    mov  eax, [ebp+8]    ; param1
    mov  ebx, [ebp+12]   ; param2
    
    ; Hacer algo con los parámetros
    add  eax, ebx
    
    mov  esp, ebp
    pop  ebp
    ret

asm_return_value:
    mov  eax, 42         ; Valor de retorno en EAX
    ret
```

### 3.2 Ensamblador Inline

#### Sintaxis Básica de GCC

```c
// Ensamblador inline básico
void set_interrupt_flag() {
    asm volatile ("sti");  // Habilitar interrupciones
}

void clear_interrupt_flag() {
    asm volatile ("cli");  // Deshabilitar interrupciones
}

// Con parámetros
void outb(uint16_t port, uint8_t value) {
    asm volatile ("outb %0, %1" 
                  : 
                  : "a"(value), "Nd"(port));
}

uint8_t inb(uint16_t port) {
    uint8_t result;
    asm volatile ("inb %1, %0" 
                  : "=a"(result) 
                  : "Nd"(port));
    return result;
}
```

#### Constrains de GCC

```c
// Diferentes tipos de constraints
void ejemplos_constraints() {
    int input = 42;
    int output;
    
    // "a" = registro EAX
    asm ("mov %1, %0" : "=a"(output) : "r"(input));
    
    // "b" = registro EBX, "c" = ECX, "d" = EDX
    asm ("" : : "a"(1), "b"(2), "c"(3), "d"(4));
    
    // "r" = cualquier registro de propósito general
    asm ("mov %1, %0" : "=r"(output) : "r"(input));
    
    // "m" = operando en memoria
    asm ("movl %1, %0" : "=m"(output) : "r"(input));
    
    // "i" = constante inmediata
    asm ("movl %0, %%eax" : : "i"(42));
}
```

### 3.3 Acceso a Hardware desde C

#### Puertos de E/S

```c
// Funciones para acceso a puertos
static inline void outb(uint16_t port, uint8_t value) {
    asm volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void outw(uint16_t port, uint16_t value) {
    asm volatile ("outw %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint16_t inw(uint16_t port) {
    uint16_t ret;
    asm volatile ("inw %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// Uso práctico
void keyboard_init() {
    outb(0x64, 0xAE);  // Habilitar puerto 1
    outb(0x60, 0xF4);  // Habilitar escaneo
}
```

#### Acceso Directo a Memoria

```c
// Mapeo de memoria de video VGA
#define VGA_WIDTH  80
#define VGA_HEIGHT 25
#define VGA_MEMORY 0xB8000

static uint16_t *vga_buffer = (uint16_t *)VGA_MEMORY;

// Colores VGA
typedef enum {
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4,
    VGA_COLOR_MAGENTA = 5,
    VGA_COLOR_BROWN = 6,
    VGA_COLOR_LIGHT_GREY = 7,
    VGA_COLOR_DARK_GREY = 8,
    VGA_COLOR_LIGHT_BLUE = 9,
    VGA_COLOR_LIGHT_GREEN = 10,
    VGA_COLOR_LIGHT_CYAN = 11,
    VGA_COLOR_LIGHT_RED = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_LIGHT_BROWN = 14,
    VGA_COLOR_WHITE = 15,
} vga_color_t;

static inline uint8_t vga_color_entry(vga_color_t fg, vga_color_t bg) {
    return fg | bg << 4;
}

static inline uint16_t vga_char_entry(unsigned char c, uint8_t color) {
    return (uint16_t)c | (uint16_t)color << 8;
}

void vga_putchar_at(char c, uint8_t color, size_t x, size_t y) {
    const size_t index = y * VGA_WIDTH + x;
    vga_buffer[index] = vga_char_entry(c, color);
}
```

---

## 🧪 Ejercicios Prácticos

### Ejercicio 1: Funciones Básicas de String (Nivel: Básico)

**Objetivo**: Implementar funciones básicas de manejo de strings sin usar bibliotecas estándar.

```c
// Completar las siguientes funciones:

// 1. Calcular longitud de string
size_t my_strlen(const char *str) {
    // TODO: Implementar
    return 0;
}

// 2. Copiar string
void my_strcpy(char *dest, const char *src) {
    // TODO: Implementar
}

// 3. Concatenar strings
void my_strcat(char *dest, const char *src) {
    // TODO: Implementar
}

// 4. Comparar strings
int my_strcmp(const char *str1, const char *str2) {
    // TODO: Implementar
    return 0;
}

// Función de prueba
void test_string_functions() {
    char buffer[100];
    my_strcpy(buffer, "Hello");
    my_strcat(buffer, " World");
    
    if (my_strcmp(buffer, "Hello World") == 0) {
        // Éxito
    }
}
```

### Ejercicio 2: Manipulación de Bits (Nivel: Intermedio)

**Objetivo**: Crear funciones para manipular campos de bits en registros de hardware.

```c
// Estructura que simula un registro de control
typedef union {
    uint32_t value;
    struct {
        uint32_t enable    : 1;   // Bit 0
        uint32_t mode      : 2;   // Bits 1-2
        uint32_t priority  : 4;   // Bits 3-6
        uint32_t reserved1 : 9;   // Bits 7-15
        uint32_t address   : 12;  // Bits 16-27
        uint32_t reserved2 : 4;   // Bits 28-31
    } fields;
} control_register_t;

// TODO: Implementar funciones para manipular el registro
void set_enable(control_register_t *reg, bool enable) {
    // TODO: Establecer bit enable
}

void set_mode(control_register_t *reg, uint8_t mode) {
    // TODO: Establecer campo mode (0-3)
}

uint8_t get_priority(control_register_t *reg) {
    // TODO: Obtener campo priority
    return 0;
}

void set_address(control_register_t *reg, uint16_t addr) {
    // TODO: Establecer campo address
}
```

### Ejercicio 3: Ensamblador Básico (Nivel: Intermedio)

**Objetivo**: Escribir funciones en ensamblador para operaciones básicas.

```nasm
; TODO: Implementar las siguientes funciones en ensamblador

section .text
global asm_add
global asm_multiply
global asm_find_max
global asm_copy_memory

; Función: int asm_add(int a, int b)
; Suma dos enteros
asm_add:
    ; TODO: Implementar suma de dos parámetros
    ret

; Función: int asm_multiply(int a, int b)  
; Multiplica dos enteros
asm_multiply:
    ; TODO: Implementar multiplicación
    ret

; Función: int asm_find_max(int *array, int size)
; Encuentra el valor máximo en un array
asm_find_max:
    ; TODO: Implementar búsqueda de máximo
    ret

; Función: void asm_copy_memory(void *dest, void *src, int size)
; Copia memoria byte por byte
asm_copy_memory:
    ; TODO: Implementar copia de memoria
    ret
```

### Ejercicio 4: Integración C y Ensamblador (Nivel: Avanzado)

**Objetivo**: Crear un sistema básico de E/S usando C y ensamblador inline.

```c
// Sistema básico de I/O para comunicación serial

#define SERIAL_PORT_A 0x3F8

// TODO: Implementar usando ensamblador inline
void serial_init() {
    // Configurar puerto serial:
    // 1. Deshabilitar interrupciones
    // 2. Establecer baud rate
    // 3. Configurar formato de datos
    // 4. Habilitar FIFO
}

void serial_putchar(char c) {
    // TODO: Enviar carácter por puerto serial
    // 1. Esperar hasta que el puerto esté listo
    // 2. Escribir carácter al puerto
}

char serial_getchar() {
    // TODO: Recibir carácter del puerto serial
    // 1. Esperar hasta que haya datos
    // 2. Leer carácter del puerto
    return 0;
}

void serial_puts(const char *str) {
    // TODO: Enviar string completo
    while (*str) {
        serial_putchar(*str++);
    }
}

// Sistema de debugging básico
void debug_print(const char *message) {
    serial_puts("[DEBUG] ");
    serial_puts(message);
    serial_putchar('\n');
}
```

---

## 📚 Recursos de Referencia

### Documentación Esencial

1. **Intel x86 Instruction Set Reference**
   - Descripción completa de todas las instrucciones x86
   - Formatos de instrucciones y encodings
   - Flags afectados por cada instrucción

2. **GCC Inline Assembly Documentation**
   - Sintaxis de ensamblador inline
   - Constraints y modificadores
   - Ejemplos prácticos

3. **C Standard (ISO/IEC 9899)**
   - Sección sobre implementaciones freestanding
   - Tipos de datos requeridos
   - Comportamiento definido vs indefinido

### Herramientas de Desarrollo

```bash
# Compilador para verificar código C
gcc -ffreestanding -nostdlib -Wall -Wextra -c file.c

# Ensamblador para verificar código ASM
nasm -f elf32 file.asm -o file.o

# Desensamblador para analizar código
objdump -d file.o

# Verificar símbolos y secciones
objdump -t file.o
nm file.o

# Hexdump para ver contenido binario
hexdump -C file.bin
```

### Comandos Útiles para Debugging

```bash
# GDB con debugging symbols
gcc -g -ffreestanding -nostdlib file.c -o program
gdb program

# Dentro de GDB:
(gdb) disassemble function_name    # Ver ensamblador
(gdb) info registers               # Ver registros
(gdb) x/10x $esp                  # Ver stack
(gdb) stepi                       # Ejecutar una instrucción
```

---

## ✅ Lista de Verificación de Conocimientos

Antes de continuar con el curso principal, asegúrate de poder:

### Conocimientos de C
- [ ] Escribir funciones sin usar bibliotecas estándar
- [ ] Manejar punteros y aritmética de punteros
- [ ] Declarar y usar estructuras y uniones
- [ ] Manipular bits usando operadores bitwise
- [ ] Entender diferencias entre tipos signed/unsigned
- [ ] Trabajar con arrays estáticos y memoria fija

### Conocimientos de Ensamblador
- [ ] Identificar y usar registros x86 de 32 bits
- [ ] Escribir instrucciones básicas (mov, add, sub, etc.)
- [ ] Entender modos de direccionamiento
- [ ] Crear loops y saltos condicionales
- [ ] Manejar el stack (push/pop)
- [ ] Escribir funciones con prólogo/epílogo

### Integración C/ASM
- [ ] Usar ensamblador inline con GCC
- [ ] Llamar funciones de ensamblador desde C
- [ ] Pasar parámetros entre C y ensamblador
- [ ] Acceder a puertos de hardware
- [ ] Manipular memoria directamente

### Conceptos de Sistemas
- [ ] Entender el concepto de freestanding
- [ ] Conocer diferencias entre user/kernel space
- [ ] Comprender mapeo de memoria de hardware
- [ ] Familiaridad con debugging de bajo nivel

---

## 🚀 Siguientes Pasos

Una vez que domines estos conceptos fundamentales, estarás listo para comenzar con:

1. **CAPITULO_01.md** - Arquitectura x86 y Bootloaders
2. **PRE_CURSO_01.md** - Herramientas y Entorno de Desarrollo

**¡Estos conocimientos son la base sólida para convertirte en un desarrollador de sistemas operativos!** 💻⚡

*"El código máquina es el lenguaje nativo del procesador; C es su traductor más fiel."*