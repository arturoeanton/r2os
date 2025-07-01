# boot.s: Entrada Multiboot para el kernel

# =============================================================================
# HEADER MULTIBOOT - Debe estar al inicio absoluto del binario
# =============================================================================

# Definir constantes para el header Multiboot
.set ALIGN,    1<<0                  # Alinear módulos cargados en límites de página
.set MEMINFO,  1<<1                  # Proporcionar mapa de memoria
.set FLAGS,    ALIGN | MEMINFO       # Campo de flags Multiboot
.set MAGIC,    0x1BADB002            # Número mágico que identifica el header
.set CHECKSUM, -(MAGIC + FLAGS)      # Checksum para validar el header

# Declarar el header Multiboot al inicio del kernel
.section .multiboot
.align 4
.long MAGIC
.long FLAGS  
.long CHECKSUM

# =============================================================================
# CÓDIGO DE ENTRADA DEL KERNEL
# =============================================================================

.section .text
.global _start
.type _start, @function

_start:
    # Configurar la pila del kernel
    mov $stack_top, %esp
    
    # Inicializar la imagen del disco con ceros (filesystem vacío)
    cld                          # Dirección hacia adelante para string ops
    mov $disk_image_start, %edi  # EDI = inicio del area de disco
    mov $0, %eax                 # EAX = 0 (patrón a escribir)
    mov $37088, %ecx             # ECX = (512 * 289 / 4) = número de dwords
    rep stosl                    # Llenar con ceros
    
    # Inicializar el Boot Sector del filesystem (sector 0)
    mov $disk_image_start, %edi
    movb $0xEB, (%edi)          # Jump instruction (típico de boot sectors)
    movb $0x00, 1(%edi)         # Completar el jump
    movb $0x90, 2(%edi)         # NOP
    
    # Escribir signature de boot sector válido
    mov $disk_image_start, %edi
    add $510, %edi              # Posición del signature (offset 510)
    movw $0xAA55, (%edi)        # Boot signature
    
    # Llamar a la función principal del kernel (escrita en C)
    call kernel_main
    
    # Si kernel_main retorna (no debería), hacer un bucle infinito
    cli                         # Deshabilitar interrupciones
halt_loop:
    hlt                         # Halt hasta próxima interrupción
    jmp halt_loop               # Repetir para siempre

.size _start, . - _start

# =============================================================================
# RESERVA DE PILA (STACK)
# =============================================================================

.section .bss
.align 16
stack_bottom:
.skip 16384      # Reservar 16 KiB para la pila
stack_top:

# =============================================================================
# DATOS DEL SISTEMA DE ARCHIVOS
# =============================================================================

.section .disk_image
.align 4096
.global disk_image_start
disk_image_start:
.skip 512 * 289  # 289 sectores × 512 bytes = espacio para el filesystem
disk_image_end:
