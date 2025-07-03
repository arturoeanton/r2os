# 🎓 Mini Kernel Educativo - Curso Completo de Sistemas Operativos

## 📚 Propósito Educativo

Este proyecto implementa un **sistema operativo educativo completo** diseñado específicamente para estudiantes universitarios y autodidactas que desean comprender los fundamentos de los sistemas operativos desde cero. El código está completamente documentado en español y incluye un **curso estructurado de 8 capítulos** con más de 32 ejercicios prácticos.

El sistema cuenta con **más de 35 comandos** de shell, sistema de **pipes funcional**, **edición de archivos en línea**, **sistema de archivos FAT16 completo**, **stack de red TCP/IP**, **multitasking preemptivo**, y **capacidades de virtualización**, proporcionando una experiencia educativa comparable a sistemas operativos reales.

## 🎯 Objetivos de Aprendizaje

Al estudiar y extender este proyecto, los estudiantes aprenderán:

- **Arquitectura x86**: Programación en ensamblador y modo protegido completo
- **Bootloader**: Proceso de arranque desde BIOS hasta kernel con Multiboot
- **Gestión de memoria**: Paginación, memoria virtual, y protección de memoria
- **Sistemas de archivos**: Implementación de FAT16 con journaling y cache
- **Entrada/Salida**: Controladores completos y manejo de interrupciones
- **Shell y Pipes**: Intérprete avanzado con IPC y job control
- **Multitasking**: Scheduler preemptivo con sincronización
- **Redes**: Stack TCP/IP completo con sockets
- **Seguridad**: Control de acceso, usuarios, y virtualización
- **Cross-compilation**: Toolchain profesional con Docker

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

## 📁 Estructura del Repositorio

```
r2os/
├── 🔧 CÓDIGO FUENTE PRINCIPAL
│   ├── kernel.c           # Núcleo principal (2000+ líneas)
│   ├── boot.s             # Bootloader Multiboot
│   ├── linker.ld          # Script del linker
│   ├── Makefile           # Sistema de construcción
│   └── CLAUDE.md          # Instrucciones para desarrollo
│
├── 📚 CURSO EDUCATIVO COMPLETO (curso/)
│   ├── PRE_CURSO_00.md    # 🔰 Fundamentos de C y Ensamblador
│   ├── PRE_CURSO_01.md    # 🛠️ Herramientas y Entorno de Desarrollo
│   ├── CAPITULO_01.md     # 🔧 Arquitectura x86 y Bootloaders
│   ├── CAPITULO_02.md     # 💾 Gestión de Memoria y E/S
│   ├── CAPITULO_03.md     # 🗃️ Sistemas de Archivos
│   ├── CAPITULO_04.md     # 🖥️ Shell y Comunicación Inter-Proceso
│   ├── CAPITULO_05.md     # ⚡ Interrupciones y Excepciones
│   ├── CAPITULO_06.md     # 🔄 Multitasking y Scheduler
│   ├── CAPITULO_07.md     # 🌐 Redes y Protocolos
│   ├── CAPITULO_08.md     # 🔒 Seguridad y Virtualización
│   ├── PLAN_CURSO_SO.md   # 📋 Plan completo del curso (16 semanas)
│   └── README.md          # 📖 Guía del curso
│
├── 🐳 ENTORNO DOCKER HÍBRIDO (docker-build/)
│   ├── Dockerfile         # Imagen con toolchain completo
│   ├── Makefile          # Comandos Docker + QEMU host
│   ├── README.md         # Documentación completa
│   └── run-macos-gui.sh  # Script para macOS con GUI
│
├── 📋 DOCUMENTACIÓN DE DESARROLLO (docs/)
│   ├── INSTRUCCIONES_PIPE.md  # Desarrollo de pipes
│   ├── PIPES_MEJORADOS.md     # Mejoras implementadas
│   └── TEST_PIPES_FIXED.md    # Tests y verificaciones
│
├── 🛠️ SCRIPTS Y HERRAMIENTAS (scripts/)
│   ├── Makefile.debug     # Makefile para debugging
│   └── Makefile.test      # Makefile para testing
│
├── 💡 EJEMPLOS Y TESTS (examples/)
│   ├── debug-kernel.c     # Kernel de debugging
│   └── test-kernel.c      # Kernel de pruebas
│
└── 🏗️ ARCHIVOS COMPILADOS (build/)
    ├── *.o               # Archivos objeto
    ├── *.elf             # Ejecutables
    └── *.bin             # Binarios
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

## 📚 Material Educativo Completo

### 🎓 Curso Integral de Sistemas Operativos (carpeta `curso/`)

#### 🔰 **Prerrequisitos (2 capítulos)**
1. **PRE_CURSO_00.md** - Fundamentos de C y Ensamblador
2. **PRE_CURSO_01.md** - Herramientas y Entorno de Desarrollo

#### 📖 **Capítulos Principales (8 capítulos)**
1. **CAPITULO_01.md** - Arquitectura x86 y Bootloaders
2. **CAPITULO_02.md** - Gestión de Memoria y E/S
3. **CAPITULO_03.md** - Sistemas de Archivos
4. **CAPITULO_04.md** - Shell y Comunicación Inter-Proceso
5. **CAPITULO_05.md** - Interrupciones y Excepciones
6. **CAPITULO_06.md** - Multitasking y Scheduler
7. **CAPITULO_07.md** - Redes y Protocolos
8. **CAPITULO_08.md** - Seguridad y Virtualización

### 📅 Plan de Estudio de 4 Meses (16 Semanas)
- **PLAN_CURSO_SO.md** - Cronograma detallado semanal
- **32 ejercicios prácticos** distribuidos en 8 capítulos
- **Proyectos semanales** con evaluación
- **4 exámenes parciales** distribuidos por mes
- **Proyecto final integrador** que combina todos los capítulos
- **Extensiones avanzadas** para estudiantes destacados

### 🐳 Entorno Docker Híbrido (carpeta `docker-build/`)
- **Compilación en Docker**: Cross-compiler i686-elf-gcc aislado
- **Ejecución en Host**: QEMU nativo (sin XQuartz/X11)
- **Setup automático**: Un comando instala todo
- **GUI nativa**: Ventana QEMU del sistema operativo
- **Funciona en Linux, macOS y Windows**
- **Cero configuración**: `make setup-all` y listo

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

> 📖 **¿Primera vez aquí?** Lee [INICIO_RAPIDO.md](INICIO_RAPIDO.md) para una guía completa en 5 minutos.

### 🎓 **Opción 1: Curso Completo (Recomendado para aprender)**
```bash
git clone <repository>
cd r2os/curso

# 📖 Prerrequisitos (si es necesario)
# Leer PRE_CURSO_00.md (C y Ensamblador)
# Leer PRE_CURSO_01.md (Herramientas)

# 🚀 Comenzar el curso
# Seguir README.md del curso para la progresión completa
```

### 🐳 **Opción 2: Ejecución Rápida con Docker**
```bash
git clone <repository>
cd r2os/docker-build
make setup-all    # Configura todo automáticamente
make run-gui      # ¡Compilar y ejecutar con ventana!
```

### 🔧 **Opción 3: Instalación Local**
```bash
git clone <repository>
cd r2os
make run        # (Requiere i686-elf-gcc instalado)
```

### 💻 **En el shell del kernel:**
```bash
shell> help                           # Ver todos los comandos (35+)
shell> echo "Hello World" | rev       # Probar pipes
shell> ls | wc                        # Contar archivos
shell> cat README.md | head 10        # Ver primeras líneas
shell> touch test.txt                 # Crear archivo
shell> echo "Contenido" > test.txt    # Escribir archivo
shell> ls -l                          # Listar con detalles
```

### 📚 **Explorar el curso educativo:**
```bash
# Leer la guía completa del curso
shell> cat curso/README.md

# Comenzar con prerrequisitos
shell> cat curso/PRE_CURSO_00.md      # C y Ensamblador
shell> cat curso/PRE_CURSO_01.md      # Herramientas

# Primer capítulo del curso
shell> cat curso/CAPITULO_01.md       # Arquitectura x86
```

**¡Explora, experimenta y domina los sistemas operativos desde los fundamentos hasta lo más avanzado!** 🎓✨🐳