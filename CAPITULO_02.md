# Capítulo 2: Gestión de Memoria y Entrada/Salida

## 📚 Introducción

Este capítulo profundiza en los aspectos fundamentales de la gestión de memoria y los mecanismos de entrada/salida en sistemas operativos. Utilizaremos el mini-kernel educativo para explorar cómo el kernel interactúa directamente con el hardware a través de controladores de dispositivos y manejo de memoria física.

## 🎯 Objetivos de Aprendizaje

Al completar este capítulo, el estudiante será capaz de:

1. **Implementar** funciones básicas de gestión de memoria sin biblioteca estándar
2. **Programar** controladores de dispositivos para VGA y teclado PS/2
3. **Utilizar** puertos de E/S para comunicación con hardware
4. **Manejar** interrupciones básicas del sistema
5. **Optimizar** operaciones de memoria para sistemas embebidos

---

## 🏛️ Parte Teórica

### 1.1 Gestión de Memoria en Modo Freestanding

#### ¿Qué es Modo Freestanding?

En desarrollo de sistemas operativos, el **modo freestanding** significa que nuestro código ejecuta sin el soporte de la biblioteca estándar de C. Esto implica:

```c
// ❌ NO disponible en freestanding
#include <stdio.h>    // No hay printf estándar
#include <stdlib.h>   // No hay malloc/free
#include <string.h>   // No hay strcpy, memcpy, etc.

// ✅ SÍ disponible en freestanding
#include <stdint.h>   // Tipos de datos enteros
#include <stdarg.h>   // Argumentos variables
#include <stddef.h>   // NULL, size_t
```

#### Implementación de Funciones Básicas

**1. Manipulación de Memoria:**
```c
// Copia n bytes desde src hacia dest
static inline void *memcpy(void *dest, const void *src, unsigned int n) {
    unsigned char *d = dest;
    const unsigned char *s = src;
    for (unsigned int i = 0; i < n; i++) 
        d[i] = s[i];
    return dest;
}

// Llena n bytes con el valor c
static inline void *memset(void *s, int c, unsigned int n) {
    unsigned char *p = s;
    for (unsigned int i = 0; i < n; i++) 
        p[i] = (unsigned char)c;
    return s;
}

// Compara n bytes entre dos regiones
static inline int memcmp(const void *a, const void *b, unsigned int n) {
    const unsigned char *p = a, *q = b;
    for (unsigned int i = 0; i < n; i++) {
        if (p[i] != q[i]) return p[i] - q[i];
    }
    return 0;
}
```

**2. Manipulación de Cadenas:**
```c
// Calcula longitud de string
static inline unsigned int strlen(const char *s) {
    unsigned int len = 0;
    while (s[len]) len++;
    return len;
}

// Encuentra primer carácter c en string s
static inline char *strchr(const char *s, int c) {
    for (; *s; s++) 
        if (*s == (char)c) return (char *)s;
    return NULL;
}

// Compara dos strings
static inline int strcmp(const char *a, const char *b) {
    while (*a && (*a == *b)) { a++; b++; }
    return (unsigned char)*a - (unsigned char)*b;
}
```

#### Optimizaciones para Sistemas Embebidos

**Técnicas de Optimización:**
1. **Inline functions**: Evita overhead de llamadas
2. **Loop unrolling**: Desenrolla bucles pequeños
3. **Alignment**: Alinea datos a límites de cache
4. **Compiler hints**: Usa `likely/unlikely` para branch prediction

```c
// Optimización con loop unrolling
static inline void *fast_memcpy(void *dest, const void *src, unsigned int n) {
    unsigned char *d = dest;
    const unsigned char *s = src;
    
    // Copiar de 4 en 4 cuando sea posible
    while (n >= 4) {
        *((uint32_t*)d) = *((uint32_t*)s);
        d += 4; s += 4; n -= 4;
    }
    
    // Copiar bytes restantes
    while (n--) *d++ = *s++;
    return dest;
}
```

### 1.2 Entrada/Salida por Puertos

#### Arquitectura de E/S en x86

En x86, los dispositivos se comunican a través de **puertos de E/S**, un espacio de direcciones separado de la memoria principal:

```
┌─────────────────┐    ┌─────────────────┐
│     Memoria     │    │   Puertos E/S   │
│   0x00000000    │    │      0x0000     │
│       ...       │    │       ...       │
│   0xFFFFFFFF    │    │      0xFFFF     │
└─────────────────┘    └─────────────────┘
         ▲                       ▲
    mov/load/store           in/out
```

**Instrucciones de E/S:**
- **IN**: Lee desde puerto de E/S
- **OUT**: Escribe a puerto de E/S
- **INS/OUTS**: Transferencia de bloques

#### Implementación de Funciones de Puerto

```c
// Envía un byte a un puerto específico
static inline void outb(uint16_t port, uint8_t val) {
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

// Lee un byte de un puerto específico
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// Variantes para 16-bit y 32-bit
static inline void outw(uint16_t port, uint16_t val) {
    asm volatile ("outw %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint16_t inw(uint16_t port) {
    uint16_t ret;
    asm volatile ("inw %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}
```

#### Inline Assembly en GCC

**Sintaxis de Inline Assembly:**
```c
asm volatile ("instrucción" : outputs : inputs : clobbered);
```

**Constrains comunes:**
- `"a"`: Registro EAX/AX/AL
- `"b"`: Registro EBX/BX/BL
- `"m"`: Operando en memoria
- `"r"`: Cualquier registro
- `"Nd"`: Constante 0-255 para puertos

### 1.3 Controlador de Pantalla VGA

#### Modo Texto VGA

El controlador VGA en modo texto proporciona:
- **Resolución**: 80x25 caracteres
- **Memoria**: 0xB8000 - 0xB8FA0 (4000 bytes)
- **Formato**: 2 bytes por carácter (carácter + atributo)

```
Memoria VGA (0xB8000):
┌────────────────────────────────────────┐
│ Char │ Attr │ Char │ Attr │ ... │ 80  │  ← Línea 1
├────────────────────────────────────────┤
│ Char │ Attr │ Char │ Attr │ ... │ 160 │  ← Línea 2
├────────────────────────────────────────┤
│ ...  │ ...  │ ...  │ ...  │ ... │ ... │
└────────────────────────────────────────┘
                                     ↑
                               Línea 25 (posición 4000)
```

**Byte de Atributo:**
```
Bit:  7   6 5 4   3 2 1 0
     ┌─┬─────────┬─────────┐
     │B│  BG     │   FG    │
     └─┴─────────┴─────────┘
      │     │         │
   Blink  Background  Foreground
```

#### Implementación del Driver VGA

```c
#define VGA_MEMORY 0xB8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

// Colores VGA estándar
enum vga_color {
    VGA_COLOR_BLACK = 0,      VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,      VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4,        VGA_COLOR_MAGENTA = 5,
    VGA_COLOR_BROWN = 6,      VGA_COLOR_LIGHT_GREY = 7,
    VGA_COLOR_DARK_GREY = 8,  VGA_COLOR_LIGHT_BLUE = 9,
    VGA_COLOR_LIGHT_GREEN = 10, VGA_COLOR_LIGHT_CYAN = 11,
    VGA_COLOR_LIGHT_RED = 12,   VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_LIGHT_BROWN = 14, VGA_COLOR_WHITE = 15,
};

// Estado del terminal
static uint16_t *vga_buffer = (uint16_t*)VGA_MEMORY;
static int terminal_row = 0;
static int terminal_column = 0;
static uint8_t terminal_color = VGA_COLOR_LIGHT_GREY | (VGA_COLOR_BLACK << 4);

// Crear entrada VGA (carácter + color)
static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
    return (uint16_t)uc | (uint16_t)color << 8;
}

// Limpiar pantalla
void terminal_clear(void) {
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            vga_buffer[index] = vga_entry(' ', terminal_color);
        }
    }
    terminal_row = 0;
    terminal_column = 0;
}
```

#### Control de Cursor Hardware

```c
// Puertos del controlador VGA para cursor
#define VGA_CTRL_REGISTER 0x3D4
#define VGA_DATA_REGISTER 0x3D5

// Actualizar posición del cursor hardware
void update_cursor(int x, int y) {
    uint16_t pos = y * VGA_WIDTH + x;
    
    // Enviar byte bajo de la posición
    outb(VGA_CTRL_REGISTER, 0x0F);
    outb(VGA_DATA_REGISTER, (uint8_t)(pos & 0xFF));
    
    // Enviar byte alto de la posición
    outb(VGA_CTRL_REGISTER, 0x0E);
    outb(VGA_DATA_REGISTER, (uint8_t)((pos >> 8) & 0xFF));
}

// Implementar scroll cuando se llena la pantalla
void terminal_scroll(void) {
    // Mover todas las líneas una posición arriba
    for (int y = 1; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            vga_buffer[(y-1) * VGA_WIDTH + x] = vga_buffer[y * VGA_WIDTH + x];
        }
    }
    
    // Limpiar última línea
    for (int x = 0; x < VGA_WIDTH; x++) {
        vga_buffer[(VGA_HEIGHT-1) * VGA_WIDTH + x] = vga_entry(' ', terminal_color);
    }
    
    terminal_row = VGA_HEIGHT - 1;
}
```

### 1.4 Controlador de Teclado PS/2

#### Protocolo PS/2

El teclado PS/2 comunica através de dos puertos:
- **Puerto 0x60**: Datos (scancodes)
- **Puerto 0x64**: Estado y comandos

```
Secuencia de comunicación PS/2:
┌─────────┐    ┌─────────┐    ┌─────────┐
│ Tecla   │───▶│ Scancode│───▶│ ASCII   │
│ Presion │    │ Hardware│    │Software │
└─────────┘    └─────────┘    └─────────┘
```

#### Scancodes y Mapeo

**Make vs Break Codes:**
- **Make code**: Tecla presionada
- **Break code**: Tecla liberada (Make + 0x80)

```c
// Tabla de mapeo de scancodes a ASCII
static const char scancode_map[128] = {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' ', 0,
    // ... más entradas
};
```

#### Implementación del Driver de Teclado

```c
#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64

// Estado de teclas modificadoras
static int shift_pressed = 0;
static int ctrl_pressed = 0;
static int alt_pressed = 0;

// Obtener carácter del teclado (polling)
static unsigned char keyboard_getchar(void) {
    while (1) {
        // Esperar hasta que haya datos disponibles
        if (!(inb(KEYBOARD_STATUS_PORT) & 0x01)) {
            continue;
        }
        
        unsigned char scancode = inb(KEYBOARD_DATA_PORT);
        
        // Manejar break codes (tecla liberada)
        if (scancode & 0x80) {
            scancode &= 0x7F;  // Quitar bit de break
            
            // Manejar liberación de teclas modificadoras
            switch (scancode) {
                case 0x2A:  // Left Shift
                case 0x36:  // Right Shift
                    shift_pressed = 0;
                    break;
                case 0x1D:  // Ctrl
                    ctrl_pressed = 0;
                    break;
                case 0x38:  // Alt
                    alt_pressed = 0;
                    break;
            }
            continue;
        }
        
        // Manejar make codes (tecla presionada)
        switch (scancode) {
            case 0x2A:  // Left Shift
            case 0x36:  // Right Shift
                shift_pressed = 1;
                continue;
            case 0x1D:  // Ctrl
                ctrl_pressed = 1;
                continue;
            case 0x38:  // Alt
                alt_pressed = 1;
                continue;
        }
        
        // Convertir scancode a carácter
        char c = scancode_map[scancode];
        if (c) {
            // Aplicar modificadores
            if (shift_pressed && c >= 'a' && c <= 'z') {
                c = c - 'a' + 'A';
            }
            return c;
        }
    }
}
```

---

## 🔧 Parte Práctica

### 2.1 Análisis del Código de Memoria

#### Funciones Implementadas en kernel.c

Nuestro mini-kernel implementa las siguientes funciones esenciales:

```c
// kernel.c: líneas 14-44
static inline void *memcpy(void *dest, const void *src, unsigned int n) {
    unsigned char *d = dest;
    const unsigned char *s = src;
    for (unsigned int i = 0; i < n; i++) d[i] = s[i];
    return dest;
}

static inline void *memset(void *s, int c, unsigned int n) {
    unsigned char *p = s;
    for (unsigned int i = 0; i < n; i++) p[i] = (unsigned char)c;
    return s;
}

static inline int memcmp(const void *a, const void *b, unsigned int n) {
    const unsigned char *p = a, *q = b;
    for (unsigned int i = 0; i < n; i++) {
        if (p[i] != q[i]) return p[i] - q[i];
    }
    return 0;
}
```

**Ventajas de inline functions:**
- Eliminan overhead de llamadas de función
- Permiten optimizaciones del compilador
- Mantienen type safety de C

### 2.2 Sistema de E/S por Puertos

#### Implementación en kernel.c

```c
// kernel.c: líneas 52-59
static inline void outb(uint16_t port, uint8_t val) {
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}
```

**Análisis del inline assembly:**
- `volatile`: Evita optimizaciones que podrían eliminar la instrucción
- `"a"(val)`: Pone `val` en registro AL/AX/EAX
- `"Nd"(port)`: Puerto como constante inmediata 0-255
- `"=a"(ret)`: Resultado en AL/AX/EAX

### 2.3 Driver VGA Completo

#### Funciones de Terminal

```c
// Variables globales del terminal
static uint16_t *vga_buffer = (uint16_t*)0xB8000;
static int terminal_row = 0;
static int terminal_column = 0;
static uint8_t terminal_color = 7;  // Gris claro sobre negro

// Función putchar básica
static void putchar(char c) {
    if (c == '\n') {
        terminal_column = 0;
        if (++terminal_row == 25) {
            terminal_scroll();
        }
    } else if (c == '\b') {
        if (terminal_column > 0) {
            terminal_column--;
            vga_buffer[terminal_row * 80 + terminal_column] = 
                vga_entry(' ', terminal_color);
        }
    } else {
        vga_buffer[terminal_row * 80 + terminal_column] = 
            vga_entry(c, terminal_color);
        
        if (++terminal_column == 80) {
            terminal_column = 0;
            if (++terminal_row == 25) {
                terminal_scroll();
            }
        }
    }
    
    update_cursor(terminal_column, terminal_row);
}
```

#### Sistema printf Básico

```c
// Implementación de printf simplificado
void printf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    
    while (*fmt) {
        if (*fmt == '%' && *(fmt + 1)) {
            fmt++;
            switch (*fmt) {
                case 's': {
                    char *s = va_arg(args, char*);
                    prints(s);
                    break;
                }
                case 'c': {
                    char c = va_arg(args, int);
                    putchar(c);
                    break;
                }
                case 'd': {
                    int n = va_arg(args, int);
                    print_int(n);
                    break;
                }
                case 'x': {
                    unsigned int n = va_arg(args, unsigned int);
                    print_hex(n, 8);
                    break;
                }
                default:
                    putchar(*fmt);
                    break;
            }
        } else {
            putchar(*fmt);
        }
        fmt++;
    }
    
    va_end(args);
}
```

### 2.4 Driver de Teclado Avanzado

#### Manejo de Teclas Especiales

```c
// Códigos especiales para teclas de navegación
#define KEY_UP    0x80
#define KEY_DOWN  0x81  
#define KEY_LEFT  0x82
#define KEY_RIGHT 0x83
#define KEY_DELETE 0x84

// Función mejorada de teclado con teclas especiales
static unsigned char keyboard_getchar(void) {
    static int ctrl_pressed = 0;
    static int shift_pressed = 0;
    static int extended = 0;
    
    while (1) {
        if (!(inb(0x64) & 0x01)) continue;
        
        unsigned char code = inb(0x60);
        
        // Manejar secuencias extendidas (0xE0)
        if (code == 0xE0) {
            extended = 1;
            continue;
        }
        
        // Manejar break codes
        if (code & 0x80) {
            code &= 0x7F;
            
            if (extended) {
                extended = 0;
                continue;
            }
            
            // Liberación de modificadores
            switch (code) {
                case 0x2A: case 0x36: shift_pressed = 0; break;
                case 0x1D: ctrl_pressed = 0; break;
            }
            continue;
        }
        
        // Manejar teclas extendidas
        if (extended) {
            extended = 0;
            switch (code) {
                case 0x48: return KEY_UP;
                case 0x50: return KEY_DOWN;
                case 0x4B: return KEY_LEFT;
                case 0x4D: return KEY_RIGHT;
                case 0x53: return KEY_DELETE;
            }
            continue;
        }
        
        // Make codes normales
        switch (code) {
            case 0x2A: case 0x36: shift_pressed = 1; continue;
            case 0x1D: ctrl_pressed = 1; continue;
        }
        
        char c = scancode_map[code];
        if (c) {
            // Aplicar Ctrl
            if (ctrl_pressed && c >= 'a' && c <= 'z') {
                return c - 'a' + 1;  // Ctrl+A = 0x01, etc.
            }
            
            // Aplicar Shift
            if (shift_pressed) {
                if (c >= 'a' && c <= 'z') {
                    return c - 'a' + 'A';
                }
                // Símbolos con Shift
                switch (c) {
                    case '1': return '!';
                    case '2': return '@';
                    case '3': return '#';
                    // ... más símbolos
                    case '\\': return '|';  // Para pipes
                }
            }
            
            return c;
        }
    }
}
```

---

## 📝 Ejercicios Prácticos

### Ejercicio 1: Optimización de memcpy
**Dificultad**: Intermedia

**Objetivo**: Implementar una versión optimizada de memcpy que use transferencias de 32-bit cuando sea posible.

**Instrucciones**:
1. Implementa `fast_memcpy()` que copie por uint32_t cuando los punteros estén alineados:

```c
void *fast_memcpy(void *dest, const void *src, size_t n) {
    uint8_t *d = (uint8_t*)dest;
    const uint8_t *s = (const uint8_t*)src;
    
    // TODO: Verificar alineación de 4 bytes
    // TODO: Copiar por uint32_t cuando sea posible
    // TODO: Copiar bytes restantes
    
    return dest;
}
```

2. Crea función de benchmark para comparar rendimiento:
```c
void benchmark_memcpy(void) {
    uint8_t src[1024], dest[1024];
    
    // Llenar src con datos de prueba
    for (int i = 0; i < 1024; i++) src[i] = i & 0xFF;
    
    // TODO: Medir tiempo de ejecución de ambas versiones
    // TODO: Mostrar resultados
}
```

**Criterios de evaluación**:
- Correctitud en casos alineados y no alineados
- Mejora de rendimiento medible
- Manejo correcto de casos límite

---

### Ejercicio 2: Colores y Estilos VGA
**Dificultad**: Básica

**Objetivo**: Extender el driver VGA para soportar colores y estilos de texto.

**Instrucciones**:
1. Implementa funciones para cambiar colores:
```c
void set_text_color(enum vga_color fg, enum vga_color bg) {
    terminal_color = fg | (bg << 4);
}

void print_colored(const char *text, enum vga_color fg, enum vga_color bg) {
    // TODO: Guardar color actual
    // TODO: Cambiar a nuevo color
    // TODO: Imprimir texto
    // TODO: Restaurar color original
}
```

2. Crea comando `color` en el shell:
```c
// En la función del shell, agregar:
else if (!strcmp(cmd, "color") && arg) {
    // TODO: Parsear argumentos de color
    // TODO: Llamar a set_text_color()
}
```

3. Implementa efectos especiales:
```c
void print_rainbow(const char *text) {
    // TODO: Cada carácter en diferente color
}

void print_blink(const char *text) {
    // TODO: Usar bit de parpadeo (bit 7 del atributo)
}
```

**Resultado esperado**: El shell debe soportar texto en colores y efectos visuales.

---

### Ejercicio 3: Buffer de Teclado con Interrupciones
**Dificultad**: Avanzada

**Objetivo**: Implementar un buffer circular para el teclado que maneje entrada de forma asíncrona.

**Instrucciones**:
1. Define estructura del buffer circular:
```c
#define KEYBOARD_BUFFER_SIZE 256

struct keyboard_buffer {
    unsigned char data[KEYBOARD_BUFFER_SIZE];
    int head;
    int tail;
    int count;
};

static struct keyboard_buffer kb_buffer = {0};
```

2. Implementa funciones del buffer:
```c
int kb_buffer_put(unsigned char c) {
    // TODO: Agregar carácter al buffer si hay espacio
    // TODO: Actualizar head y count
    // TODO: Retornar éxito/error
}

int kb_buffer_get(unsigned char *c) {
    // TODO: Extraer carácter del buffer si hay datos
    // TODO: Actualizar tail y count
    // TODO: Retornar éxito/error
}

int kb_buffer_empty(void) {
    return kb_buffer.count == 0;
}
```

3. Modifica el handler de teclado:
```c
// En lugar de retornar directamente, usar buffer
static unsigned char keyboard_getchar_buffered(void) {
    unsigned char c;
    while (kb_buffer_get(&c) != 0) {
        // Esperar hasta que haya datos
        // En una implementación real, esto sería una interrupción
    }
    return c;
}
```

**Criterios de evaluación**:
- Buffer circular funciona correctamente
- No se pierden caracteres
- Manejo adecuado de buffer lleno/vacío

---

### Ejercicio 4: Monitor de Sistema en Tiempo Real
**Dificultad**: Muy Avanzada

**Objetivo**: Crear un monitor que muestre información del sistema en tiempo real.

**Instrucciones**:
1. Implementa recolección de estadísticas:
```c
struct system_stats {
    uint32_t total_memory;
    uint32_t used_memory;
    uint32_t free_memory;
    uint32_t uptime_seconds;
    uint32_t keyboard_events;
    uint32_t screen_updates;
};

void update_system_stats(struct system_stats *stats) {
    // TODO: Calcular uso de memoria
    // TODO: Obtener tiempo de funcionamiento
    // TODO: Contar eventos de dispositivos
}
```

2. Crea interfaz visual:
```c
void draw_system_monitor(struct system_stats *stats) {
    // TODO: Limpiar área de la pantalla
    // TODO: Dibujar barras de progreso para memoria
    // TODO: Mostrar estadísticas numéricas
    // TODO: Actualizar cada segundo
}

void draw_progress_bar(int x, int y, int width, int percent, 
                      enum vga_color color) {
    // TODO: Dibujar barra de progreso ASCII
    // [████████░░] 80%
}
```

3. Implementa comando `top`:
```c
else if (!strcmp(cmd, "top")) {
    printf("Monitor del sistema (presiona ESC para salir)\n");
    
    while (1) {
        struct system_stats stats;
        update_system_stats(&stats);
        draw_system_monitor(&stats);
        
        // TODO: Verificar si usuario presionó ESC
        // TODO: Esperar ~1 segundo antes de actualizar
    }
}
```

**Características adicionales**:
- Gráficos ASCII para barras de progreso
- Actualización en tiempo real sin parpadeo
- Estadísticas precisas de uso de recursos
- Interfaz intuitiva de usuario

---

## 📋 Resumen del Capítulo

### Conceptos Clave Aprendidos

1. **Programación Freestanding**:
   - Implementación manual de funciones básicas de C
   - Optimizaciones específicas para sistemas embebidos
   - Manejo directo de memoria sin biblioteca estándar

2. **Entrada/Salida por Puertos**:
   - Arquitectura de E/S separada en x86
   - Uso de instrucciones IN/OUT para comunicación con hardware
   - Inline assembly para acceso directo al hardware

3. **Driver VGA**:
   - Modo texto 80x25 con memoria mapeada
   - Control de cursor hardware a través de puertos
   - Implementación de scroll y efectos de color

4. **Driver de Teclado PS/2**:
   - Protocolo de scancodes make/break
   - Manejo de teclas modificadoras (Shift, Ctrl, Alt)
   - Mapeo de scancodes a caracteres ASCII

### Habilidades Técnicas Desarrolladas

- ✅ **Implementación** de funciones básicas sin biblioteca estándar
- ✅ **Programación** de controladores de dispositivos de bajo nivel  
- ✅ **Uso** de inline assembly para acceso directo a hardware
- ✅ **Optimización** de código para sistemas con recursos limitados
- ✅ **Debugging** de problemas de hardware y timing

### Conexión con el Código del Mini-Kernel

En nuestro mini-kernel educativo, estos conceptos se implementan directamente:

- **Líneas 14-55**: Funciones básicas de memoria y cadenas
- **Líneas 52-59**: Funciones de E/S por puertos
- **Líneas 360-420**: Driver VGA completo con scroll
- **Líneas 206-350**: Driver de teclado con teclas especiales
- **Sistema de printf**: Implementación completa para debugging

### Preparación para el Siguiente Capítulo

El **Capítulo 3** construirá sobre estos fundamentos para explorar:
- Sistemas de archivos y almacenamiento persistente
- Implementación de FAT16 en RAM
- Operaciones CRUD sobre archivos
- Metadata y organización de datos en disco

Los ejercicios de este capítulo han establecido las bases de comunicación con hardware que serán esenciales para implementar drivers de almacenamiento y sistemas de archivos más complejos.

---

**📚 Lecturas Complementarias:**
- "Computer Organization and Design" por Patterson & Hennessy
- Intel x86 I/O Programming Manual
- OSDev Wiki: "VGA Hardware" y "PS/2 Keyboard"
- "Systems Programming" por Dhananjay Rekhi