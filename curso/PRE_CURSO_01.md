# PRE-CURSO 01: Herramientas y Entorno de Desarrollo

## 📚 Introducción

Este capítulo cubre todas las herramientas necesarias para desarrollar sistemas operativos, desde la instalación hasta el uso avanzado. Aprenderás a configurar un entorno de desarrollo completo y a dominar las herramientas profesionales que usan los desarrolladores de kernels.

## 🎯 Objetivos de Aprendizaje

Al completar este pre-curso, serás capaz de:

1. **Configurar** un entorno de desarrollo cross-platform completo
2. **Usar** herramientas de compilación y debugging profesionales
3. **Entender** el proceso de build de un sistema operativo
4. **Dominar** QEMU para emulación y testing
5. **Aplicar** técnicas de debugging y análisis de bajo nivel

---

## 🛠️ Parte I: Herramientas Fundamentales

### 1.1 Cross-Compiler (i686-elf-gcc)

#### ¿Por qué un Cross-Compiler?

```bash
# ❌ Compilador nativo (NO funciona para kernels)
gcc kernel.c -o kernel  # Genera ejecutable para el OS host

# ✅ Cross-compiler (SÍ funciona para kernels)
i686-elf-gcc -ffreestanding -nostdlib kernel.c -o kernel.o
```

Un cross-compiler nos permite compilar código para una arquitectura diferente a la de nuestro sistema host. Para desarrollo de OS necesitamos:

- **Target**: i686 (x86 de 32 bits)
- **Host**: nuestro sistema (Linux, macOS, Windows)
- **Sin dependencias**: no usa bibliotecas del OS host

#### Instalación de i686-elf-gcc

**En Ubuntu/Debian:**
```bash
# Dependencias para compilar el cross-compiler
sudo apt update
sudo apt install build-essential bison flex libgmp3-dev libmpc-dev libmpfr-dev texinfo

# Opción 1: Instalar desde repositorio (más fácil)
sudo apt install gcc-multilib

# Opción 2: Compilar desde fuente (más control)
# Ver scripts de instalación en OSDev Wiki
```

**En macOS:**
```bash
# Instalar Homebrew si no lo tienes
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Instalar cross-compiler
brew install i686-elf-gcc
brew install i686-elf-binutils

# Verificar instalación
i686-elf-gcc --version
```

**En Windows (WSL recomendado):**
```bash
# Usar WSL2 con Ubuntu
wsl --install -d Ubuntu

# Dentro de WSL, seguir instrucciones de Ubuntu
sudo apt install gcc-multilib
```

#### Verificación de la Instalación

```bash
# Verificar que las herramientas están disponibles
which i686-elf-gcc
which i686-elf-ld
which i686-elf-objdump
which i686-elf-objcopy

# Verificar versión y target
i686-elf-gcc --version
i686-elf-gcc -dumpmachine  # Debe mostrar: i686-elf
```

### 1.2 QEMU - Emulador de Sistema

#### Instalación de QEMU

**Ubuntu/Debian:**
```bash
sudo apt install qemu-system-x86
```

**macOS:**
```bash
brew install qemu
```

**Windows:**
```bash
# En WSL
sudo apt install qemu-system-x86

# O descargar binarios desde: https://www.qemu.org/download/
```

#### Opciones Básicas de QEMU

```bash
# Comando básico para ejecutar kernel
qemu-system-i386 -kernel myos.elf

# Opciones comunes explicadas:
qemu-system-i386 \
    -kernel myos.elf \          # Cargar kernel directamente
    -m 32 \                     # 32MB de RAM
    -display curses \           # Display en terminal
    -serial stdio \             # Salida serial a terminal
    -no-reboot \               # No reiniciar automáticamente
    -no-shutdown               # No cerrar automáticamente
```

#### Opciones Avanzadas de QEMU

```bash
# Para debugging con GDB
qemu-system-i386 \
    -kernel myos.elf \
    -s \                       # Habilitar GDB server en puerto 1234
    -S \                       # Pausar al inicio
    -m 32

# Para monitoreo de sistema
qemu-system-i386 \
    -kernel myos.elf \
    -monitor stdio \           # Monitor QEMU en terminal
    -m 32

# Con dispositivos adicionales
qemu-system-i386 \
    -kernel myos.elf \
    -netdev user,id=net0 \     # Networking
    -device e1000,netdev=net0 \ # Tarjeta de red
    -m 32
```

### 1.3 GDB - GNU Debugger

#### Configuración para Debugging de Kernels

```bash
# Compilar kernel con símbolos de debug
i686-elf-gcc -g -ffreestanding -nostdlib -c kernel.c -o kernel.o
i686-elf-ld -T linker.ld kernel.o -o myos.elf

# En terminal 1: Ejecutar QEMU con GDB server
qemu-system-i386 -kernel myos.elf -s -S

# En terminal 2: Conectar GDB
i686-elf-gdb myos.elf
(gdb) target remote localhost:1234
(gdb) break kernel_main
(gdb) continue
```

#### Comandos Esenciales de GDB

```gdb
# Navegación básica
(gdb) break function_name      # Establecer breakpoint
(gdb) break *0x100000         # Breakpoint en dirección específica
(gdb) continue                # Continuar ejecución
(gdb) step                    # Ejecutar línea (entrar en funciones)
(gdb) next                    # Ejecutar línea (saltar funciones)
(gdb) stepi                   # Ejecutar una instrucción
(gdb) nexti                   # Ejecutar una instrucción (saltar calls)

# Inspección de estado
(gdb) info registers          # Ver todos los registros
(gdb) print $eax              # Ver registro específico
(gdb) print variable          # Ver variable
(gdb) x/10x $esp              # Ver memoria (10 words en hex desde ESP)
(gdb) x/10i $eip              # Ver instrucciones desde EIP
(gdb) backtrace               # Ver call stack

# Modificación de estado
(gdb) set $eax = 42           # Modificar registro
(gdb) set variable = 100      # Modificar variable
(gdb) jump *0x100000          # Saltar a dirección
```

#### Script de GDB para Debugging Automático

```bash
# Crear archivo .gdbinit
cat > .gdbinit << 'EOF'
# Conectar automáticamente a QEMU
target remote localhost:1234

# Configuración para x86
set architecture i386

# Breakpoints útiles
break kernel_main
break panic

# Comandos personalizados
define regs
    info registers
end

define stack
    x/20x $esp
end

define code
    x/10i $eip
end

# Auto-mostrar registros importantes
display/x $eax
display/x $ebx
display/x $ecx
display/x $edx
display/x $esp
display/x $ebp
display/x $eip

echo \n=== GDB listo para debugging ===\n
EOF
```

### 1.4 Herramientas de Análisis Binario

#### objdump - Desensamblador

```bash
# Ver todas las secciones
i686-elf-objdump -h myos.elf

# Desensamblar código
i686-elf-objdump -d myos.elf

# Ver símbolos
i686-elf-objdump -t myos.elf

# Desensamblar sección específica
i686-elf-objdump -d -j .text myos.elf

# Con direcciones y opcodes
i686-elf-objdump -d -S myos.elf  # Si compilaste con -g
```

#### nm - Lista de Símbolos

```bash
# Ver todos los símbolos
i686-elf-nm myos.elf

# Solo símbolos definidos
i686-elf-nm -D myos.elf

# Ordenar por dirección
i686-elf-nm -n myos.elf

# Solo símbolos undefined
i686-elf-nm -u myos.elf
```

#### readelf - Información de ELF

```bash
# Ver headers ELF
i686-elf-readelf -h myos.elf

# Ver program headers
i686-elf-readelf -l myos.elf

# Ver section headers
i686-elf-readelf -S myos.elf

# Ver tabla de símbolos
i686-elf-readelf -s myos.elf
```

#### hexdump - Vista Hexadecimal

```bash
# Vista hexadecimal con ASCII
hexdump -C myos.bin

# Solo primeros 512 bytes (boot sector)
hexdump -C -n 512 myos.bin

# Buscar signature Multiboot
hexdump -C myos.bin | grep "1BADB002"
```

---

## 🔧 Parte II: Sistema de Build Avanzado

### 2.1 Makefiles para Sistemas Operativos

#### Makefile Básico

```makefile
# Variables de configuración
CC = i686-elf-gcc
AS = i686-elf-as
LD = i686-elf-ld
OBJCOPY = i686-elf-objcopy

# Flags de compilación
CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra
LDFLAGS = -ffreestanding -O2 -nostdlib

# Archivos fuente
C_SOURCES = $(wildcard *.c)
ASM_SOURCES = $(wildcard *.s)

# Archivos objeto
C_OBJECTS = $(C_SOURCES:.c=.o)
ASM_OBJECTS = $(ASM_SOURCES:.s=.o)
OBJECTS = $(C_OBJECTS) $(ASM_OBJECTS)

# Target principal
myos.elf: $(OBJECTS)
	$(LD) -T linker.ld -o $@ $^ $(LDFLAGS)

# Compilar archivos C
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Ensamblar archivos ASM
%.o: %.s
	$(AS) $< -o $@

# Limpiar archivos generados
clean:
	rm -f *.o myos.elf

# Ejecutar en QEMU
run: myos.elf
	qemu-system-i386 -kernel $<

# Debugging con GDB
debug: myos.elf
	qemu-system-i386 -kernel $< -s -S &
	i686-elf-gdb $<

.PHONY: clean run debug
```

#### Makefile Avanzado con Múltiples Directorios

```makefile
# Configuración del proyecto
PROJECT_NAME = mini-kernel
VERSION = 1.0

# Herramientas
CC = i686-elf-gcc
AS = i686-elf-as
LD = i686-elf-ld
OBJCOPY = i686-elf-objcopy
OBJDUMP = i686-elf-objdump

# Directorios
SRCDIR = src
INCDIR = include
BUILDDIR = build
OUTPUTDIR = output

# Flags
CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra -Werror
CFLAGS += -I$(INCDIR)
CFLAGS += -fno-builtin -fno-stack-protector
CFLAGS += -mno-red-zone -mno-mmx -mno-sse -mno-sse2

ASFLAGS = 

LDFLAGS = -ffreestanding -O2 -nostdlib
LDFLAGS += -Wl,--build-id=none

# Buscar archivos fuente recursivamente
SOURCES_C = $(shell find $(SRCDIR) -name '*.c')
SOURCES_ASM = $(shell find $(SRCDIR) -name '*.s')

# Generar nombres de archivos objeto
OBJECTS_C = $(SOURCES_C:$(SRCDIR)/%.c=$(BUILDDIR)/%.o)
OBJECTS_ASM = $(SOURCES_ASM:$(SRCDIR)/%.s=$(BUILDDIR)/%.o)
OBJECTS = $(OBJECTS_C) $(OBJECTS_ASM)

# Target principal
$(OUTPUTDIR)/$(PROJECT_NAME).elf: $(OBJECTS) | $(OUTPUTDIR)
	$(LD) -T linker.ld -o $@ $^ $(LDFLAGS)
	@echo "=== Build completado: $@ ==="

# Compilar archivos C
$(BUILDDIR)/%.o: $(SRCDIR)/%.c | $(BUILDDIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compilado: $<"

# Ensamblar archivos ASM
$(BUILDDIR)/%.o: $(SRCDIR)/%.s | $(BUILDDIR)
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) $< -o $@
	@echo "Ensamblado: $<"

# Crear directorios
$(BUILDDIR):
	mkdir -p $(BUILDDIR)

$(OUTPUTDIR):
	mkdir -p $(OUTPUTDIR)

# Targets de utilidad
all: $(OUTPUTDIR)/$(PROJECT_NAME).elf

clean:
	rm -rf $(BUILDDIR) $(OUTPUTDIR)
	@echo "Limpieza completada"

run: $(OUTPUTDIR)/$(PROJECT_NAME).elf
	qemu-system-i386 -kernel $< -m 32

debug: $(OUTPUTDIR)/$(PROJECT_NAME).elf
	@echo "Iniciando QEMU con GDB server..."
	qemu-system-i386 -kernel $< -s -S -m 32 &
	sleep 1
	i686-elf-gdb $< -ex "target remote localhost:1234"

# Análisis del binario
info: $(OUTPUTDIR)/$(PROJECT_NAME).elf
	@echo "=== Información del ejecutable ==="
	$(OBJDUMP) -h $<
	@echo ""
	@echo "=== Símbolos principales ==="
	i686-elf-nm $< | grep -E "(kernel_main|_start)"
	@echo ""
	@echo "=== Tamaño del archivo ==="
	ls -lh $<

disasm: $(OUTPUTDIR)/$(PROJECT_NAME).elf
	$(OBJDUMP) -d $< > $(OUTPUTDIR)/disassembly.txt
	@echo "Desensamblado guardado en $(OUTPUTDIR)/disassembly.txt"

# Testing automático
test: $(OUTPUTDIR)/$(PROJECT_NAME).elf
	@echo "Ejecutando tests automáticos..."
	timeout 10s qemu-system-i386 -kernel $< -display none -serial stdio || true

.PHONY: all clean run debug info disasm test
```

### 2.2 Scripts de Linker

#### linker.ld Básico

```ld
ENTRY(_start)

SECTIONS
{
    /* Kernel cargado en 1MB */
    . = 0x100000;

    .text : {
        *(.multiboot)      /* Header multiboot al inicio */
        *(.text)           /* Código */
    }

    .rodata : {
        *(.rodata)         /* Datos de solo lectura */
    }

    .data : {
        *(.data)           /* Datos inicializados */
    }

    .bss : {
        *(COMMON)          /* Variables comunes */
        *(.bss)            /* Datos no inicializados */
    }

    /* Símbolos para marcar fin del kernel */
    _kernel_end = .;
}
```

#### linker.ld Avanzado

```ld
ENTRY(_start)

SECTIONS
{
    /* Kernel cargado en dirección física 1MB */
    . = 0x100000;
    
    /* Guardar inicio del kernel */
    _kernel_start = .;
    
    /* Sección multiboot (debe estar al inicio) */
    .multiboot : {
        *(.multiboot)
    }
    
    /* Código ejecutable */
    .text : ALIGN(4K) {
        _text_start = .;
        *(.text)
        _text_end = .;
    }
    
    /* Datos de solo lectura */
    .rodata : ALIGN(4K) {
        _rodata_start = .;
        *(.rodata)
        _rodata_end = .;
    }
    
    /* Datos inicializados */
    .data : ALIGN(4K) {
        _data_start = .;
        *(.data)
        _data_end = .;
    }
    
    /* Datos no inicializados */
    .bss : ALIGN(4K) {
        _bss_start = .;
        *(COMMON)
        *(.bss)
        _bss_end = .;
    }
    
    /* Símbolos útiles para el kernel */
    _kernel_end = .;
    _kernel_size = _kernel_end - _kernel_start;
    
    /* Descartar secciones no necesarias */
    /DISCARD/ : {
        *(.comment)
        *(.eh_frame)
        *(.note.GNU-stack)
    }
}
```

### 2.3 Herramientas de Docker

#### Dockerfile para Desarrollo

```dockerfile
FROM ubuntu:22.04

# Instalar dependencias
RUN apt-get update && apt-get install -y \
    build-essential \
    gcc-multilib \
    nasm \
    qemu-system-x86 \
    gdb \
    git \
    curl \
    wget \
    && rm -rf /var/lib/apt/lists/*

# Crear usuario de desarrollo
RUN useradd -m -s /bin/bash developer
USER developer
WORKDIR /home/developer

# Variables de entorno
ENV PATH="/usr/local/cross/bin:${PATH}"
ENV TARGET=i686-elf
ENV PREFIX="/usr/local/cross"

# Script de bienvenida
RUN echo 'echo "=== Entorno de desarrollo de OS listo ==="' >> ~/.bashrc
RUN echo 'echo "Herramientas disponibles:"' >> ~/.bashrc
RUN echo 'echo "  - i686-elf-gcc"' >> ~/.bashrc
RUN echo 'echo "  - qemu-system-i386"' >> ~/.bashrc
RUN echo 'echo "  - gdb"' >> ~/.bashrc

CMD ["/bin/bash"]
```

#### docker-compose.yml

```yaml
version: '3.8'

services:
  os-dev:
    build: .
    volumes:
      - .:/workspace
    working_dir: /workspace
    tty: true
    stdin_open: true
    
  os-debug:
    build: .
    volumes:
      - .:/workspace
    working_dir: /workspace
    ports:
      - "1234:1234"  # Puerto GDB
    command: qemu-system-i386 -kernel myos.elf -s -S
```

---

## 🔍 Parte III: Debugging y Análisis Avanzado

### 3.1 Debugging con Símbolos

#### Compilación con Debug Info

```bash
# Compilar con información de debug
i686-elf-gcc -g -ffreestanding -nostdlib -c kernel.c -o kernel.o

# Verificar que los símbolos están presentes
i686-elf-objdump -t kernel.o | grep debug
i686-elf-readelf -S kernel.o | grep debug
```

#### Script de GDB Avanzado

```gdb
# Archivo: os-debug.gdb

# Configuración inicial
set architecture i386
target remote localhost:1234

# Función para mostrar estado completo
define show_state
    echo \n=== ESTADO DEL SISTEMA ===\n
    info registers
    echo \n=== STACK ===\n
    x/8x $esp
    echo \n=== CÓDIGO ACTUAL ===\n
    x/5i $eip
    echo \n========================\n
end

# Función para mostrar memoria de video
define show_vga
    echo \n=== MEMORIA VGA ===\n
    x/80x 0xb8000
end

# Breakpoints automáticos en funciones importantes
break kernel_main
break panic
break page_fault_handler

# Comandos personalizados para análisis
define trace_calls
    set logging on
    set logging file calls.log
    break *
    commands
        bt
        continue
    end
end

# Auto-ejecutar al conectar
continue
```

### 3.2 Profiling y Análisis de Rendimiento

#### Análisis de Tamaño de Binario

```bash
#!/bin/bash
# Script: analyze_binary.sh

KERNEL="myos.elf"

echo "=== ANÁLISIS DE TAMAÑO DEL KERNEL ==="
echo "Archivo: $KERNEL"
echo "Tamaño total: $(ls -lh $KERNEL | awk '{print $5}')"
echo ""

echo "=== SECCIONES ==="
i686-elf-objdump -h $KERNEL | grep -E "^\s*[0-9]"

echo ""
echo "=== TOP 10 SÍMBOLOS MÁS GRANDES ==="
i686-elf-nm --print-size --size-sort --radix=d $KERNEL | tail -10

echo ""
echo "=== FUNCIONES MÁS GRANDES ==="
i686-elf-objdump -t $KERNEL | grep " F " | \
    awk '{print $1 " " $NF}' | sort | \
    while read addr name; do
        size=$(i686-elf-objdump -t $KERNEL | awk "/$name/{getline; print \$1-0x$addr}")
        printf "%8d %s\n" "$size" "$name"
    done | sort -n | tail -10
```

#### Análisis de Performance con QEMU

```bash
# Script: performance_test.sh

echo "=== TEST DE RENDIMIENTO ==="

# Test 1: Tiempo de boot
echo "1. Tiempo de boot hasta kernel_main:"
time timeout 5s qemu-system-i386 -kernel myos.elf -display none -serial stdio \
    -monitor none 2>&1 | grep "Kernel iniciado"

# Test 2: Uso de memoria
echo "2. Uso de memoria:"
qemu-system-i386 -kernel myos.elf -m 32 -monitor stdio > /dev/null 2>&1 &
QEMU_PID=$!
sleep 2
echo "info memory" | nc localhost 1234 2>/dev/null || echo "Monitor no disponible"
kill $QEMU_PID 2>/dev/null

# Test 3: Análisis de instrucciones
echo "3. Conteo de instrucciones (primeros 1000 ciclos):"
qemu-system-i386 -kernel myos.elf -d exec,nochain \
    -display none -serial stdio 2>&1 | head -1000 | \
    grep -E "^Trace" | wc -l
```

### 3.3 Herramientas de Análisis Estático

#### Script de Verificación de Código

```bash
#!/bin/bash
# Script: code_check.sh

echo "=== VERIFICACIÓN DE CÓDIGO ==="

# 1. Verificar warnings del compilador
echo "1. Compilando con warnings máximos..."
i686-elf-gcc -Wall -Wextra -Werror -ffreestanding -nostdlib -c *.c

# 2. Verificar que no hay funciones de biblioteca estándar
echo "2. Verificando uso de funciones prohibidas..."
FORBIDDEN="printf scanf malloc free exit abort"
for func in $FORBIDDEN; do
    if i686-elf-nm myos.elf | grep -q "U $func"; then
        echo "❌ ADVERTENCIA: Función prohibida encontrada: $func"
    fi
done

# 3. Verificar alineación de stack
echo "3. Verificando alineación de stack..."
i686-elf-objdump -d myos.elf | grep -E "sub.*esp" | \
    while read line; do
        if echo "$line" | grep -qE "sub.*0x[0-9a-f]*[^048c]"; then
            echo "❌ ADVERTENCIA: Stack posiblemente mal alineado: $line"
        fi
    done

# 4. Verificar header multiboot
echo "4. Verificando header multiboot..."
if hexdump -C myos.elf | grep -q "02 b0 ad 1b"; then
    echo "✅ Header multiboot encontrado"
else
    echo "❌ ERROR: Header multiboot no encontrado"
fi

echo "=== VERIFICACIÓN COMPLETADA ==="
```

---

## 🧪 Ejercicios Prácticos

### Ejercicio 1: Configuración del Entorno (Nivel: Básico)

**Objetivo**: Configurar correctamente todas las herramientas de desarrollo.

```bash
# TODO: Completar este script de verificación

#!/bin/bash
# Script: verify_setup.sh

echo "=== VERIFICACIÓN DEL ENTORNO ==="

# 1. Verificar cross-compiler
echo "1. Verificando i686-elf-gcc..."
if command -v i686-elf-gcc &> /dev/null; then
    echo "✅ i686-elf-gcc encontrado: $(i686-elf-gcc --version | head -1)"
    # TODO: Verificar que el target es correcto
else
    echo "❌ i686-elf-gcc no encontrado"
fi

# 2. Verificar QEMU
echo "2. Verificando QEMU..."
# TODO: Verificar instalación de qemu-system-i386

# 3. Verificar GDB
echo "3. Verificando GDB..."
# TODO: Verificar que i686-elf-gdb está disponible

# 4. Test de compilación
echo "4. Test de compilación..."
# TODO: Crear un programa de prueba simple y compilarlo

echo "=== VERIFICACIÓN COMPLETADA ==="
```

### Ejercicio 2: Makefile Personalizado (Nivel: Intermedio)

**Objetivo**: Crear un Makefile que compile el proyecto con diferentes configuraciones.

```makefile
# TODO: Completar este Makefile

# Variables de configuración
CC = i686-elf-gcc
AS = i686-elf-as
LD = i686-elf-ld

# TODO: Definir CFLAGS para:
# - Compilación de debug (-g -O0)
# - Compilación de release (-O2)
# - Compilación con análisis estático (-Wall -Wextra -Werror)

CFLAGS_DEBUG = 
CFLAGS_RELEASE = 
CFLAGS_ANALYSIS = 

# TODO: Implementar targets:
# - debug: compilar con símbolos de debug
# - release: compilar optimizado
# - analyze: compilar con máximos warnings
# - profile: compilar con información de profiling

debug:
	# TODO: Implementar

release:
	# TODO: Implementar

analyze:
	# TODO: Implementar

profile:
	# TODO: Implementar

.PHONY: debug release analyze profile
```

### Ejercicio 3: Script de Debugging Automático (Nivel: Avanzado)

**Objetivo**: Crear un script que automatice el proceso de debugging.

```bash
#!/bin/bash
# Script: auto_debug.sh

# TODO: Completar este script

usage() {
    echo "Uso: $0 [opciones] kernel.elf"
    echo "Opciones:"
    echo "  -b <addr>    Establecer breakpoint en dirección"
    echo "  -f <func>    Establecer breakpoint en función"
    echo "  -t <tiempo>  Timeout para ejecución automática"
    echo "  -l <log>     Archivo de log para output"
}

# TODO: Implementar:
# 1. Parsing de argumentos de línea de comandos
# 2. Inicio automático de QEMU en background
# 3. Conexión automática de GDB
# 4. Configuración de breakpoints automáticos
# 5. Logging de sesión de debugging
# 6. Limpieza automática al salir

# Variables
KERNEL=""
BREAKPOINTS=()
TIMEOUT=0
LOGFILE=""

# TODO: Implementar lógica principal
```

### Ejercicio 4: Análisis Completo de Binario (Nivel: Experto)

**Objetivo**: Crear herramientas de análisis completo del kernel compilado.

```bash
#!/bin/bash
# Script: kernel_analyzer.sh

# TODO: Implementar análisis completo que incluya:

analyze_kernel() {
    local kernel="$1"
    
    echo "=== ANÁLISIS COMPLETO DEL KERNEL ==="
    echo "Archivo: $kernel"
    echo "Fecha: $(date)"
    echo ""
    
    # TODO: 1. Información básica del archivo
    echo "1. INFORMACIÓN BÁSICA"
    # - Tamaño del archivo
    # - Tipo de archivo (verificar que es ELF)
    # - Arquitectura target
    
    # TODO: 2. Análisis de secciones
    echo "2. ANÁLISIS DE SECCIONES"
    # - Listar todas las secciones
    # - Tamaño de cada sección
    # - Porcentaje del total que representa cada sección
    
    # TODO: 3. Análisis de símbolos
    echo "3. ANÁLISIS DE SÍMBOLOS"
    # - Número total de símbolos
    # - Símbolos más grandes
    # - Símbolos no definidos (si los hay)
    
    # TODO: 4. Análisis de código
    echo "4. ANÁLISIS DE CÓDIGO"
    # - Punto de entrada
    # - Funciones principales
    # - Instrucciones más comunes
    
    # TODO: 5. Verificaciones de seguridad
    echo "5. VERIFICACIONES DE SEGURIDAD"
    # - Verificar que no hay stack canaries inadecuados
    # - Verificar alineación de memoria
    # - Verificar que no hay funciones prohibidas
    
    # TODO: 6. Análisis de dependencias
    echo "6. ANÁLISIS DE DEPENDENCIAS"
    # - Listar símbolos externos requeridos
    # - Verificar que son símbolos permitidos para kernels
}

# TODO: Implementar funciones auxiliares
analyze_sections() {
    # Implementar análisis detallado de secciones
}

analyze_symbols() {
    # Implementar análisis detallado de símbolos
}

verify_security() {
    # Implementar verificaciones de seguridad
}

# TODO: Implementar main
main() {
    if [ $# -ne 1 ]; then
        echo "Uso: $0 <kernel.elf>"
        exit 1
    fi
    
    analyze_kernel "$1"
}

main "$@"
```

---

## 📋 Configuraciones Recomendadas

### 3.1 Configuración de Editores

#### Visual Studio Code

```json
// .vscode/settings.json
{
    "C_Cpp.default.compilerPath": "/usr/local/bin/i686-elf-gcc",
    "C_Cpp.default.cStandard": "gnu99",
    "C_Cpp.default.defines": [
        "__KERNEL__",
        "__i386__"
    ],
    "C_Cpp.default.includePath": [
        "${workspaceFolder}/include",
        "${workspaceFolder}/src"
    ],
    "files.associations": {
        "*.ld": "plaintext",
        "*.s": "assembly"
    }
}
```

```json
// .vscode/tasks.json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "Build Kernel",
            "type": "shell",
            "command": "make",
            "group": {
                "kind": "build",
                "isDefault": true
            }
        },
        {
            "label": "Run in QEMU",
            "type": "shell",
            "command": "make run",
            "dependsOn": "Build Kernel"
        },
        {
            "label": "Debug with GDB",
            "type": "shell",
            "command": "make debug",
            "dependsOn": "Build Kernel"
        }
    ]
}
```

#### Vim/Neovim

```vim
" ~/.vimrc additions for OS development

" Configuración para archivos C de kernel
autocmd FileType c setlocal expandtab tabstop=4 shiftwidth=4
autocmd FileType c setlocal cindent
autocmd FileType c setlocal textwidth=80

" Configuración para ensamblador
autocmd FileType asm setlocal expandtab tabstop=8 shiftwidth=8
autocmd FileType asm setlocal syntax=nasm

" Comandos personalizados
command! KBuild !make
command! KRun !make run
command! KDebug !make debug

" Mapeos de teclas
nnoremap <F5> :KBuild<CR>
nnoremap <F6> :KRun<CR>
nnoremap <F7> :KDebug<CR>

" Detección de archivos de linker
autocmd BufRead,BufNewFile *.ld set filetype=ld
```

### 3.2 Scripts de Automatización

#### Script de Setup Completo

```bash
#!/bin/bash
# Script: setup_environment.sh

set -e  # Salir en cualquier error

echo "=== CONFIGURACIÓN DEL ENTORNO DE DESARROLLO DE OS ==="

# Detectar sistema operativo
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    PLATFORM="linux"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    PLATFORM="macos"
elif [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" ]]; then
    PLATFORM="windows"
else
    echo "❌ Plataforma no soportada: $OSTYPE"
    exit 1
fi

echo "Plataforma detectada: $PLATFORM"

# Instalar dependencias según la plataforma
install_dependencies() {
    case $PLATFORM in
        linux)
            echo "Instalando dependencias en Linux..."
            sudo apt update
            sudo apt install -y build-essential gcc-multilib nasm qemu-system-x86 gdb git
            ;;
        macos)
            echo "Instalando dependencias en macOS..."
            if ! command -v brew &> /dev/null; then
                echo "Instalando Homebrew..."
                /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
            fi
            brew install i686-elf-gcc i686-elf-binutils nasm qemu gdb
            ;;
        windows)
            echo "Por favor, usa WSL2 con Ubuntu en Windows"
            echo "Ejecuta: wsl --install -d Ubuntu"
            exit 1
            ;;
    esac
}

# Verificar instalación
verify_installation() {
    echo "Verificando instalación..."
    
    local tools=("i686-elf-gcc" "i686-elf-ld" "nasm" "qemu-system-i386")
    local all_good=true
    
    for tool in "${tools[@]}"; do
        if command -v "$tool" &> /dev/null; then
            echo "✅ $tool: $(command -v "$tool")"
        else
            echo "❌ $tool: No encontrado"
            all_good=false
        fi
    done
    
    if $all_good; then
        echo "✅ Todas las herramientas están instaladas correctamente"
    else
        echo "❌ Algunas herramientas faltan. Verifica la instalación."
        exit 1
    fi
}

# Crear estructura de proyecto
create_project_structure() {
    echo "Creando estructura de proyecto..."
    
    mkdir -p {src,include,build,output,docs,scripts}
    
    # Crear archivos de ejemplo si no existen
    if [[ ! -f "Makefile" ]]; then
        cat > Makefile << 'EOF'
# Makefile básico para kernel
CC = i686-elf-gcc
CFLAGS = -ffreestanding -O2 -Wall -Wextra

kernel.o: kernel.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o

.PHONY: clean
EOF
    fi
    
    echo "✅ Estructura de proyecto creada"
}

# Función principal
main() {
    install_dependencies
    verify_installation
    create_project_structure
    
    echo ""
    echo "🎉 ¡Entorno configurado exitosamente!"
    echo ""
    echo "Próximos pasos:"
    echo "1. Compilar un kernel de prueba"
    echo "2. Ejecutar en QEMU"
    echo "3. Comenzar con el curso de desarrollo de OS"
    echo ""
    echo "Para verificar que todo funciona:"
    echo "  make"
    echo "  make run"
}

main "$@"
```

---

## 📚 Referencia Rápida de Comandos

### Compilación y Build

```bash
# Compilar archivo C para kernel
i686-elf-gcc -ffreestanding -nostdlib -c file.c -o file.o

# Ensamblar archivo ASM
i686-elf-as file.s -o file.o

# Enlazar objetos
i686-elf-ld -T linker.ld -o kernel.elf *.o

# Build completo con Makefile
make clean && make
```

### Ejecución y Testing

```bash
# Ejecutar en QEMU (modo gráfico)
qemu-system-i386 -kernel kernel.elf

# Ejecutar en QEMU (modo texto)
qemu-system-i386 -kernel kernel.elf -nographic

# Ejecutar con más memoria
qemu-system-i386 -kernel kernel.elf -m 64

# Ejecutar con salida serial
qemu-system-i386 -kernel kernel.elf -serial stdio
```

### Debugging

```bash
# Debugging con GDB
qemu-system-i386 -kernel kernel.elf -s -S &
i686-elf-gdb kernel.elf
(gdb) target remote localhost:1234

# Análisis rápido de binario
i686-elf-objdump -d kernel.elf | less
i686-elf-nm kernel.elf | grep main
i686-elf-readelf -h kernel.elf
```

### Análisis y Verificación

```bash
# Ver secciones del ELF
i686-elf-objdump -h kernel.elf

# Ver símbolos
i686-elf-nm kernel.elf

# Desensamblado completo
i686-elf-objdump -d kernel.elf > disasm.txt

# Verificar formato
file kernel.elf
hexdump -C kernel.elf | head
```

---

## ✅ Lista de Verificación Final

Antes de continuar con el curso principal, verifica que puedes:

### Herramientas Básicas
- [ ] Compilar código C con i686-elf-gcc
- [ ] Ensamblar código ASM con i686-elf-as/nasm
- [ ] Enlazar objetos con i686-elf-ld
- [ ] Ejecutar kernel en QEMU
- [ ] Conectar GDB a QEMU

### Build System
- [ ] Escribir Makefiles básicos
- [ ] Usar scripts de linker (.ld)
- [ ] Automatizar build con scripts
- [ ] Limpiar y reconstruir proyecto

### Debugging
- [ ] Compilar con símbolos de debug
- [ ] Usar breakpoints en GDB
- [ ] Inspeccionar memoria y registros
- [ ] Analizar código desensamblado

### Análisis
- [ ] Usar objdump para análisis
- [ ] Verificar símbolos con nm
- [ ] Analizar headers ELF con readelf
- [ ] Inspeccionar binarios con hexdump

---

## 🚀 Siguientes Pasos

¡Perfecto! Ahora que dominas las herramientas, estás listo para:

1. **PRE_CURSO_00.md** - Repasar C y ensamblador si es necesario
2. **CAPITULO_01.md** - Comenzar con arquitectura x86 y bootloaders
3. **Proyecto práctico** - Compilar y ejecutar el mini-kernel educativo

**¡Tienes todas las herramientas para convertirte en un desarrollador de sistemas operativos!** 🛠️⚡

*"Las mejores herramientas no hacen al mejor programador, pero las herramientas incorrectas pueden arruinar al mejor código."*