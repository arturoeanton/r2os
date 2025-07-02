# Makefile para Mi Mini OS

# Herramientas de Cross-Compilación
PREFIX   := i686-elf
CC       := $(PREFIX)-gcc
AS       := $(PREFIX)-as
LD       := $(PREFIX)-ld
OBJCOPY  := $(PREFIX)-objcopy

# Flags de compilación y enlazado
# -ffreestanding: No asumir una biblioteca estándar.
# -nostdlib: No enlazar con la biblioteca estándar.
# -fno-builtin: No usar builtins que puedan requerir libc.
# -O2: Optimización.
# -Wall -Wextra: Todas las advertencias.
# -g: Símbolos de depuración.
CFLAGS   := -std=gnu99 -ffreestanding -nostdlib -fno-builtin -O2 -Wall -Wextra -g
ASFLAGS  := -g
LDFLAGS  := -T linker.ld -nostdlib
QEMU     := qemu-system-i386
QEMUFLAGS:= -kernel myos.elf -m 32 -nographic

# Archivos fuente y objeto
OBJS := boot.o kernel.o

# Target por defecto
all: myos.bin

# Regla para crear el binario final plano. CRÍTICO para QEMU -kernel.
# Extrae las secciones en el orden correcto para el header Multiboot.
myos.bin: myos.elf
	$(OBJCOPY) -O binary $< $@

# Regla para enlazar los objetos en un archivo ELF.
myos.elf: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

# Regla para compilar el ensamblador
boot.o: boot.s
	$(AS) $(ASFLAGS) -o $@ $<

# Regla para compilar el código C
kernel.o: kernel.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Regla para ejecutar el OS en QEMU
run: myos.elf
	$(QEMU) $(QEMUFLAGS)

# Regla para ejecutar con más debugging
debug: myos.elf
	$(QEMU) -kernel myos.elf -m 32 -nographic -d int,cpu_reset -no-reboot

# Regla para ejecutar con monitor QEMU
monitor: myos.elf
	$(QEMU) -kernel myos.elf -m 32 -monitor stdio

# Regla para ejecutar sin -nographic (con ventana)
run-gui: myos.elf
	$(QEMU) -kernel myos.elf -m 32

# Regla para ejecutar con salida serial para debugging
run-serial: myos.elf
	$(QEMU) -kernel myos.elf -m 32 -display none -serial stdio

# Regla para limpiar archivos generados
clean:
	rm -f *.o *.elf *.bin

# Phony targets
.PHONY: all clean run