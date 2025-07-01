# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is an educational x86 operating system kernel written in C and assembly. The project implements a minimal OS with:
- Multiboot-compliant kernel that boots with QEMU
- Custom bootloader (bootsect.asm) 
- FAT16 filesystem implementation in RAM
- Basic shell with file operations
- VGA text mode display
- PS/2 keyboard input

## Architecture

### Boot Process
1. **bootsect.asm**: Legacy BIOS bootloader (512 bytes) that loads kernel from disk
2. **boot.s**: Multiboot header + protected mode setup + kernel entry point
3. **kernel.c**: Main kernel implementation with filesystem and shell

### Memory Layout (linker.ld)
- **DISK region**: 0x00000000-0x00024200 (289 sectors × 512 bytes for filesystem image)
- **KERNEL region**: 0x00200000+ (2MB+, Multiboot standard)

### Filesystem
- FAT16 implementation entirely in RAM (disk_image array)
- 289 total sectors: 1 boot + 256 FAT + 32 root directory entries + data
- Supports basic file operations: ls, cat, touch, cp, mv, copycon

### Shell Commands
- `ls` - list files
- `cat <file>` - display file contents  
- `touch <file>` - create empty file
- `cp <src> <dst>` - copy file
- `mv <old> <new>` - rename file
- `copycon <file>` - create file from keyboard input (Ctrl+Z to end)
- `echo <text>` - print text
- `clear` - clear screen
- `free` - show memory usage
- `help` - show commands

## Development Commands

### Build System
```bash
# Build the kernel
make

# Build and run in QEMU
make run

# Clean build artifacts
make clean
```

### Cross-Compilation Requirements
The Makefile expects i686-elf cross-compilation tools:
- `i686-elf-gcc` - C compiler
- `i686-elf-as` - Assembler  
- `i686-elf-ld` - Linker
- `i686-elf-objcopy` - Object file converter

### QEMU Execution
- Uses `qemu-system-i386` with `-kernel` flag
- 32MB RAM allocation
- No graphics mode (nographic)
- Boots directly to kernel via Multiboot

## Key Implementation Details

### Custom libc Functions
The kernel implements its own minimal C library functions (memcpy, memset, strlen, etc.) since it runs in freestanding mode without standard library.

### VGA Text Mode
- 80x25 character display at 0xB8000
- Hardware cursor control via VGA registers (ports 0x3D4/0x3D5)
- Simple printf implementation supporting %s, %c, %d, %x format specifiers

### Keyboard Input
- PS/2 keyboard via ports 0x60/0x64
- Scancode to ASCII translation table
- Blocking getchar implementation

### File System Operations
- All filesystem data stored in memory (disk_image array)
- FAT16 directory entries with 8.3 filename format
- Single cluster per file (simplified implementation)
- Files stored starting at cluster 2 in contiguous sectors

## Testing
The kernel can be tested by:
1. Running `make run` to boot in QEMU
2. Using shell commands to create/manipulate files
3. Verifying filesystem operations work correctly
4. Testing keyboard input and display output

## Debugging
- Kernel prints boot message on successful initialization
- Shell prompt indicates successful kernel load
- VGA text output provides immediate feedback
- QEMU console shows kernel output directly