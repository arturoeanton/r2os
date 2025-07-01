# Capítulo 1: Arquitectura x86 y Bootloaders

## 📚 Introducción

Este capítulo introduce los conceptos fundamentales de la arquitectura x86 y el proceso de arranque de un sistema operativo. Utilizaremos el mini-kernel educativo como caso de estudio para comprender cómo el hardware y el software interactúan desde el momento en que se enciende la computadora.

## 🎯 Objetivos de Aprendizaje

Al completar este capítulo, el estudiante será capaz de:

1. **Explicar** la arquitectura básica del procesador x86
2. **Describir** el proceso completo de boot desde BIOS hasta kernel
3. **Analizar** el código del bootloader en ensamblador
4. **Implementar** modificaciones básicas al proceso de arranque
5. **Configurar** el entorno de cross-compilation para desarrollo de OS

---

## 🏛️ Parte Teórica

### 1.1 Arquitectura del Procesador x86

#### Historia y Evolución

La arquitectura x86 fue introducida por Intel en 1978 con el procesador 8086. Esta arquitectura ha evolucionado manteniendo compatibilidad hacia atrás, lo que la convierte en una excelente plataforma educativa.

**Generaciones importantes:**
- **8086 (1978)**: 16-bit, segmentación básica
- **80386 (1985)**: Modo protegido de 32-bit
- **Pentium (1993)**: Pipeline superescalar
- **x86-64 (2003)**: Extensión a 64-bit

#### Modos de Operación

El procesador x86 opera en diferentes modos que determinan sus capacidades:

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Modo Real     │───▶│ Modo Protegido  │───▶│   Modo Long     │
│    16-bit       │    │     32-bit      │    │     64-bit      │
│  Segmentación   │    │  Paginación +   │    │   64-bit +      │
│     Básica      │    │  Segmentación   │    │  Paginación     │
└─────────────────┘    └─────────────────┘    └─────────────────┘
        ▲                       ▲                       ▲
     Boot time            Kernel moderno         Sistemas actuales
```

**Modo Real (16-bit)**
- Compatible con 8086 original
- Acceso directo a hardware
- Memoria limitada a 1MB
- Sin protección de memoria

**Modo Protegido (32-bit)**
- Espacio de direcciones de 4GB
- Protección entre procesos
- Memoria virtual con paginación
- Anillos de privilegio (0-3)

#### Registros del Procesador

**Registros de Propósito General (32-bit):**
```
┌─────────────────────────────────┐
│              EAX                │  ← Acumulador
├─────────────┬───────────────────┤
│     AX      │                   │
├──────┬──────┤                   │
│  AH  │  AL  │                   │
└──────┴──────┴───────────────────┘

EBX - Base (índice)
ECX - Contador (loops)
EDX - Datos/E-S
ESI - Índice fuente
EDI - Índice destino
EBP - Puntero base del stack
ESP - Puntero del stack
```

**Registros de Segmento:**
- **CS**: Code Segment (código)
- **DS**: Data Segment (datos)
- **ES**: Extra Segment (extra)
- **SS**: Stack Segment (pila)
- **FS/GS**: Segmentos adicionales

**Registros de Control:**
- **EIP**: Instruction Pointer (contador de programa)
- **EFLAGS**: Flags de estado
- **CR0-CR4**: Registros de control del sistema

#### Segmentación vs Paginación

**Segmentación (Modo Real y Protegido):**
```
Dirección Lógica = Segmento:Offset
Dirección Física = (Segmento × 16) + Offset  [Modo Real]
Dirección Lineal = Base_Segmento + Offset    [Modo Protegido]
```

**Paginación (Solo Modo Protegido):**
```
Dirección Virtual → [MMU] → Dirección Física
     32-bit              →    32-bit

Page Directory → Page Table → Page Frame
     10-bit        10-bit       12-bit
```

### 1.2 Proceso de Boot

#### Secuencia de Arranque Completa

```
1. Power-On ──▶ 2. BIOS/UEFI ──▶ 3. Bootloader ──▶ 4. Kernel ──▶ 5. Init/Shell
    (POST)        (Hardware)       (Stage 1&2)      (OS Core)    (Userspace)
```

#### BIOS (Basic Input/Output System)

**Funciones principales:**
1. **POST** (Power-On Self Test): Verificación de hardware
2. **Detección de dispositivos**: RAM, CPU, periféricos
3. **Carga del bootloader**: Desde MBR (Master Boot Record)
4. **Servicios básicos**: Interrupciones para teclado, pantalla, disco

**Limitaciones del BIOS:**
- Modo real (16-bit)
- Memoria limitada (640KB convencional)
- Acceso a disco por CHS (Cylinder-Head-Sector)

#### Bootloader Multi-etapa

**Stage 1: MBR (Master Boot Record)**
- Código: 446 bytes máximo
- Tabla de particiones: 64 bytes
- Signature: 2 bytes (0x55AA)
- **Función**: Cargar Stage 2

**Stage 2: Bootloader Principal**
- Más espacio para funcionalidad
- Puede acceder al sistema de archivos
- Carga el kernel en memoria
- Transfiere control al kernel

### 1.3 Multiboot Specification

#### ¿Qué es Multiboot?

Multiboot es una especificación que define una interfaz estándar entre bootloaders y kernels. Permite que diferentes bootloaders (GRUB, SYSLINUX) puedan cargar cualquier kernel compatible.

**Ventajas:**
- Estandarización del proceso de boot
- Simplificación del desarrollo de kernels
- Información estructurada del sistema

#### Header Multiboot

```assembly
.section .multiboot
.align 4
.long 0x1BADB002          # Magic number
.long 0x00000003          # Flags
.long -(0x1BADB002 + 0x00000003)  # Checksum
```

**Campos obligatorios:**
- **Magic**: 0x1BADB002 (identificación)
- **Flags**: Características requeridas
- **Checksum**: Validación de integridad

#### Información del Sistema

El bootloader pasa información al kernel:
```c
struct multiboot_info {
    uint32_t flags;           // Campos válidos
    uint32_t mem_lower;       // Memoria baja (KB)
    uint32_t mem_upper;       // Memoria alta (KB)
    uint32_t boot_device;     // Dispositivo de boot
    uint32_t cmdline;         // Línea de comandos
    uint32_t mods_count;      // Número de módulos
    uint32_t mods_addr;       // Dirección de módulos
    // ... más campos
};
```

---

## 🔧 Parte Práctica

### 2.1 Análisis del Código del Bootloader

#### Archivo boot.s - Estructura General

```assembly
# boot.s - Bootloader del mini-kernel educativo

.section .multiboot
    # Header Multiboot para GRUB
    .align 4
    .long 0x1BADB002                    # Magic number
    .long 0x00000003                    # Flags (align modules, memory info)
    .long -(0x1BADB002 + 0x00000003)    # Checksum

.section .text
.global _start
_start:
    # Configurar stack
    mov $stack_top, %esp
    
    # Llamar al kernel principal
    call kernel_main
    
    # Loop infinito si el kernel retorna
halt_loop:
    hlt
    jmp halt_loop

.section .bss
.align 16
stack_bottom:
.skip 16384  # 16KB de stack
stack_top:
```

#### Análisis Línea por Línea

**1. Header Multiboot:**
```assembly
.section .multiboot
.align 4
.long 0x1BADB002
```
- Crea sección especial para el header
- Alineación a 4 bytes (requerido)
- Magic number para identificación

**2. Configuración del Stack:**
```assembly
mov $stack_top, %esp
```
- ESP (Stack Pointer) apunta al tope del stack
- Stack crece hacia direcciones menores
- 16KB de espacio reservado

**3. Llamada al Kernel:**
```assembly
call kernel_main
```
- Transfiere control al código C
- Push de dirección de retorno en stack
- Salto a la función principal

### 2.2 Configuración del Entorno

#### Cross-Compilation Toolchain

**¿Por qué Cross-Compilation?**
```
Host System (Linux/Mac/Windows)     Target System (Bare Metal x86)
         ↓                                    ↑
   Native Compiler                     Cross Compiler
  (gcc para host)                    (i686-elf-gcc)
```

**Instalación del Toolchain:**
```bash
# 1. Descargar fuentes de GCC y Binutils
wget http://ftp.gnu.org/gnu/binutils/binutils-2.35.tar.gz
wget http://ftp.gnu.org/gnu/gcc/gcc-10.2.0/gcc-10.2.0.tar.gz

# 2. Configurar variables
export PREFIX="/usr/local/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"

# 3. Compilar Binutils
tar -xf binutils-2.35.tar.gz
cd binutils-2.35
./configure --target=$TARGET --prefix="$PREFIX" --disable-nls
make && make install

# 4. Compilar GCC
tar -xf gcc-10.2.0.tar.gz
cd gcc-10.2.0
./configure --target=$TARGET --prefix="$PREFIX" --disable-nls \
            --enable-languages=c --without-headers
make all-gcc && make install-gcc
```

#### Makefile del Proyecto

```makefile
# Makefile para mini-kernel educativo

# Configuración del cross-compiler
CC = i686-elf-gcc
AS = i686-elf-as
LD = i686-elf-ld
OBJCOPY = i686-elf-objcopy

# Flags de compilación
CFLAGS = -std=gnu99 -ffreestanding -nostdlib -fno-builtin -O2 -Wall -Wextra -g
LDFLAGS = -T linker.ld -nostdlib

# Objetivos
OBJECTS = boot.o kernel.o

all: myos.bin

# Ensamblar bootloader
boot.o: boot.s
	$(AS) -g -o $@ $<

# Compilar kernel
kernel.o: kernel.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Enlazar kernel completo
myos.elf: $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $^

# Crear imagen binaria
myos.bin: myos.elf
	$(OBJCOPY) -O binary $< $@

# Ejecutar en QEMU
run: myos.bin
	qemu-system-i386 -kernel myos.elf -m 32 -nographic

# Limpiar archivos generados
clean:
	rm -f *.o *.elf *.bin

.PHONY: all run clean
```

### 2.3 Script del Linker

#### linker.ld - Organización de Memoria

```ld
/* linker.ld - Script del linker para el kernel */

ENTRY(_start)

SECTIONS
{
    /* Kernel cargado en 1MB (estándar Multiboot) */
    . = 0x00100000;

    /* Sección de texto (código) */
    .text ALIGN(4K) : {
        *(.multiboot)   /* Header Multiboot al inicio */
        *(.text)        /* Código del programa */
    }

    /* Datos de solo lectura */
    .rodata ALIGN(4K) : {
        *(.rodata)
    }

    /* Datos inicializados */
    .data ALIGN(4K) : {
        *(.data)
    }

    /* Datos no inicializados */
    .bss ALIGN(4K) : {
        *(COMMON)
        *(.bss)
    }
}
```

**Explicación de las secciones:**
- **.text**: Código ejecutable
- **.rodata**: Datos de solo lectura (strings, constantes)
- **.data**: Variables globales inicializadas
- **.bss**: Variables globales no inicializadas

### 2.4 Debugging y Herramientas

#### QEMU para Emulación

**Opciones útiles de QEMU:**
```bash
# Ejecución básica
qemu-system-i386 -kernel myos.elf

# Con más memoria
qemu-system-i386 -kernel myos.elf -m 64

# Sin interfaz gráfica (solo terminal)
qemu-system-i386 -kernel myos.elf -nographic

# Con debugging (GDB)
qemu-system-i386 -kernel myos.elf -s -S
```

#### Debugging con GDB

```bash
# Terminal 1: Ejecutar QEMU con debugging
qemu-system-i386 -kernel myos.elf -s -S

# Terminal 2: Conectar GDB
gdb myos.elf
(gdb) target remote localhost:1234
(gdb) break kernel_main
(gdb) continue
(gdb) step
(gdb) info registers
(gdb) x/10i $eip
```

---

## 📝 Ejercicios Prácticos

### Ejercicio 1: Modificar el Mensaje de Boot
**Dificultad**: Básica

**Objetivo**: Personalizar el mensaje que muestra el kernel al arrancar.

**Instrucciones**:
1. Localiza la función `kernel_main()` en `kernel.c`
2. Encuentra la línea que imprime el mensaje de bienvenida
3. Modifica el mensaje para incluir tu nombre y la fecha
4. Compila y ejecuta el kernel modificado

**Código esperado**:
```c
printf("=== MI KERNEL PERSONALIZADO ===\n");
printf("Desarrollado por: [Tu Nombre]\n");
printf("Fecha: [Fecha actual]\n");
printf("Mini-Kernel Educativo iniciado!\n");
```

**Entregable**: Captura de pantalla mostrando el mensaje personalizado.

---

### Ejercicio 2: Análisis del Stack
**Dificultad**: Intermedia

**Objetivo**: Comprender cómo se utiliza el stack durante el boot.

**Instrucciones**:
1. Agrega función para mostrar información del stack:
```c
void show_stack_info(void) {
    uint32_t *stack_ptr;
    asm volatile("mov %%esp, %0" : "=r" (stack_ptr));
    
    printf("Stack pointer: 0x%08x\n", (uint32_t)stack_ptr);
    printf("Stack top: 0x%08x\n", 0x00104000);  // Del linker script
    printf("Stack usage: %d bytes\n", 
           0x00104000 - (uint32_t)stack_ptr);
}
```

2. Llama a esta función desde `kernel_main()`
3. Observa cómo cambia el uso del stack durante la ejecución

**Preguntas a responder**:
- ¿Cuál es el tamaño total del stack?
- ¿Cuántos bytes se usan al inicio?
- ¿Por qué el stack crece hacia direcciones menores?

---

### Ejercicio 3: Header Multiboot Personalizado
**Dificultad**: Avanzada

**Objetivo**: Modificar el header Multiboot para solicitar información adicional.

**Instrucciones**:
1. Modifica `boot.s` para solicitar información de memoria:
```assembly
.section .multiboot
.align 4
.long 0x1BADB002                           # Magic
.long 0x00000003                           # Flags: memoria + alineación
.long -(0x1BADB002 + 0x00000003)          # Checksum
```

2. En `kernel.c`, agrega función para leer información Multiboot:
```c
void show_multiboot_info(uint32_t magic, uint32_t info_addr) {
    if (magic != 0x2BADB002) {
        printf("Error: Magic number incorrecto\n");
        return;
    }
    
    struct multiboot_info *mbi = (struct multiboot_info *)info_addr;
    
    if (mbi->flags & 0x01) {
        printf("Memoria baja: %d KB\n", mbi->mem_lower);
        printf("Memoria alta: %d KB\n", mbi->mem_upper);
    }
}
```

3. Modifica `kernel_main()` para recibir los parámetros Multiboot

**Resultado esperado**: El kernel debe mostrar información de memoria del sistema.

---

### Ejercicio 4: Bootloader Multi-etapa
**Dificultad**: Muy Avanzada

**Objetivo**: Crear un bootloader personalizado que cargue nuestro kernel.

**Instrucciones**:
1. Crea `bootloader.asm` con funcionalidad básica:
```assembly
[BITS 16]
[ORG 0x7C00]

start:
    ; Configurar segmentos
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    
    ; Mostrar mensaje
    mov si, msg_loading
    call print_string
    
    ; Cargar kernel desde disco
    mov ah, 0x02    ; Función read sectors
    mov al, 10      ; 10 sectores
    mov ch, 0       ; Cylinder 0
    mov cl, 2       ; Sector 2 (después del bootloader)
    mov dh, 0       ; Head 0
    mov bx, 0x1000  ; Dirección de carga
    int 0x13        ; Llamada BIOS
    
    ; Saltar al kernel
    jmp 0x1000
    
print_string:
    lodsb
    or al, al
    jz .done
    mov ah, 0x0E
    int 0x10
    jmp print_string
.done:
    ret
    
msg_loading db 'Cargando kernel personalizado...', 13, 10, 0

times 510-($-$$) db 0
dw 0xAA55
```

2. Modifica el Makefile para crear imagen de disco
3. Testa el bootloader en QEMU

**Desafío adicional**: Implementa lectura desde sistema de archivos FAT12.

---

## 📋 Resumen del Capítulo

### Conceptos Clave Aprendidos

1. **Arquitectura x86**:
   - Modos de operación (Real, Protegido, Long)
   - Registros de propósito general y especiales
   - Segmentación y paginación como mecanismos de memoria

2. **Proceso de Boot**:
   - Secuencia completa desde encendido hasta kernel
   - Rol del BIOS/UEFI en la inicialización
   - Bootloaders multi-etapa y sus limitaciones

3. **Multiboot Specification**:
   - Estándar para compatibilidad bootloader-kernel
   - Header Multiboot y sus campos
   - Información del sistema pasada al kernel

4. **Cross-Compilation**:
   - Necesidad de toolchain específico para bare metal
   - Configuración de i686-elf-gcc
   - Scripts de linker para organización de memoria

### Habilidades Técnicas Desarrolladas

- ✅ **Configuración** de entorno de desarrollo para OS
- ✅ **Análisis** de código ensamblador x86
- ✅ **Modificación** de bootloaders existentes
- ✅ **Debugging** usando QEMU y GDB
- ✅ **Compilación** cruzada para arquitecturas bare metal

### Conexión con el Código del Mini-Kernel

En nuestro mini-kernel educativo, estos conceptos se aplican directamente:

- **boot.s**: Implementa Multiboot header y configuración inicial
- **linker.ld**: Organiza memoria según convenciones x86
- **Makefile**: Utiliza cross-compilation toolchain
- **kernel.c**: Recibe control del bootloader en modo protegido

### Preparación para el Siguiente Capítulo

El **Capítulo 2** se basará en estos fundamentos para explorar:
- Gestión detallada de memoria en modo protegido
- Controladores de dispositivos de bajo nivel
- Manejo de interrupciones y E/S por puertos
- Implementación de drivers para VGA y teclado PS/2

Los ejercicios de este capítulo han preparado el entorno necesario y la comprensión básica de la arquitectura que será esencial para los temas más avanzados del curso.

---

**📚 Lecturas Complementarias:**
- Intel 80386 Programmer's Reference Manual
- OSDev Wiki: "Getting Started" section
- GNU GRUB Multiboot Specification
- "PC Assembly Language" por Paul Carter