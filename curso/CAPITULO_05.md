# Capítulo 5: Interrupciones y Excepciones

## 📚 Introducción

Este capítulo explora uno de los mecanismos más fundamentales de los sistemas operativos modernos: el manejo de interrupciones y excepciones. Estas tecnologías permiten que el procesador responda de manera eficiente a eventos externos e internos, constituyendo la base de la multitarea, la E/O asíncrona y el manejo de errores.

## 🎯 Objetivos de Aprendizaje

Al completar este capítulo, el estudiante será capaz de:

1. **Explicar** la diferencia entre interrupciones, excepciones y traps
2. **Implementar** handlers básicos de interrupciones en x86
3. **Configurar** el Interrupt Descriptor Table (IDT)
4. **Diseñar** sistemas de manejo de eventos asíncronos
5. **Optimizar** el rendimiento del sistema mediante interrupciones eficientes

---

## 🏛️ Parte Teórica

### 1.1 Fundamentos de Interrupciones

#### ¿Qué son las Interrupciones?

Las interrupciones son señales que indican al procesador que debe suspender temporalmente la ejecución del programa actual para atender un evento específico. Son el mecanismo fundamental que permite a los sistemas operativos ser **reactivos** y **multitarea**.

```
┌─────────────────────────────────────────────────┐
│            FLUJO NORMAL DE EJECUCIÓN            │
│                                                 │
│  Instrucción 1 → Instrucción 2 → Instrucción 3 │
│         ↓              ↑                        │
│    INTERRUPCIÓN    RETORNO                      │
│         ↓              ↑                        │
│  ┌─────────────────────────┐                    │
│  │    HANDLER DE           │                    │
│  │    INTERRUPCIÓN         │                    │
│  │                         │                    │
│  │  1. Guardar contexto    │                    │
│  │  2. Atender evento      │                    │
│  │  3. Restaurar contexto  │                    │
│  └─────────────────────────┘                    │
└─────────────────────────────────────────────────┘
```

#### Tipos de Interrupciones

**1. Interrupciones Hardware (IRQ - Interrupt Requests)**
```
┌─────────────────────┬─────────────────────┬─────────────────────┐
│       Fuente        │        IRQ          │     Descripción     │
├─────────────────────┼─────────────────────┼─────────────────────┤
│ Timer/PIT           │ IRQ 0               │ Interrupción timer  │
│ Teclado             │ IRQ 1               │ Tecla presionada    │
│ Cascade (PIC2)      │ IRQ 2               │ Cascada de PICs     │
│ COM2/COM4           │ IRQ 3               │ Puerto serial       │
│ COM1/COM3           │ IRQ 4               │ Puerto serial       │
│ LPT2                │ IRQ 5               │ Puerto paralelo     │
│ Floppy              │ IRQ 6               │ Disquetera          │
│ LPT1                │ IRQ 7               │ Puerto paralelo     │
│ RTC                 │ IRQ 8               │ Reloj tiempo real   │
│ Libre               │ IRQ 9               │ Disponible          │
│ Libre               │ IRQ 10              │ Disponible          │
│ Libre               │ IRQ 11              │ Disponible          │
│ Mouse PS/2          │ IRQ 12              │ Mouse               │
│ FPU                 │ IRQ 13              │ Coprocesador        │
│ IDE Primario        │ IRQ 14              │ Disco duro          │
│ IDE Secundario      │ IRQ 15              │ Disco duro          │
└─────────────────────┴─────────────────────┴─────────────────────┘
```

**2. Excepciones (Processor-Generated)**
```
┌──────────────────┬─────────┬─────────────────────────────────────┐
│   Excepción      │ Vector  │             Descripción             │
├──────────────────┼─────────┼─────────────────────────────────────┤
│ Divide Error     │    0    │ División por cero                   │
│ Debug            │    1    │ Debugging                           │
│ NMI              │    2    │ Non-Maskable Interrupt              │
│ Breakpoint       │    3    │ Instrucción INT 3                   │
│ Overflow         │    4    │ Overflow aritmético                 │
│ Bounds           │    5    │ Índice fuera de límites             │
│ Invalid Opcode   │    6    │ Instrucción inválida                │
│ Device N/A       │    7    │ Coprocesador no disponible          │
│ Double Fault     │    8    │ Fallo doble                         │
│ Invalid TSS      │   10    │ Task State Segment inválido         │
│ Segment N/P      │   11    │ Segmento no presente                │
│ Stack Fault      │   12    │ Fallo de stack                      │
│ General Prot.    │   13    │ Violación de protección general     │
│ Page Fault       │   14    │ Fallo de página                     │
│ FPU Error        │   16    │ Error de punto flotante             │
│ Alignment        │   17    │ Error de alineación                 │
│ Machine Check    │   18    │ Error de máquina                    │
│ SIMD FP          │   19    │ Error SIMD punto flotante           │
└──────────────────┴─────────┴─────────────────────────────────────┘
```

**3. Software Interrupts (System Calls)**
```c
// Interrupciones de software - System calls
INT 0x80    // System call tradicional en Linux
SYSCALL     // System call rápido en x86-64
SYSENTER    // System call rápido en x86-32
```

### 1.2 Interrupt Descriptor Table (IDT)

#### Estructura del IDT

El IDT es una tabla que mapea cada vector de interrupción a su handler correspondiente:

```c
// Descriptor de interrupción en x86
struct idt_entry {
    uint16_t offset_low;    // Offset bits 0-15
    uint16_t selector;      // Selector de segmento de código
    uint8_t  zero;         // Reservado (debe ser 0)
    uint8_t  type_attr;    // Tipo y atributos
    uint16_t offset_high;   // Offset bits 16-31
} __attribute__((packed));

// Registro IDTR que apunta al IDT
struct idt_register {
    uint16_t limit;        // Tamaño del IDT - 1
    uint32_t base;         // Dirección base del IDT
} __attribute__((packed));
```

#### Tipos de Descriptores

```
Tipo de Gate    | Tipo  | Descripción
----------------|-------|------------------------------------------
Task Gate       | 0101  | Cambio de tarea (obsoleto)
Interrupt Gate  | 1110  | Handler de interrupción (deshabilita IF)
Trap Gate       | 1111  | Handler de excepción (mantiene IF)
```

#### Atributos de Descriptores

```
Bit 7 | Bit 6-5 | Bit 4 | Bit 3-0
------|---------|-------|--------
  P   |   DPL   |   S   |  TYPE
```

- **P (Present)**: Descriptor presente (1) o no (0)
- **DPL (Descriptor Privilege Level)**: Nivel de privilegio (0-3)
- **S (Storage Segment)**: Debe ser 0 para gates
- **TYPE**: Tipo de gate (ver tabla anterior)

### 1.3 Programmable Interrupt Controller (PIC)

#### Arquitectura del PIC 8259A

```
        ┌─────────────────┐    ┌─────────────────┐
        │    PIC Master   │    │    PIC Slave    │
        │     (8259A)     │    │     (8259A)     │
        └─────────────────┘    └─────────────────┘
                 │                       │
         ┌───────┼───────┐               │
         │       │       │               │
    IRQ0─┤  IRQ1─┤  IRQ2─┼───────────────┤
    IRQ3─┤  IRQ4─┤  IRQ5─┤          IRQ8─┤
    IRQ6─┤  IRQ7─┤       │          IRQ9─┤
         │       │       │         IRQ10─┤
         └───────┼───────┘         IRQ11─┤
                 │                 IRQ12─┤
                 │                 IRQ13─┤
                 │                 IRQ14─┤
                 │                 IRQ15─┤
                 │                       │
                 └───► CPU INT Pin        │
                                         │
           Ports: 0x20, 0x21    Ports: 0xA0, 0xA1
```

#### Configuración del PIC

```c
// Comandos de inicialización del PIC
#define PIC1_COMMAND    0x20
#define PIC1_DATA       0x21
#define PIC2_COMMAND    0xA0
#define PIC2_DATA       0xA1

#define ICW1_INIT       0x10    // Comando de inicialización
#define ICW1_ICW4       0x01    // ICW4 será enviado
#define ICW4_8086       0x01    // Modo 8086/88

// Inicialización del PIC
void pic_init(uint8_t offset1, uint8_t offset2) {
    // Guardar máscaras actuales
    uint8_t mask1 = inb(PIC1_DATA);
    uint8_t mask2 = inb(PIC2_DATA);
    
    // Inicializar PIC1
    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    outb(PIC1_DATA, offset1);           // ICW2: Vector offset
    outb(PIC1_DATA, 0x04);              // ICW3: Slave en IRQ2
    outb(PIC1_DATA, ICW4_8086);         // ICW4: Modo 8086
    
    // Inicializar PIC2
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    outb(PIC2_DATA, offset2);           // ICW2: Vector offset
    outb(PIC2_DATA, 0x02);              // ICW3: Cascada identity
    outb(PIC2_DATA, ICW4_8086);         // ICW4: Modo 8086
    
    // Restaurar máscaras
    outb(PIC1_DATA, mask1);
    outb(PIC2_DATA, mask2);
}
```

### 1.4 Advanced Programmable Interrupt Controller (APIC)

#### Evolución del PIC al APIC

```
PIC 8259A (Legacy)          →        Local APIC (Modern)
─────────────────────────────────────────────────────────
• 16 IRQ lines                      • 240+ interrupt vectors
• Edge-triggered only               • Edge and level triggered
• Fixed priority                    • Programmable priority
• No SMP support                    • Full SMP support
• 8-bit vector numbers              • 8-bit vector numbers
• No inter-processor interrupts    • Inter-processor interrupts
• No local timer                   • Built-in local timer
```

#### Componentes del Sistema APIC

**1. Local APIC (En cada CPU):**
```c
#define APIC_BASE_MSR       0x1B
#define APIC_BASE_ADDRESS   0xFEE00000

// Registros importantes del Local APIC
#define APIC_ID             0x020   // ID del procesador
#define APIC_LVR            0x030   // Version register
#define APIC_TPR            0x080   // Task priority register
#define APIC_EOI            0x0B0   // End of interrupt
#define APIC_LDR            0x0D0   // Logical destination
#define APIC_SIVR           0x0F0   // Spurious interrupt vector
#define APIC_ICR_LOW        0x300   // Interrupt command (low)
#define APIC_ICR_HIGH       0x310   // Interrupt command (high)
#define APIC_TIMER_LVT      0x320   // Timer local vector table
#define APIC_TIMER_INITIAL  0x380   // Timer initial count
#define APIC_TIMER_CURRENT  0x390   // Timer current count
#define APIC_TIMER_DIVIDE   0x3E0   // Timer divide configuration
```

**2. I/O APIC (Para dispositivos externos):**
```c
#define IOAPIC_BASE         0xFEC00000

// Registros del I/O APIC
#define IOAPIC_REGSEL       0x00    // Register select
#define IOAPIC_REGWIN       0x10    // Register window

// Registros direccionables
#define IOAPIC_ID           0x00    // ID del I/O APIC
#define IOAPIC_VER          0x01    // Version
#define IOAPIC_ARB          0x02    // Arbitration
#define IOAPIC_REDTBL       0x10    // Redirection table base
```

---

## 🔧 Parte Práctica

### 2.1 Implementación Básica del IDT

#### Configuración del IDT en el Mini-Kernel

```c
// Estructura para entradas del IDT
struct idt_entry {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t  zero;
    uint8_t  type_attr;
    uint16_t offset_high;
} __attribute__((packed));

// Estructura para el registro IDTR
struct idt_register {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

// Tabla IDT (256 entradas)
static struct idt_entry idt[256];
static struct idt_register idtr;

// Configurar una entrada del IDT
void idt_set_gate(uint8_t num, uint32_t offset, uint16_t selector, 
                  uint8_t type_attr) {
    idt[num].offset_low = offset & 0xFFFF;
    idt[num].offset_high = (offset >> 16) & 0xFFFF;
    idt[num].selector = selector;
    idt[num].zero = 0;
    idt[num].type_attr = type_attr;
}

// Inicializar el IDT
void idt_init(void) {
    // Configurar el registro IDTR
    idtr.limit = (sizeof(struct idt_entry) * 256) - 1;
    idtr.base = (uint32_t)&idt;
    
    // Limpiar todas las entradas
    memset(&idt, 0, sizeof(struct idt_entry) * 256);
    
    // Configurar excepciones del procesador
    idt_set_gate(0, (uint32_t)divide_error_handler, 0x08, 0x8E);
    idt_set_gate(1, (uint32_t)debug_handler, 0x08, 0x8E);
    idt_set_gate(2, (uint32_t)nmi_handler, 0x08, 0x8E);
    idt_set_gate(3, (uint32_t)breakpoint_handler, 0x08, 0x8E);
    idt_set_gate(4, (uint32_t)overflow_handler, 0x08, 0x8E);
    idt_set_gate(5, (uint32_t)bounds_handler, 0x08, 0x8E);
    idt_set_gate(6, (uint32_t)invalid_opcode_handler, 0x08, 0x8E);
    idt_set_gate(7, (uint32_t)device_na_handler, 0x08, 0x8E);
    idt_set_gate(8, (uint32_t)double_fault_handler, 0x08, 0x8E);
    idt_set_gate(10, (uint32_t)invalid_tss_handler, 0x08, 0x8E);
    idt_set_gate(11, (uint32_t)segment_not_present_handler, 0x08, 0x8E);
    idt_set_gate(12, (uint32_t)stack_fault_handler, 0x08, 0x8E);
    idt_set_gate(13, (uint32_t)general_protection_handler, 0x08, 0x8E);
    idt_set_gate(14, (uint32_t)page_fault_handler, 0x08, 0x8E);
    idt_set_gate(16, (uint32_t)fpu_error_handler, 0x08, 0x8E);
    idt_set_gate(17, (uint32_t)alignment_check_handler, 0x08, 0x8E);
    idt_set_gate(18, (uint32_t)machine_check_handler, 0x08, 0x8E);
    idt_set_gate(19, (uint32_t)simd_exception_handler, 0x08, 0x8E);
    
    // Configurar interrupciones hardware (IRQs)
    idt_set_gate(32, (uint32_t)timer_handler, 0x08, 0x8E);      // IRQ0
    idt_set_gate(33, (uint32_t)keyboard_handler, 0x08, 0x8E);   // IRQ1
    idt_set_gate(34, (uint32_t)cascade_handler, 0x08, 0x8E);    // IRQ2
    idt_set_gate(35, (uint32_t)com2_handler, 0x08, 0x8E);       // IRQ3
    idt_set_gate(36, (uint32_t)com1_handler, 0x08, 0x8E);       // IRQ4
    // ... más IRQs según necesidad
    
    // Cargar el IDT
    asm volatile ("lidt %0" : : "m" (idtr));
    
    printf("IDT inicializado correctamente\n");
}
```

### 2.2 Handlers de Interrupciones

#### Template Básico de Handler

```assembly
# Macro para handlers que NO pushean error code
.macro ISR_NOERROR num
.global isr\num
isr\num:
    push $0          # Push dummy error code
    push $\num       # Push interrupt number
    jmp isr_common   # Jump to common handler
.endm

# Macro para handlers que SÍ pushean error code
.macro ISR_ERROR num
.global isr\num
isr\num:
    push $\num       # Push interrupt number
    jmp isr_common   # Jump to common handler
.endm

# Handlers de excepciones
ISR_NOERROR 0    # Divide by zero
ISR_NOERROR 1    # Debug
ISR_NOERROR 2    # Non-maskable interrupt
ISR_NOERROR 3    # Breakpoint
ISR_NOERROR 4    # Overflow
ISR_NOERROR 5    # Bounds
ISR_NOERROR 6    # Invalid opcode
ISR_NOERROR 7    # Device not available
ISR_ERROR   8    # Double fault (pushes error code)
ISR_NOERROR 9    # Coprocessor segment overrun
ISR_ERROR   10   # Invalid TSS
ISR_ERROR   11   # Segment not present
ISR_ERROR   12   # Stack fault
ISR_ERROR   13   # General protection fault
ISR_ERROR   14   # Page fault
ISR_NOERROR 15   # Reserved
ISR_NOERROR 16   # FPU error
ISR_ERROR   17   # Alignment check
ISR_NOERROR 18   # Machine check
ISR_NOERROR 19   # SIMD exception

# Handler común que salva/restaura contexto
isr_common:
    # Guardar todos los registros
    pusha
    
    # Guardar registros de segmento
    push %ds
    push %es
    push %fs
    push %gs
    
    # Cargar segmento de datos del kernel
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    
    # Llamar al handler en C
    push %esp       # Pasar puntero al contexto
    call interrupt_handler
    add $4, %esp    # Limpiar stack
    
    # Restaurar registros de segmento
    pop %gs
    pop %fs
    pop %es
    pop %ds
    
    # Restaurar registros generales
    popa
    
    # Limpiar error code e interrupt number
    add $8, %esp
    
    # Retornar de la interrupción
    iret
```

#### Handler Principal en C

```c
// Estructura para el contexto de la interrupción
struct interrupt_context {
    uint32_t gs, fs, es, ds;                    // Segmentos
    uint32_t edi, esi, ebp, esp;                // Registros generales
    uint32_t ebx, edx, ecx, eax;                // Registros generales
    uint32_t interrupt_number, error_code;      // Info de la interrupción
    uint32_t eip, cs, eflags, user_esp, ss;     // Contexto del procesador
};

// Handler principal de interrupciones
void interrupt_handler(struct interrupt_context *ctx) {
    if (ctx->interrupt_number < 32) {
        // Excepción del procesador
        handle_exception(ctx);
    } else if (ctx->interrupt_number < 48) {
        // Interrupción hardware (IRQ)
        handle_irq(ctx);
    } else {
        // Interrupción de software
        handle_software_interrupt(ctx);
    }
}

// Handler de excepciones
void handle_exception(struct interrupt_context *ctx) {
    const char *exception_messages[] = {
        "Division By Zero",
        "Debug",
        "Non Maskable Interrupt",
        "Breakpoint",
        "Into Detected Overflow",
        "Out of Bounds",
        "Invalid Opcode",
        "No Coprocessor",
        "Double Fault",
        "Coprocessor Segment Overrun",
        "Bad TSS",
        "Segment Not Present",
        "Stack Fault",
        "General Protection Fault",
        "Page Fault",
        "Unknown Interrupt",
        "Coprocessor Fault",
        "Alignment Check",
        "Machine Check",
        "SIMD Floating-Point Exception"
    };
    
    printf("EXCEPCIÓN: %s (0x%X) en EIP: 0x%X\n", 
           exception_messages[ctx->interrupt_number],
           ctx->interrupt_number,
           ctx->eip);
    
    if (ctx->error_code != 0) {
        printf("Código de error: 0x%X\n", ctx->error_code);
    }
    
    // Handler específico según la excepción
    switch (ctx->interrupt_number) {
        case 0:  // Division by zero
            handle_divide_error(ctx);
            break;
        case 14: // Page fault
            handle_page_fault(ctx);
            break;
        case 13: // General protection fault
            handle_general_protection_fault(ctx);
            break;
        default:
            // Para excepciones no manejadas, detener el sistema
            printf("Excepción no manejada - Sistema detenido\n");
            asm volatile ("cli; hlt");
            break;
    }
}
```

### 2.3 Manejo de IRQs Hardware

#### Handler de Timer (IRQ0)

```c
// Variables globales para el timer
static volatile uint32_t timer_ticks = 0;
static uint32_t timer_frequency = 1000; // 1000 Hz = 1ms por tick

// Handler del timer
void timer_handler(struct interrupt_context *ctx) {
    timer_ticks++;
    
    // Cada 1000 ticks (1 segundo) mostrar mensaje
    if (timer_ticks % 1000 == 0) {
        printf("Timer: %d segundos\n", timer_ticks / 1000);
    }
    
    // Aquí se puede implementar el scheduler
    // schedule_next_task();
    
    // Enviar EOI al PIC
    pic_send_eoi(0);
}

// Configurar la frecuencia del timer
void timer_set_frequency(uint32_t frequency) {
    timer_frequency = frequency;
    uint32_t divisor = 1193180 / frequency;
    
    // Configurar el PIT (Programmable Interval Timer)
    outb(0x43, 0x36);                          // Command byte
    outb(0x40, divisor & 0xFF);                // Low byte
    outb(0x40, (divisor >> 8) & 0xFF);         // High byte
}

// Enviar End of Interrupt al PIC
void pic_send_eoi(uint8_t irq) {
    if (irq >= 8) {
        outb(PIC2_COMMAND, 0x20);  // EOI al PIC slave
    }
    outb(PIC1_COMMAND, 0x20);      // EOI al PIC master
}
```

#### Handler de Teclado Mejorado (IRQ1)

```c
// Buffer circular para el teclado
#define KEYBOARD_BUFFER_SIZE 256
static char keyboard_buffer[KEYBOARD_BUFFER_SIZE];
static volatile int keyboard_head = 0;
static volatile int keyboard_tail = 0;
static volatile int keyboard_count = 0;

// Handler del teclado
void keyboard_handler(struct interrupt_context *ctx) {
    uint8_t scancode = inb(0x60);
    
    // Procesar scancode
    char ascii = scancode_to_ascii(scancode);
    if (ascii != 0) {
        // Agregar al buffer si hay espacio
        if (keyboard_count < KEYBOARD_BUFFER_SIZE - 1) {
            keyboard_buffer[keyboard_head] = ascii;
            keyboard_head = (keyboard_head + 1) % KEYBOARD_BUFFER_SIZE;
            keyboard_count++;
        }
    }
    
    // Enviar EOI
    pic_send_eoi(1);
}

// Función no-bloqueante para leer del teclado
int keyboard_getchar_nonblocking(void) {
    if (keyboard_count == 0) {
        return -1;  // No hay caracteres disponibles
    }
    
    char c = keyboard_buffer[keyboard_tail];
    keyboard_tail = (keyboard_tail + 1) % KEYBOARD_BUFFER_SIZE;
    keyboard_count--;
    
    return c;
}

// Función bloqueante para leer del teclado
char keyboard_getchar_blocking(void) {
    int c;
    while ((c = keyboard_getchar_nonblocking()) == -1) {
        // Esperar hasta que haya un carácter
        asm volatile ("hlt");  // Dormir hasta la próxima interrupción
    }
    return (char)c;
}
```

### 2.4 System Calls Básicos

#### Implementación de INT 0x80

```c
// Números de system calls
#define SYS_EXIT        1
#define SYS_WRITE       4
#define SYS_READ        3
#define SYS_OPEN        5
#define SYS_CLOSE       6

// Handler de system calls
void syscall_handler(struct interrupt_context *ctx) {
    uint32_t syscall_number = ctx->eax;
    uint32_t arg1 = ctx->ebx;
    uint32_t arg2 = ctx->ecx;
    uint32_t arg3 = ctx->edx;
    uint32_t arg4 = ctx->esi;
    uint32_t arg5 = ctx->edi;
    
    uint32_t result = 0;
    
    switch (syscall_number) {
        case SYS_EXIT:
            printf("Proceso terminado con código: %d\n", arg1);
            // Aquí se implementaría la terminación del proceso
            break;
            
        case SYS_WRITE:
            // arg1 = fd, arg2 = buffer, arg3 = count
            if (arg1 == 1) {  // stdout
                char *buffer = (char *)arg2;
                for (uint32_t i = 0; i < arg3; i++) {
                    putchar(buffer[i]);
                }
                result = arg3;
            }
            break;
            
        case SYS_READ:
            // arg1 = fd, arg2 = buffer, arg3 = count
            if (arg1 == 0) {  // stdin
                char *buffer = (char *)arg2;
                for (uint32_t i = 0; i < arg3; i++) {
                    buffer[i] = keyboard_getchar_blocking();
                    if (buffer[i] == '\n') {
                        result = i + 1;
                        break;
                    }
                }
            }
            break;
            
        default:
            printf("System call no implementado: %d\n", syscall_number);
            result = -1;
            break;
    }
    
    // Retornar resultado en EAX
    ctx->eax = result;
}
```

---

## 📝 Ejercicios Prácticos

### Ejercicio 1: Timer Programable
**Dificultad**: Intermedia

**Objetivo**: Implementar un sistema de timers programables para el kernel.

**Instrucciones**:
1. Crear estructura para timers:
```c
struct kernel_timer {
    uint32_t interval_ms;     // Intervalo en milisegundos
    uint32_t remaining_ms;    // Tiempo restante
    void (*callback)(void*);  // Función a llamar
    void *data;              // Datos para el callback
    int active;              // Timer activo/inactivo
    struct kernel_timer *next; // Lista enlazada
};
```

2. Implementar funciones de gestión:
```c
int timer_create(uint32_t interval_ms, void (*callback)(void*), void *data);
void timer_destroy(int timer_id);
void timer_update(void);  // Llamar desde timer_handler
```

3. Ejemplo de uso:
```c
void heartbeat_callback(void *data) {
    printf("Heartbeat del sistema\n");
}

// Crear timer que ejecute cada 5 segundos
int heartbeat_timer = timer_create(5000, heartbeat_callback, NULL);
```

---

### Ejercicio 2: Sistema de Señales
**Dificultad**: Avanzada

**Objetivo**: Implementar un sistema básico de señales para comunicación entre procesos.

**Instrucciones**:
1. Definir tipos de señales:
```c
#define SIGKILL  9
#define SIGTERM  15
#define SIGUSR1  10
#define SIGUSR2  12

typedef void (*signal_handler_t)(int);
```

2. Implementar funciones de señales:
```c
signal_handler_t signal(int sig, signal_handler_t handler);
int kill(int pid, int sig);
void signal_deliver(int pid, int sig);
```

3. Integrar con el sistema de interrupciones:
```c
// En timer_handler, verificar señales pendientes
void check_pending_signals(void) {
    // TODO: Verificar y entregar señales pendientes
}
```

---

### Ejercicio 3: Profiler de Interrupciones
**Dificultad**: Muy Avanzada

**Objetivo**: Crear un profiler que mida el tiempo gastado en cada tipo de interrupción.

**Instrucciones**:
1. Estructura para estadísticas:
```c
struct interrupt_stats {
    uint64_t count;          // Número de veces llamado
    uint64_t total_cycles;   // Ciclos totales gastados
    uint64_t max_cycles;     // Máximo tiempo en una llamada
    uint64_t min_cycles;     // Mínimo tiempo en una llamada
};

static struct interrupt_stats irq_stats[256];
```

2. Instrumentar handlers:
```c
uint64_t rdtsc(void) {
    uint32_t low, high;
    asm volatile ("rdtsc" : "=a" (low), "=d" (high));
    return ((uint64_t)high << 32) | low;
}

void profiled_interrupt_handler(struct interrupt_context *ctx) {
    uint64_t start = rdtsc();
    
    // Llamar al handler original
    interrupt_handler(ctx);
    
    uint64_t end = rdtsc();
    uint64_t cycles = end - start;
    
    // Actualizar estadísticas
    int irq = ctx->interrupt_number;
    irq_stats[irq].count++;
    irq_stats[irq].total_cycles += cycles;
    if (cycles > irq_stats[irq].max_cycles) {
        irq_stats[irq].max_cycles = cycles;
    }
    if (irq_stats[irq].min_cycles == 0 || cycles < irq_stats[irq].min_cycles) {
        irq_stats[irq].min_cycles = cycles;
    }
}
```

3. Comando para mostrar estadísticas:
```c
void show_interrupt_stats(void) {
    printf("IRQ | Count    | Avg Cycles | Max Cycles | Min Cycles\n");
    printf("----|----------|------------|------------|------------\n");
    for (int i = 0; i < 256; i++) {
        if (irq_stats[i].count > 0) {
            uint64_t avg = irq_stats[i].total_cycles / irq_stats[i].count;
            printf("%3d | %8llu | %10llu | %10llu | %10llu\n",
                   i, irq_stats[i].count, avg,
                   irq_stats[i].max_cycles, irq_stats[i].min_cycles);
        }
    }
}
```

---

### Ejercicio 4: Driver de Mouse PS/2
**Dificultad**: Extrema

**Objetivo**: Implementar un driver completo para mouse PS/2 con interrupciones.

**Instrucciones**:
1. Configurar mouse PS/2:
```c
#define MOUSE_IRQ           12
#define MOUSE_COMMAND_PORT  0x64
#define MOUSE_DATA_PORT     0x60

void mouse_init(void) {
    // Habilitar mouse PS/2
    ps2_write_command(0xA8);  // Enable aux device
    ps2_write_command(0x20);  // Get config
    uint8_t config = ps2_read_data();
    config |= 0x02;           // Enable IRQ12
    config &= ~0x20;          // Enable mouse clock
    ps2_write_command(0x60);  // Set config
    ps2_write_data(config);
    
    // Configurar mouse
    mouse_write(0xF6);        // Set defaults
    mouse_write(0xF4);        // Enable data reporting
    
    // Registrar handler
    idt_set_gate(32 + MOUSE_IRQ, (uint32_t)mouse_handler, 0x08, 0x8E);
}
```

2. Implementar protocolo de 3 bytes:
```c
struct mouse_packet {
    uint8_t flags;
    int16_t delta_x;
    int16_t delta_y;
    uint8_t buttons;
};

static uint8_t mouse_bytes[3];
static int mouse_byte_index = 0;

void mouse_handler(struct interrupt_context *ctx) {
    uint8_t data = inb(MOUSE_DATA_PORT);
    
    // Primer byte debe tener bit 3 set
    if (mouse_byte_index == 0 && !(data & 0x08)) {
        return;  // Resincronizar
    }
    
    mouse_bytes[mouse_byte_index] = data;
    mouse_byte_index++;
    
    if (mouse_byte_index == 3) {
        // Paquete completo
        struct mouse_packet packet;
        parse_mouse_packet(mouse_bytes, &packet);
        handle_mouse_event(&packet);
        mouse_byte_index = 0;
    }
    
    pic_send_eoi(MOUSE_IRQ);
}
```

3. Procesar eventos del mouse:
```c
void handle_mouse_event(struct mouse_packet *packet) {
    static int mouse_x = 40, mouse_y = 12;  // Centro de pantalla
    
    // Actualizar posición
    mouse_x += packet->delta_x;
    mouse_y -= packet->delta_y;  // Y invertido
    
    // Limitar a pantalla
    if (mouse_x < 0) mouse_x = 0;
    if (mouse_x >= 80) mouse_x = 79;
    if (mouse_y < 0) mouse_y = 0;
    if (mouse_y >= 25) mouse_y = 24;
    
    // Dibujar cursor
    draw_mouse_cursor(mouse_x, mouse_y);
    
    // Manejar clicks
    if (packet->buttons & 0x01) {  // Botón izquierdo
        handle_left_click(mouse_x, mouse_y);
    }
    if (packet->buttons & 0x02) {  // Botón derecho
        handle_right_click(mouse_x, mouse_y);
    }
}
```

---

## 📋 Resumen del Capítulo

### Conceptos Clave Aprendidos

1. **Fundamentos de Interrupciones**:
   - Diferencias entre interrupciones, excepciones y traps
   - Vectores de interrupción y su clasificación
   - Flujo de ejecución durante una interrupción

2. **Interrupt Descriptor Table (IDT)**:
   - Estructura y configuración del IDT
   - Tipos de descriptores y sus atributos
   - Carga y activación del IDT

3. **Controladores de Interrupciones**:
   - PIC 8259A: configuración y limitaciones
   - APIC: características avanzadas y SMP
   - Gestión de prioridades y máscaras

4. **Implementación Práctica**:
   - Handlers en ensamblador y C
   - Gestión de contexto y stack
   - System calls básicos

### Habilidades Técnicas Desarrolladas

- ✅ **Configuración** del IDT y handlers de interrupciones
- ✅ **Implementación** de drivers usando interrupciones
- ✅ **Manejo** de excepciones y depuración de fallas
- ✅ **Optimización** del rendimiento del sistema
- ✅ **Diseño** de interfaces asíncronas

### Conexión con Sistemas Reales

Este capítulo establece las bases para:
- **Sistemas multitarea**: El timer permite scheduling preemptivo
- **Drivers de dispositivos**: E/O asíncrona eficiente
- **System calls**: Interfaz segura kernel/usuario
- **Manejo de errores**: Recuperación robusta de fallas

### Preparación para el Siguiente Capítulo

El **Capítulo 6** construirá sobre estos fundamentos para explorar:
- Multitasking cooperativo y preemptivo
- Implementación de un scheduler completo
- Context switching eficiente
- Sincronización entre procesos

---

**📚 Lecturas Complementarias:**
- "Intel 64 and IA-32 Architectures Software Developer's Manual, Volume 3A"
- "Understanding the Linux Kernel" - Capítulo 4: Interrupts and Exceptions
- "Modern Operating Systems" - Capítulo 1.6: System Calls
- "Operating System Concepts" - Capítulo 13: I/O Systems