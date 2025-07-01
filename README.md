# Mini Kernel Educativo - Sistema Operativo en C

## 📚 Propósito Educativo

Este proyecto implementa un **sistema operativo mínimo** diseñado específicamente para estudiantes que desean comprender los fundamentos de un OS desde cero. El código está completamente documentado en español y incluye explicaciones pedagógicas de cada componente.

## 🎯 Objetivos de Aprendizaje

Al estudiar y extender este proyecto, los estudiantes aprenderán:

- **Arquitectura x86**: Programación en ensamblador y modo protegido
- **Bootloader**: Proceso de arranque desde BIOS hasta kernel
- **Gestión de memoria**: Manejo directo de memoria física
- **Sistemas de archivos**: Implementación de FAT16 simplificado
- **Entrada/Salida**: Controladores de teclado PS/2 y pantalla VGA
- **Shell**: Intérprete de comandos básico
- **Cross-compilation**: Toolchain para desarrollo de sistemas embebidos

## 🏗️ Arquitectura del Sistema

### Componentes Principales

```
┌─────────────────────────────────────────────────────┐
│                   APLICACIONES                      │
│  Shell con comandos: ls, cat, cp, mv, edln, etc.   │
├─────────────────────────────────────────────────────┤
│                SISTEMA DE ARCHIVOS                  │
│     FAT16 simplificado en RAM (289 sectores)       │
├─────────────────────────────────────────────────────┤
│                 CONTROLADORES                       │
│    Teclado PS/2  │  Pantalla VGA  │  Memoria RAM    │
├─────────────────────────────────────────────────────┤
│                     KERNEL                          │
│  kernel.c - Núcleo principal del sistema operativo │
├─────────────────────────────────────────────────────┤
│                   BOOTLOADER                        │
│   boot.s - Multiboot + Arranque en modo protegido  │
└─────────────────────────────────────────────────────┘
```

### Proceso de Arranque

1. **BIOS/QEMU** carga el bootloader usando Multiboot
2. **boot.s** configura el modo protegido y salta al kernel
3. **kernel.c** inicializa los subsistemas y entra en el shell
4. **Usuario** interactúa a través de comandos del shell

## 📁 Estructura de Archivos

```
r2os/
├── kernel.c          # Núcleo principal (sistema de archivos, shell, drivers)
├── boot.s            # Bootloader Multiboot en ensamblador
├── linker.ld         # Script del enlazador (layout de memoria)
├── Makefile          # Sistema de construcción automatizada
├── CLAUDE.md         # Instrucciones para asistente de IA
└── README.md         # Esta documentación
```

## 🔧 Herramientas Necesarias

### Cross-Compilation Toolchain

```bash
# En Ubuntu/Debian:
sudo apt install build-essential gcc-multilib nasm qemu-system-x86

# Instalar herramientas i686-elf:
# 1. Descargar desde: https://github.com/lordmilko/i686-elf-tools
# 2. O compilar desde fuentes siguiendo: https://wiki.osdev.org/GCC_Cross-Compiler
```

### Herramientas Requeridas

- `i686-elf-gcc` - Compilador C para arquitectura i686
- `i686-elf-as` - Ensamblador
- `i686-elf-ld` - Enlazador
- `i686-elf-objcopy` - Manipulación de archivos objeto
- `qemu-system-i386` - Emulador para pruebas

## 🚀 Compilación y Ejecución

### Comandos Básicos

```bash
# Compilar el sistema operativo
make

# Ejecutar en QEMU
make run

# Limpiar archivos generados
make clean
```

### Proceso de Compilación

1. **Ensamblado**: `boot.s` → `boot.o`
2. **Compilación**: `kernel.c` → `kernel.o`
3. **Enlazado**: `boot.o` + `kernel.o` → `myos.elf`
4. **Conversión**: `myos.elf` → `myos.bin` (opcional)

## 💻 Uso del Sistema

### Comandos Disponibles

| Comando | Descripción | Ejemplo |
|---------|-------------|---------|
| `ls` | Listar archivos | `ls` |
| `cat <archivo>` | Mostrar contenido | `cat hola.txt` |
| `touch <archivo>` | Crear archivo vacío | `touch nuevo.txt` |
| `copycon <archivo>` | Crear desde teclado | `copycon test.txt` |
| `edln <archivo> <línea> <texto>` | Editar línea específica | `edln test.txt 2 Hello World` |
| `cp <origen> <destino>` | Copiar archivo | `cp test.txt backup.txt` |
| `mv <viejo> <nuevo>` | Renombrar archivo | `mv test.txt archivo.txt` |
| `delete <archivo>` | Eliminar archivo | `delete temp.txt` |
| `echo <texto>` | Imprimir texto | `echo Hola Mundo` |
| `free` | Mostrar memoria | `free` |
| `clear` / `cls` | Limpiar pantalla | `clear` |
| `help` / `?` | Mostrar ayuda | `help` |

### Funcionalidades del Teclado

- **Letras**: a-z (minúsculas), A-Z (con Shift)
- **Números**: 0-9
- **Símbolos**: .,;'[]/-= y con Shift: !@#$%^&*()_+{}:"<>?
- **Navegación**: ←/→ (mover cursor), ↑/↓ (historial)
- **Edición**: Backspace, Delete, Enter
- **Control**: Ctrl+Z/D (terminar copycon)

## 🧪 Experimentación y Extensión

### Actividades Propuestas para Estudiantes

#### Nivel Básico
1. **Agregar comando `date`**: Mostrar fecha/hora actual
2. **Comando `hexdump`**: Mostrar archivos en hexadecimal
3. **Mejorar `ls`**: Agregar timestamps, permisos simulados
4. **Comando `wc`**: Contar líneas, palabras, caracteres

#### Nivel Intermedio
5. **Pipes simples**: Implementar `cat archivo.txt | grep palabra`
6. **Variables de entorno**: `set VAR=valor`, `echo $VAR`
7. **Redirección**: `echo texto > archivo.txt`, `cat < archivo.txt`
8. **Subdirectorios**: Extender FAT16 para soportar carpetas

#### Nivel Avanzado
9. **Multitasking básico**: Implementar procesos simples
10. **Red básica**: Driver para tarjeta de red virtual
11. **GUI simple**: Modo gráfico VGA con ventanas
12. **Syscalls**: Interfaz para programas de usuario

### Puntos de Extensión Clave

#### 1. Sistema de Archivos
- **Ubicación**: Funciones `fs_*` en `kernel.c`
- **Mejoras**: Subdirectorios, permisos, timestamps
- **Desafío**: Implementar ext2 simplificado

#### 2. Shell y Comandos
- **Ubicación**: Función `shell_loop()` y parsing
- **Mejoras**: Autocompletado, aliases, scripts
- **Desafío**: Parser de sintaxis complejo

#### 3. Drivers de Hardware
- **Ubicación**: Funciones `keyboard_*`, `putchar`, VGA
- **Mejoras**: Mouse, sonido, timers
- **Desafío**: Driver de disco real (ATA/SATA)

#### 4. Gestión de Memoria
- **Ubicación**: Actualmente estático en `linker.ld`
- **Mejoras**: Malloc/free, paginación, memoria virtual
- **Desafío**: Garbage collector automático

## 🔍 Análisis del Código

### Conceptos Importantes

#### Modo Freestanding
```c
// Sin biblioteca estándar - implementamos todo
static inline void *memcpy(void *dest, const void *src, unsigned int n) {
    // Implementación manual de memcpy
}
```

#### Acceso Directo al Hardware
```c
// Comunicación con puertos I/O
static inline void outb(uint16_t port, uint8_t val) {
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}
```

#### Sistema de Archivos en RAM
```c
// Simulación de disco en memoria
static uint8_t *disk_image = disk_image_start;
static void read_sector(uint32_t lba, void *buf) {
    memcpy(buf, disk_image + lba * SECTOR_SIZE, SECTOR_SIZE);
}
```

### Debugging y Troubleshooting

#### Problemas Comunes

1. **"Invalid kernel header"**
   - Verificar Multiboot en `boot.s`
   - Revisar `linker.ld` alignment

2. **Sistema no arranca**
   - Comprobar toolchain i686-elf
   - Verificar flags en Makefile

3. **Comandos no funcionan**
   - Debug con `printf` en shell_loop
   - Verificar parsing de argumentos

#### Herramientas de Debug

```bash
# Desensamblar el kernel
i686-elf-objdump -d myos.elf | less

# Información de secciones
i686-elf-readelf -l myos.elf

# QEMU con monitor
qemu-system-i386 -kernel myos.elf -monitor stdio
```

## 📖 Recursos Adicionales

### Documentación Técnica
- [OSDev Wiki](https://wiki.osdev.org/) - Recurso principal para desarrollo de OS
- [Intel x86 Manual](https://software.intel.com/content/www/us/en/develop/articles/intel-sdm.html)
- [Multiboot Specification](https://www.gnu.org/software/grub/manual/multiboot/)

### Libros Recomendados
- "Operating System Concepts" - Silberschatz, Galvin, Gagne
- "Modern Operating Systems" - Andrew Tanenbaum
- "Programming from the Ground Up" - Jonathan Bartlett

### Tutoriales Online
- [Bran's Kernel Development Tutorial](http://www.osdever.net/bkerndev/Docs/title.htm)
- [James Molloy's Kernel Tutorials](http://www.jamesmolloy.co.uk/tutorial_html/)

## 🤝 Contribución y Desarrollo

### Para Estudiantes

1. **Fork** este repositorio
2. **Experimenta** con las actividades propuestas
3. **Documenta** tus cambios en español
4. **Comparte** tus extensiones con la comunidad

### Para Instructores

Este proyecto puede servir como:
- **Proyecto final** de curso de Sistemas Operativos
- **Laboratorios prácticos** semanales
- **Base** para investigación en sistemas embebidos
- **Introducción** a programación de sistemas

---

## 📜 Licencia

Este proyecto es de dominio público educativo. Se alienta su uso, modificación y distribución para fines académicos.

**¡Diviértete explorando los fundamentos de los sistemas operativos!** 🚀