# Mini Kernel Educativo - Sistema Operativo en C

## 📚 Propósito Educativo

Este proyecto implementa un **sistema operativo mínimo** diseñado específicamente para estudiantes que desean comprender los fundamentos de un OS desde cero. El código está completamente documentado en español y incluye explicaciones pedagógicas de cada componente.

El sistema cuenta con **más de 35 comandos** de shell, sistema de **pipes funcional**, **edición de archivos en línea**, y un **sistema de archivos FAT16 completo**, proporcionando una experiencia educativa similar a un sistema UNIX básico.

## 🎯 Objetivos de Aprendizaje

Al estudiar y extender este proyecto, los estudiantes aprenderán:

- **Arquitectura x86**: Programación en ensamblador y modo protegido
- **Bootloader**: Proceso de arranque desde BIOS hasta kernel
- **Gestión de memoria**: Manejo directo de memoria física
- **Sistemas de archivos**: Implementación de FAT16 simplificado
- **Entrada/Salida**: Controladores de teclado PS/2 y pantalla VGA
- **Shell y Pipes**: Intérprete de comandos con comunicación inter-proceso
- **Cross-compilation**: Toolchain para desarrollo de sistemas embebidos

## 🏗️ Arquitectura del Sistema

### Componentes Principales

```
┌─────────────────────────────────────────────────────┐
│                   APLICACIONES                      │
│  Shell con 35+ comandos: ls, cat, pipes, edición   │
├─────────────────────────────────────────────────────┤
│              SISTEMA DE PIPES                       │
│  Comunicación entre comandos: cmd1 | cmd2 | cmd3   │
├─────────────────────────────────────────────────────┤
│                SISTEMA DE ARCHIVOS                  │
│     FAT16 completo en RAM (289 sectores)           │
│  Operaciones: CRUD, edición línea, directorios     │
├─────────────────────────────────────────────────────┤
│                 CONTROLADORES                       │
│  Teclado PS/2   │  Pantalla VGA   │  Memoria RAM    │
│  Shift/Ctrl     │  Scroll/Cursor  │  Gestión dinámica│
├─────────────────────────────────────────────────────┤
│                     KERNEL                          │
│  kernel.c - Núcleo principal (2000+ líneas)        │
├─────────────────────────────────────────────────────┤
│                   BOOTLOADER                        │
│   boot.s - Multiboot + Arranque en modo protegido  │
└─────────────────────────────────────────────────────┘
```

### Proceso de Arranque

1. **BIOS/QEMU** carga el bootloader usando Multiboot
2. **boot.s** configura el modo protegido y salta al kernel
3. **kernel.c** inicializa VGA, teclado y sistema de archivos
4. **Shell** presenta prompt interactivo con historial y navegación
5. **Usuario** ejecuta comandos y pipes de forma interactiva

## 📁 Estructura de Archivos

```
r2os/
├── kernel.c           # Núcleo principal (2000+ líneas)
├── boot.s             # Bootloader Multiboot
├── linker.ld          # Script del linker
├── Makefile           # Sistema de construcción
├── README.md          # Este archivo
├── CLAUDE.md          # Instrucciones para desarrollo
├── curso/             # 📚 Material educativo completo
│   ├── PLAN_CURSO_SO.md    # Plan del curso de 4 meses
│   ├── CAPITULO_01.md      # Arquitectura x86 y Bootloaders
│   ├── CAPITULO_02.md      # Gestión de Memoria y E/S
│   ├── CAPITULO_03.md      # Sistemas de Archivos
│   ├── CAPITULO_04.md      # Shell y Comunicación Inter-Proceso
│   └── README.md           # Guía del curso
└── docker-build/      # 🐳 Entorno Docker de desarrollo
    ├── Dockerfile          # Imagen con i686-elf-gcc
    ├── Makefile           # Comandos Docker simplificados
    ├── README.md          # Documentación Docker
    └── .dockerignore      # Optimización de contexto
```

## 🚀 Compilación y Ejecución

### 🐳 Opción 1: Usando Docker (Recomendado - SIN XQuartz)

**Sin instalación compleja - Compilación en Docker + QEMU nativo**

```bash
# Navegar al entorno Docker
cd docker-build

# Configuración automática completa (solo la primera vez)
make setup-all

# ¡Compilar y ejecutar con ventana!
make run-gui
```

**Ventajas del nuevo método:**
- ✅ **Sin XQuartz**: QEMU se ejecuta en el host nativo
- ✅ **Compilación aislada**: Cross-compiler en Docker  
- ✅ **GUI nativa**: Ventana QEMU normal del sistema
- ✅ **Setup automático**: Instala QEMU automáticamente
- ✅ **Cero configuración**: Un solo comando para empezar

### 🔧 Opción 2: Instalación Local

```bash
# Ubuntu/Debian
sudo apt update
sudo apt install build-essential qemu-system-i386
sudo apt install gcc-multilib g++-multilib

# Toolchain cross-compilation (i686-elf)
# Seguir guía en: https://wiki.osdev.org/GCC_Cross-Compiler
```

**Comandos básicos:**
```bash
# Compilar el kernel
make

# Ejecutar en QEMU
make run

# Limpiar archivos compilados
make clean
```

### Salida Esperada

```
Mini-Kernel Educativo iniciado!
Inicializando VGA... OK
Inicializando teclado... OK
Inicializando filesystem FAT16... OK
Sistema listo. Usa 'help' para ver comandos disponibles.

shell> help
=== MINI-KERNEL EDUCATIVO - COMANDOS DISPONIBLES ===
...
shell> ls
(lista de archivos)
shell> echo "Hello World" | rev
dlroW olleH
shell> 
```

## 📋 Sistema de Comandos

### Comandos de Archivos y Directorios (15 comandos)
- `ls` - Listar archivos
- `cat <file>` - Mostrar contenido
- `touch <file>` - Crear archivo vacío
- `cp <src> <dst>` - Copiar archivo
- `mv <old> <new>` - Renombrar archivo
- `delete <file>` - Eliminar archivo
- `mkdir <dir>` - Crear directorio (simulado)
- `pwd` - Mostrar directorio actual
- `copycon <file>` - Crear archivo desde teclado
- `edln <file> <num> <texto>` - Editar línea específica
- `delln <file> <num>` - Eliminar línea específica
- `insln <file> <num>` - Insertar línea en blanco
- `tee <file>` - Escribir a archivo y pantalla
- `stat <file>` - Estadísticas de archivo
- `du <file>` - Uso de disco del archivo

### Comandos de Análisis de Texto (8 comandos)
- `wc <file>` - Contar líneas, palabras, caracteres
- `head <file> [n]` - Mostrar primeras n líneas
- `tail <file> [n]` - Mostrar últimas n líneas
- `grep <txt> <file>` - Buscar texto en archivo
- `hexdump <file>` - Mostrar archivo en hexadecimal
- `file <file>` - Determinar tipo de archivo
- `sort <file>` - Ordenar contenido (simulado)
- `rev <text>` - Invertir texto

### Comandos de Sistema (7 comandos)
- `echo <text>` - Imprimir texto
- `which <cmd>` - Encontrar ubicación de comando
- `whoami` - Mostrar usuario actual
- `uname` - Información del sistema
- `date` - Fecha actual
- `uptime` - Tiempo funcionamiento
- `free` - Uso de memoria

### Comandos de Shell (5 comandos)
- `history` - Historial de comandos
- `cls/clear` - Limpiar pantalla
- `help` - Mostrar todos los comandos
- `man <comando>` - Documentación de comando
- `testpipe` - Diagnosticar teclas del teclado

## 🔧 Sistema de Pipes

### Comandos de Entrada (Generan datos)
- `ls`, `cat <file>`, `echo <text>`, `rev <text>`
- `date`, `whoami`, `uname`

### Comandos de Salida (Procesan datos)
- `grep <patrón>`, `wc`, `head [n]`, `tail [n]`
- `rev`, `sort`, `uniq`, `cut`

### Ejemplos de Uso
```bash
# Análisis básico
ls | wc                    # Contar archivos
echo "hello world" | rev   # Invertir texto

# Búsqueda y filtrado
cat archivo.txt | grep error     # Buscar errores
ls | grep .txt                   # Solo archivos .txt

# Combinaciones creativas
date | rev                       # Fecha invertida
whoami | grep root              # Verificar usuario
cat log.txt | head 10 | grep WARN  # Primeras 10 líneas con warnings
```

## 🔍 Funcionalidades Avanzadas

### Edición de Archivos en Línea
```bash
shell> edln archivo.txt 5 "Nueva línea 5"   # Editar línea 5
shell> delln archivo.txt 3                  # Eliminar línea 3
shell> insln archivo.txt 2                  # Insertar línea en blanco en posición 2
```

### Navegación y Historial
- **Flechas arriba/abajo**: Navegar historial de comandos
- **Flechas izquierda/derecha**: Mover cursor en línea actual
- **Backspace**: Borrar carácter a la izquierda
- **Delete**: Borrar carácter a la derecha
- **Ctrl+combinaciones**: Caracteres de control

### Sistema de Archivos FAT16
- **289 sectores** organizados como FAT16 real
- **32 entradas** de directorio raíz
- **Operaciones atómicas** de archivo
- **Detección de errores** y validación
- **Metadata completa** (tamaño, atributos, clusters)

## 📚 Material Educativo

### 📚 Curso Completo de Sistemas Operativos (carpeta `curso/`)
1. **CAPITULO_01.md** - Arquitectura x86 y Bootloaders
2. **CAPITULO_02.md** - Gestión de Memoria y E/S
3. **CAPITULO_03.md** - Sistemas de Archivos
4. **CAPITULO_04.md** - Shell y Comunicación Inter-Proceso

### 📅 Plan de Estudio de 4 Meses
- **PLAN_CURSO_SO.md** - Cronograma detallado semanal
- **16 semanas** de contenido progresivo
- **Proyectos prácticos** cada semana
- **Evaluaciones** y trabajo final integrador
- **Extensiones avanzadas** para estudiantes destacados

### 🐳 Entorno Docker (carpeta `docker-build/`)
- **Setup automático** de cross-compiler i686-elf-gcc
- **QEMU integrado** para emulación
- **Funciona en Linux, macOS y Windows**
- **Sin configuración compleja** de herramientas

## 🎓 Ejercicios y Proyectos

### Nivel Básico
1. Agregar nuevos comandos al shell
2. Modificar el formato de salida de `ls`
3. Implementar comando `echo` con escape sequences
4. Crear comando `time` que mida duración de otros comandos

### Nivel Intermedio
1. Implementar subdirectorios en el sistema de archivos
2. Agregar soporte para archivos binarios
3. Crear comando `find` para buscar archivos
4. Implementar redirección de salida (`>` y `>>`)

### Nivel Avanzado
1. Agregar soporte para múltiples pipes (`cmd1 | cmd2 | cmd3`)
2. Implementar variables de entorno
3. Crear sistema básico de permisos de archivos
4. Agregar soporte para procesos en background (`&`)

## 🔧 Depuración y Desarrollo

### Herramientas de Debug
```bash
# Compilar con símbolos de debug
make debug

# Ejecutar con GDB
qemu-system-i386 -kernel myos.elf -s -S &
gdb myos.elf
(gdb) target remote localhost:1234
(gdb) break kernel_main
(gdb) continue
```

### Comando de Diagnóstico
```bash
shell> testpipe    # Diagnosticar problemas con el carácter "|"
```

## 📖 Referencias y Documentación

### Documentación Técnica
- [OSDev Wiki](https://wiki.osdev.org/) - Recursos completos de desarrollo de OS
- [Intel x86 Manual](https://software.intel.com/content/www/us/en/develop/articles/intel-sdm.html)
- [Multiboot Specification](https://www.gnu.org/software/grub/manual/multiboot/)
- [FAT16 File System](https://en.wikipedia.org/wiki/File_Allocation_Table)

### Libros Recomendados
- "Operating System Concepts" - Silberschatz, Galvin, Gagne
- "Modern Operating Systems" - Andrew Tanenbaum  
- "The Design and Implementation of the FreeBSD Operating System"

## 🤝 Contribuciones

Este proyecto es educativo y está diseñado para:
- Estudiantes de Ciencias de la Computación
- Cursos de Sistemas Operativos
- Autodidactas interesados en OS development
- Investigadores en sistemas embebidos

### Cómo Contribuir
1. Fork del repositorio
2. Crear rama para nueva funcionalidad
3. Documentar cambios en español
4. Incluir ejemplos y ejercicios
5. Enviar Pull Request con descripción detallada

## 📄 Licencia

Proyecto educativo de código abierto. Libre para uso académico y educativo.

---

## 🚀 **¡Empezar es fácil!**

### 🐳 Con Docker (Recomendado)
```bash
git clone <repository>
cd r2os/docker-build
make setup-all    # Configura todo automáticamente
make run-gui      # ¡Compilar y ejecutar con ventana!
```

### 🔧 Instalación Local
```bash
git clone <repository>
cd r2os
make run        # (Requiere i686-elf-gcc instalado)
```

### 💻 En el shell del kernel:
```bash
shell> help
shell> testpipe    # Si tienes problemas con el carácter |
shell> echo "Hello World" | rev
shell> ls | wc
shell> cat curso/README.md    # ¡Explorar el curso!
```

**¡Explora, experimenta y aprende cómo funciona un sistema operativo desde adentro!** 🎓✨🐳