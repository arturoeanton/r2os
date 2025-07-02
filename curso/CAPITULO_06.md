# Capítulo 6: Multitasking y Scheduler

## 📚 Introducción

Este capítulo explora los mecanismos fundamentales que permiten a los sistemas operativos ejecutar múltiples tareas de manera concurrente. El multitasking y el scheduler son componentes esenciales que definen la capacidad de un sistema operativo para gestionar eficientemente los recursos del procesador entre diferentes procesos.

## 🎯 Objetivos de Aprendizaje

Al completar este capítulo, el estudiante será capaz de:

1. **Explicar** los conceptos fundamentales de multitasking preemptivo y cooperativo
2. **Implementar** un scheduler básico de round-robin
3. **Diseñar** estructuras de datos para la gestión de procesos
4. **Optimizar** algoritmos de scheduling para diferentes escenarios
5. **Crear** mecanismos de sincronización entre procesos

---

## 🏛️ Parte Teórica

### 1.1 Fundamentos del Multitasking

#### ¿Qué es el Multitasking?

El multitasking es la capacidad de un sistema operativo para ejecutar múltiples procesos de manera concurrente, creando la ilusión de que todos los procesos se ejecutan simultáneamente en un sistema de un solo procesador.

```
┌─────────────────────────────────────────────────────────────┐
│                    TIMELINE DE EJECUCIÓN                    │
│                                                             │
│  Proceso A: █████░░░░░█████░░░░░█████░░░░░█████             │
│  Proceso B: ░░░░░█████░░░░░█████░░░░░█████░░░░░█████         │
│  Proceso C: ░░░░░░░░░░░░░░░░░░░░█████░░░░░░░░░░█████         │
│             ↑     ↑     ↑     ↑     ↑     ↑     ↑         │
│           T1   T2   T3   T4   T5   T6   T7   T8            │
│                                                             │
│  █ = Proceso ejecutándose                                   │
│  ░ = Proceso en espera                                      │
└─────────────────────────────────────────────────────────────┘
```

#### Tipos de Multitasking

**1. Multitasking Cooperativo**
```c
// Ejemplo conceptual de multitasking cooperativo
void cooperative_task_1() {
    while (1) {
        printf("Ejecutando tarea 1\n");
        // Importante: ceder control voluntariamente
        yield();  // El proceso decide cuándo ceder
    }
}

void cooperative_task_2() {
    while (1) {
        printf("Ejecutando tarea 2\n");
        yield();  // Sin yield(), el proceso monopoliza la CPU
    }
}
```

**2. Multitasking Preemptivo**
```c
// El scheduler interrumpe procesos automáticamente
void preemptive_scheduler() {
    // Timer interrupt cada 10ms
    setup_timer_interrupt(10);  // Quantum de tiempo
    
    while (1) {
        current_process = get_next_process();
        switch_to_process(current_process);
        // El timer interrupt forzará el cambio de contexto
    }
}
```

### 1.2 Scheduler (Planificador)

#### Componentes del Scheduler

```
┌─────────────────────────────────────────────────────────────┐
│                    COMPONENTES DEL SCHEDULER                 │
│                                                             │
│  ┌─────────────────┐    ┌─────────────────┐                │
│  │   PROCESS       │    │   SCHEDULER     │                │
│  │   CONTROL       │    │   ALGORITHM     │                │
│  │   BLOCK (PCB)   │    │                 │                │
│  │                 │    │  - Round Robin  │                │
│  │  - PID          │    │  - Priority     │                │
│  │  - State        │    │  - FIFO         │                │
│  │  - Priority     │    │  - SJF          │                │
│  │  - Registers    │    │  - Multi-level  │                │
│  │  - Memory       │    │                 │                │
│  └─────────────────┘    └─────────────────┘                │
│           │                       │                        │
│           ▼                       ▼                        │
│  ┌─────────────────┐    ┌─────────────────┐                │
│  │   CONTEXT       │    │   READY QUEUE   │                │
│  │   SWITCHING     │    │                 │                │
│  │                 │    │  Ready Process  │                │
│  │  - Save state   │    │  Ready Process  │                │
│  │  - Load state   │    │  Ready Process  │                │
│  │  - Switch MMU   │    │  Ready Process  │                │
│  └─────────────────┘    └─────────────────┘                │
└─────────────────────────────────────────────────────────────┘
```

#### Estados de los Procesos

```
┌─────────────────────────────────────────────────────────────┐
│                    ESTADOS DE PROCESOS                      │
│                                                             │
│     ┌─────────────┐                                        │
│     │     NEW     │                                        │
│     │ (Creándose) │                                        │
│     └─────────────┘                                        │
│            │                                               │
│            ▼                                               │
│     ┌─────────────┐    Schedule    ┌─────────────┐         │
│     │    READY    │ ◄─────────────► │   RUNNING   │         │
│     │ (Preparado) │                │ (Ejecutando)│         │
│     └─────────────┘                └─────────────┘         │
│            ▲                               │               │
│            │                               ▼               │
│     ┌─────────────┐    I/O Complete ┌─────────────┐         │
│     │   WAITING   │ ◄─────────────── │ TERMINATED  │         │
│     │ (Esperando) │                 │ (Terminado) │         │
│     └─────────────┘                 └─────────────┘         │
│                                                             │
│  Transiciones:                                             │
│  1. NEW → READY     : Proceso admitido                     │
│  2. READY → RUNNING : Scheduler selecciona proceso         │
│  3. RUNNING → READY : Quantum expirado (preemption)       │
│  4. RUNNING → WAITING: Espera I/O o evento                │
│  5. WAITING → READY : Evento completado                    │
│  6. RUNNING → TERMINATED: Proceso termina                  │
└─────────────────────────────────────────────────────────────┘
```

### 1.3 Algoritmos de Scheduling

#### 1. Round Robin (RR)

```c
// Implementación conceptual de Round Robin
typedef struct {
    int quantum;          // Tiempo asignado por turno
    queue_t ready_queue;  // Cola de procesos listos
} round_robin_scheduler_t;

void round_robin_schedule(round_robin_scheduler_t *scheduler) {
    process_t *current = dequeue(&scheduler->ready_queue);
    
    if (current == NULL) {
        idle_process();
        return;
    }
    
    // Configurar timer para el quantum
    set_timer_quantum(scheduler->quantum);
    
    // Ejecutar proceso
    switch_to_process(current);
    
    // Cuando el timer expira, regresa aquí
    enqueue(&scheduler->ready_queue, current);
}
```

#### 2. Priority Scheduling

```c
// Scheduling basado en prioridades
typedef struct {
    priority_queue_t priority_queues[MAX_PRIORITY];
    int highest_priority;
} priority_scheduler_t;

process_t *priority_schedule(priority_scheduler_t *scheduler) {
    // Buscar proceso con mayor prioridad
    for (int i = scheduler->highest_priority; i >= 0; i--) {
        if (!is_empty(&scheduler->priority_queues[i])) {
            return dequeue(&scheduler->priority_queues[i]);
        }
    }
    return NULL;  // No hay procesos
}
```

#### 3. Multilevel Feedback Queue (MLFQ)

```c
// Scheduler multinivel con retroalimentación
typedef struct {
    queue_t queues[NUM_LEVELS];
    int quantum[NUM_LEVELS];      // Quantum por nivel
    int current_level;
} mlfq_scheduler_t;

void mlfq_schedule(mlfq_scheduler_t *scheduler) {
    // Buscar proceso en el nivel de mayor prioridad
    for (int level = 0; level < NUM_LEVELS; level++) {
        if (!is_empty(&scheduler->queues[level])) {
            process_t *proc = dequeue(&scheduler->queues[level]);
            
            // Asignar quantum según el nivel
            set_timer_quantum(scheduler->quantum[level]);
            
            // Si el proceso no termina, bajar de nivel
            if (switch_to_process(proc) == QUANTUM_EXPIRED) {
                int new_level = min(level + 1, NUM_LEVELS - 1);
                enqueue(&scheduler->queues[new_level], proc);
            }
            return;
        }
    }
}
```

### 1.4 Context Switching

#### Proceso de Cambio de Contexto

```c
// Estructura para guardar el contexto del proceso
typedef struct {
    uint32_t eax, ebx, ecx, edx;
    uint32_t esi, edi, ebp, esp;
    uint32_t eip, eflags;
    uint32_t cs, ds, es, fs, gs, ss;
    uint32_t cr3;  // Page directory para MMU
} cpu_context_t;

// Función de cambio de contexto
void context_switch(process_t *old_proc, process_t *new_proc) {
    // 1. Guardar contexto del proceso actual
    save_cpu_context(&old_proc->context);
    
    // 2. Cambiar espacio de memoria (MMU)
    set_page_directory(new_proc->page_directory);
    
    // 3. Restaurar contexto del nuevo proceso
    restore_cpu_context(&new_proc->context);
    
    // 4. Saltar a la ejecución del nuevo proceso
    jump_to_process(new_proc);
}
```

#### Implementación en Ensamblador

```nasm
; context_switch.asm - Cambio de contexto optimizado
section .text
global context_switch

context_switch:
    ; Parámetros: old_context (esp+4), new_context (esp+8)
    
    ; Guardar contexto actual
    mov eax, [esp+4]        ; old_context
    mov [eax+0], ebx        ; Guardar registros
    mov [eax+4], ecx
    mov [eax+8], edx
    mov [eax+12], esi
    mov [eax+16], edi
    mov [eax+20], ebp
    mov [eax+24], esp
    
    ; Guardar EIP (dirección de retorno)
    mov ebx, [esp]          ; EIP está en el stack
    mov [eax+28], ebx
    
    ; Guardar flags
    pushfd
    pop ebx
    mov [eax+32], ebx
    
    ; Cargar nuevo contexto
    mov eax, [esp+8]        ; new_context
    mov ebx, [eax+0]        ; Restaurar registros
    mov ecx, [eax+4]
    mov edx, [eax+8]
    mov esi, [eax+12]
    mov edi, [eax+16]
    mov ebp, [eax+20]
    mov esp, [eax+24]
    
    ; Restaurar flags
    push dword [eax+32]
    popfd
    
    ; Saltar al nuevo proceso
    jmp dword [eax+28]
```

---

## 🔧 Parte Práctica

### 2.1 Implementación Básica de Scheduler

#### Estructura del Process Control Block (PCB)

```c
// kernel_scheduler.h
#ifndef KERNEL_SCHEDULER_H
#define KERNEL_SCHEDULER_H

#include <stdint.h>

// Estados de proceso
typedef enum {
    PROCESS_NEW,
    PROCESS_READY,
    PROCESS_RUNNING,
    PROCESS_WAITING,
    PROCESS_TERMINATED
} process_state_t;

// Prioridades de proceso
typedef enum {
    PRIORITY_HIGH = 0,
    PRIORITY_NORMAL = 1,
    PRIORITY_LOW = 2,
    PRIORITY_IDLE = 3
} process_priority_t;

// Process Control Block
typedef struct process {
    uint32_t pid;                    // Process ID
    char name[32];                   // Nombre del proceso
    process_state_t state;           // Estado actual
    process_priority_t priority;     // Prioridad
    
    // Contexto de CPU
    cpu_context_t context;           // Registros guardados
    
    // Gestión de memoria
    uint32_t *page_directory;        // Directorio de páginas
    uint32_t stack_base;             // Base del stack
    uint32_t stack_size;             // Tamaño del stack
    
    // Estadísticas
    uint32_t time_created;           // Cuando se creó
    uint32_t time_scheduled;         // Tiempo total programado
    uint32_t quantum_remaining;      // Quantum restante
    
    // Lista enlazada
    struct process *next;            // Siguiente proceso
    struct process *prev;            // Proceso anterior
} process_t;

// Scheduler principal
typedef struct {
    process_t *current_process;      // Proceso actual
    process_t *ready_queue;          // Cola de procesos listos
    process_t *waiting_queue;        // Cola de procesos esperando
    
    uint32_t quantum_length;         // Duración del quantum (ms)
    uint32_t total_processes;        // Total de procesos
    uint32_t next_pid;               // Siguiente PID a asignar
    
    // Estadísticas
    uint32_t context_switches;       // Número de cambios de contexto
    uint32_t total_runtime;          // Tiempo total del sistema
} scheduler_t;

// Funciones del scheduler
void scheduler_init(void);
process_t *process_create(const char *name, void (*entry_point)(void));
void process_exit(process_t *process);
void scheduler_run(void);
void scheduler_yield(void);
void scheduler_preempt(void);

#endif
```

#### Implementación del Scheduler

```c
// kernel_scheduler.c
#include "kernel_scheduler.h"
#include "timer.h"
#include "memory.h"
#include "kernel.h"

static scheduler_t scheduler;

void scheduler_init(void) {
    scheduler.current_process = NULL;
    scheduler.ready_queue = NULL;
    scheduler.waiting_queue = NULL;
    scheduler.quantum_length = 10;  // 10ms por defecto
    scheduler.total_processes = 0;
    scheduler.next_pid = 1;
    scheduler.context_switches = 0;
    scheduler.total_runtime = 0;
    
    printf("Scheduler inicializado - Quantum: %d ms\n", scheduler.quantum_length);
}

process_t *process_create(const char *name, void (*entry_point)(void)) {
    process_t *process = kmalloc(sizeof(process_t));
    if (!process) {
        printf("Error: No se pudo crear proceso '%s'\n", name);
        return NULL;
    }
    
    // Inicializar PCB
    process->pid = scheduler.next_pid++;
    strncpy(process->name, name, 31);
    process->name[31] = '\0';
    process->state = PROCESS_NEW;
    process->priority = PRIORITY_NORMAL;
    
    // Configurar stack del proceso
    process->stack_size = 4096;  // 4KB stack
    process->stack_base = (uint32_t)kmalloc(process->stack_size);
    if (!process->stack_base) {
        kfree(process);
        return NULL;
    }
    
    // Configurar contexto inicial
    memset(&process->context, 0, sizeof(cpu_context_t));
    process->context.esp = process->stack_base + process->stack_size - 4;
    process->context.eip = (uint32_t)entry_point;
    process->context.eflags = 0x200;  // Interrupts enabled
    
    // Configurar memoria virtual (simple)
    process->page_directory = create_page_directory();
    
    // Estadísticas
    process->time_created = get_system_time();
    process->time_scheduled = 0;
    process->quantum_remaining = scheduler.quantum_length;
    
    // Agregar a cola de listos
    process->state = PROCESS_READY;
    add_to_ready_queue(process);
    
    scheduler.total_processes++;
    
    printf("Proceso creado: PID=%d, Nombre='%s'\n", process->pid, process->name);
    return process;
}

void add_to_ready_queue(process_t *process) {
    if (scheduler.ready_queue == NULL) {
        scheduler.ready_queue = process;
        process->next = process;
        process->prev = process;
    } else {
        process->next = scheduler.ready_queue;
        process->prev = scheduler.ready_queue->prev;
        scheduler.ready_queue->prev->next = process;
        scheduler.ready_queue->prev = process;
    }
    process->state = PROCESS_READY;
}

process_t *remove_from_ready_queue(void) {
    if (scheduler.ready_queue == NULL) {
        return NULL;
    }
    
    process_t *process = scheduler.ready_queue;
    
    if (process->next == process) {
        // Solo un proceso en la cola
        scheduler.ready_queue = NULL;
    } else {
        process->prev->next = process->next;
        process->next->prev = process->prev;
        scheduler.ready_queue = process->next;
    }
    
    process->next = NULL;
    process->prev = NULL;
    return process;
}

void scheduler_run(void) {
    if (scheduler.current_process == NULL) {
        // Primera ejecución
        scheduler.current_process = remove_from_ready_queue();
        if (scheduler.current_process == NULL) {
            idle_process();
            return;
        }
    }
    
    process_t *current = scheduler.current_process;
    current->state = PROCESS_RUNNING;
    
    // Configurar timer para quantum
    set_timer_quantum(scheduler.quantum_length);
    
    // Cambiar contexto al proceso
    printf("Ejecutando proceso: PID=%d, Nombre='%s'\n", 
           current->pid, current->name);
    
    switch_to_process(current);
}

void scheduler_yield(void) {
    if (scheduler.current_process == NULL) {
        return;
    }
    
    process_t *current = scheduler.current_process;
    
    // Guardar contexto actual
    save_current_context(&current->context);
    
    // Mover proceso actual a cola de listos
    add_to_ready_queue(current);
    
    // Seleccionar siguiente proceso
    scheduler.current_process = remove_from_ready_queue();
    
    scheduler.context_switches++;
    
    // Ejecutar siguiente proceso
    scheduler_run();
}

void scheduler_preempt(void) {
    // Llamada desde timer interrupt
    if (scheduler.current_process == NULL) {
        return;
    }
    
    process_t *current = scheduler.current_process;
    
    // Decrementar quantum
    current->quantum_remaining--;
    
    if (current->quantum_remaining <= 0) {
        // Quantum agotado, cambiar de proceso
        current->quantum_remaining = scheduler.quantum_length;
        scheduler_yield();
    }
}

void idle_process(void) {
    // Proceso idle cuando no hay nada que ejecutar
    printf("Sistema idle - No hay procesos listos\n");
    
    // Esperar interrupciones
    asm volatile("hlt");
}
```

### 2.2 Integración con Timer

```c
// timer_integration.c
#include "timer.h"
#include "kernel_scheduler.h"

void timer_interrupt_handler(void) {
    static uint32_t ticks = 0;
    ticks++;
    
    // Actualizar tiempo del sistema
    update_system_time();
    
    // Preemption cada tick
    scheduler_preempt();
    
    // Estadísticas cada segundo
    if (ticks % 100 == 0) {  // Asumiendo 100 Hz
        print_scheduler_stats();
    }
}

void print_scheduler_stats(void) {
    printf("\n=== ESTADÍSTICAS DEL SCHEDULER ===\n");
    printf("Procesos totales: %d\n", scheduler.total_processes);
    printf("Cambios de contexto: %d\n", scheduler.context_switches);
    printf("Tiempo total: %d ms\n", scheduler.total_runtime);
    printf("Quantum actual: %d ms\n", scheduler.quantum_length);
    
    if (scheduler.current_process) {
        printf("Proceso actual: PID=%d, Nombre='%s'\n",
               scheduler.current_process->pid,
               scheduler.current_process->name);
    }
    printf("==================================\n");
}
```

### 2.3 Procesos de Ejemplo

```c
// example_processes.c
#include "kernel_scheduler.h"

void test_process_1(void) {
    int counter = 0;
    
    while (counter < 100) {
        printf("Proceso 1: contador = %d\n", counter);
        counter++;
        
        // Simular trabajo
        for (volatile int i = 0; i < 100000; i++);
        
        // Ceder CPU voluntariamente
        scheduler_yield();
    }
    
    printf("Proceso 1 terminado\n");
    process_exit(scheduler.current_process);
}

void test_process_2(void) {
    int counter = 0;
    
    while (counter < 50) {
        printf("Proceso 2: valor = %d\n", counter * 2);
        counter++;
        
        // Simular trabajo más intensivo
        for (volatile int i = 0; i < 200000; i++);
        
        scheduler_yield();
    }
    
    printf("Proceso 2 terminado\n");
    process_exit(scheduler.current_process);
}

void cpu_intensive_process(void) {
    printf("Proceso CPU-intensivo iniciado\n");
    
    // Proceso que no cede CPU voluntariamente
    for (int i = 0; i < 1000000; i++) {
        if (i % 100000 == 0) {
            printf("CPU-intensivo: iteración %d\n", i);
        }
    }
    
    printf("Proceso CPU-intensivo terminado\n");
    process_exit(scheduler.current_process);
}

void io_simulation_process(void) {
    for (int i = 0; i < 10; i++) {
        printf("Proceso I/O: operación %d\n", i);
        
        // Simular operación I/O
        simulate_io_operation();
        
        printf("Proceso I/O: operación %d completada\n", i);
    }
    
    printf("Proceso I/O terminado\n");
    process_exit(scheduler.current_process);
}

void simulate_io_operation(void) {
    // Simular I/O colocando proceso en estado waiting
    if (scheduler.current_process) {
        scheduler.current_process->state = PROCESS_WAITING;
        add_to_waiting_queue(scheduler.current_process);
        scheduler.current_process = NULL;
        
        // Simular delay de I/O
        schedule_io_completion(50);  // 50ms delay
        
        // Cambiar a otro proceso
        scheduler_run();
    }
}
```

---

## 🧪 Ejercicios Prácticos

### Ejercicio 1: Scheduler Básico (Nivel: Intermedio)

**Objetivo**: Implementar un scheduler simple con round-robin.

**Instrucciones**:
1. Crear 3 procesos de prueba
2. Implementar cola circular para procesos listos
3. Integrar con timer para preemption
4. Medir tiempo de ejecución de cada proceso

**Código Base**:
```c
// Completar la implementación
void simple_scheduler_demo(void) {
    printf("=== DEMO: Scheduler Básico ===\n");
    
    // Inicializar scheduler
    scheduler_init();
    
    // Crear procesos de prueba
    process_t *proc1 = process_create("Contador", test_process_1);
    process_t *proc2 = process_create("Multiplicador", test_process_2);
    process_t *proc3 = process_create("CPU-Intensivo", cpu_intensive_process);
    
    // TODO: Implementar loop del scheduler
    // TODO: Mostrar estadísticas
    
    printf("=== FIN DEMO ===\n");
}
```

### Ejercicio 2: Priority Scheduling (Nivel: Avanzado)

**Objetivo**: Implementar scheduling basado en prioridades.

**Instrucciones**:
1. Modificar PCB para incluir prioridades
2. Implementar colas de prioridad
3. Manejar starvation con aging
4. Comparar rendimiento con round-robin

**Código Base**:
```c
// Estructura de colas de prioridad
typedef struct {
    process_t *high_priority;
    process_t *normal_priority;
    process_t *low_priority;
    process_t *idle_priority;
} priority_queues_t;

// TODO: Implementar funciones de priority scheduling
void priority_scheduler_init(void);
void add_to_priority_queue(process_t *process);
process_t *get_highest_priority_process(void);
void aging_mechanism(void);
```

### Ejercicio 3: Multilevel Feedback Queue (Nivel: Extremo)

**Objetivo**: Implementar MLFQ con múltiples colas y feedback.

**Instrucciones**:
1. Crear 4 niveles de prioridad
2. Implementar degradación de prioridad
3. Boost periódico de prioridades
4. Quantum variable según nivel

**Código Base**:
```c
#define NUM_LEVELS 4
#define BOOST_INTERVAL 1000  // ms

typedef struct {
    process_t *queues[NUM_LEVELS];
    int quantum_size[NUM_LEVELS];
    uint32_t last_boost_time;
} mlfq_t;

// TODO: Implementar MLFQ completo
void mlfq_init(mlfq_t *mlfq);
void mlfq_add_process(mlfq_t *mlfq, process_t *process);
process_t *mlfq_schedule(mlfq_t *mlfq);
void mlfq_boost(mlfq_t *mlfq);
```

### Ejercicio 4: Análisis de Rendimiento (Nivel: Experto)

**Objetivo**: Comparar diferentes algoritmos de scheduling.

**Instrucciones**:
1. Implementar métricas de rendimiento
2. Crear benchmark con diferentes tipos de procesos
3. Generar estadísticas comparativas
4. Optimizar el scheduler basado en resultados

**Métricas a Medir**:
- Throughput (procesos/segundo)
- Turnaround time promedio
- Waiting time promedio
- Response time promedio
- CPU utilization

```c
// Estructura de métricas
typedef struct {
    uint32_t total_processes;
    uint32_t completed_processes;
    uint32_t total_turnaround_time;
    uint32_t total_waiting_time;
    uint32_t total_response_time;
    uint32_t context_switches;
    uint32_t cpu_idle_time;
} performance_metrics_t;

// TODO: Implementar sistema de métricas
void metrics_init(performance_metrics_t *metrics);
void metrics_process_completed(performance_metrics_t *metrics, process_t *process);
void metrics_print_report(performance_metrics_t *metrics);
```

---

## 📊 Análisis de Complejidad

### Complejidad Temporal

| Algoritmo | Inserción | Selección | Cambio de Contexto |
|-----------|-----------|-----------|-------------------|
| FIFO | O(1) | O(1) | O(1) |
| Round Robin | O(1) | O(1) | O(1) |
| Priority Queue | O(log n) | O(log n) | O(1) |
| MLFQ | O(1) | O(k) | O(1) |

### Complejidad Espacial

```c
// Análisis de memoria por proceso
sizeof(process_t) = 
    sizeof(uint32_t) * 10 +      // Campos básicos
    sizeof(cpu_context_t) +       // Contexto CPU (~64 bytes)
    sizeof(char) * 32 +           // Nombre del proceso
    sizeof(struct process *) * 2; // Punteros lista

// Memoria total del scheduler
scheduler_memory = 
    sizeof(scheduler_t) +                    // Estructura principal
    num_processes * sizeof(process_t) +      // PCBs
    num_processes * stack_size +             // Stacks de procesos
    num_processes * page_directory_size;     // Tablas de página
```

---

## 🔍 Debugging y Optimización

### Herramientas de Debug

```c
// Funciones de debugging del scheduler
void debug_print_process_list(void) {
    printf("\n=== LISTA DE PROCESOS ===\n");
    
    process_t *current = scheduler.ready_queue;
    if (current == NULL) {
        printf("No hay procesos en cola\n");
        return;
    }
    
    do {
        printf("PID: %d, Nombre: %s, Estado: %d, Prioridad: %d\n",
               current->pid, current->name, current->state, current->priority);
        current = current->next;
    } while (current != scheduler.ready_queue);
    
    printf("========================\n");
}

void debug_scheduler_state(void) {
    printf("\n=== ESTADO DEL SCHEDULER ===\n");
    printf("Proceso actual: %s\n", 
           scheduler.current_process ? scheduler.current_process->name : "Ninguno");
    printf("Quantum restante: %d\n", 
           scheduler.current_process ? scheduler.current_process->quantum_remaining : 0);
    printf("Procesos totales: %d\n", scheduler.total_processes);
    printf("Cambios de contexto: %d\n", scheduler.context_switches);
    printf("============================\n");
}
```

### Optimizaciones

```c
// Optimización 1: Cache de procesos frecuentes
typedef struct {
    process_t *last_scheduled;
    uint32_t cache_hits;
    uint32_t cache_misses;
} scheduler_cache_t;

// Optimización 2: Batch scheduling
void batch_schedule(int batch_size) {
    for (int i = 0; i < batch_size && scheduler.ready_queue; i++) {
        process_t *proc = remove_from_ready_queue();
        execute_process_batch(proc);
    }
}

// Optimización 3: Lazy context switching
bool need_context_switch(process_t *current, process_t *next) {
    return current->page_directory != next->page_directory ||
           current->priority != next->priority;
}
```

---

## 🎯 Proyecto Final: Sistema Multitasking Completo

### Especificaciones

Implementar un sistema completo de multitasking con:

1. **Scheduler Híbrido**:
   - MLFQ para procesos interactivos
   - Round-robin para procesos batch
   - Priority scheduling para procesos críticos

2. **Gestión de Procesos**:
   - Creación y terminación dinámica
   - Comunicación inter-proceso básica
   - Sincronización con semáforos

3. **Interfaz de Usuario**:
   - Comandos de shell para gestión de procesos
   - Monitoreo en tiempo real
   - Estadísticas de rendimiento

### Comandos a Implementar

```c
// Comandos del shell para multitasking
void cmd_ps(void);          // Listar procesos
void cmd_kill(int pid);     // Terminar proceso
void cmd_top(void);         // Monitor en tiempo real
void cmd_nice(int pid, int priority);  // Cambiar prioridad
void cmd_jobs(void);        // Procesos en background
void cmd_bg(int pid);       // Enviar a background
void cmd_fg(int pid);       // Traer a foreground
```

### Criterios de Evaluación

1. **Funcionalidad** (40%):
   - Scheduler funciona correctamente
   - Procesos se ejecutan sin interferencias
   - Manejo correcto de estados

2. **Rendimiento** (30%):
   - Baja latencia en cambios de contexto
   - Uso eficiente de CPU
   - Métricas de rendimiento

3. **Robustez** (20%):
   - Manejo de errores
   - Recuperación de procesos problemáticos
   - Estabilidad del sistema

4. **Código** (10%):
   - Estructura clara y documentada
   - Uso de buenas prácticas
   - Facilidad de mantenimiento

---

## 📚 Recursos Adicionales

### Lecturas Recomendadas

1. **"Operating System Concepts" - Silberschatz**
   - Capítulos 3-5: Process Management
   - Capítulos 6-7: CPU Scheduling

2. **"Modern Operating Systems" - Tanenbaum**
   - Capítulo 2: Processes and Threads
   - Capítulo 3: Memory Management

3. **"The Design of the UNIX Operating System" - Bach**
   - Capítulo 8: Process Scheduling

### Recursos Online

- [OSDev Wiki - Scheduling](https://wiki.osdev.org/Scheduling)
- [Linux Kernel Development - Process Scheduling](https://www.kernel.org/doc/html/latest/scheduler/)
- [Intel x86 Manual - Task Management](https://software.intel.com/content/www/us/en/develop/articles/intel-sdm.html)

### Herramientas de Desarrollo

```bash
# Profiling de rendimiento
make profile-scheduler

# Debugging con GDB
make debug-scheduler
(gdb) break scheduler_run
(gdb) info registers
(gdb) backtrace

# Análisis de memoria
make memory-analysis
```

---

## 🏆 Resumen del Capítulo

### Conceptos Clave Aprendidos

1. **Multitasking**: Ejecución concurrente de múltiples procesos
2. **Scheduler**: Algoritmo que decide qué proceso ejecutar
3. **Context Switch**: Cambio de contexto entre procesos
4. **Process States**: Estados de los procesos (NEW, READY, RUNNING, etc.)
5. **Scheduling Algorithms**: Round-robin, prioridades, MLFQ

### Habilidades Desarrolladas

- ✅ Implementación de scheduler básico
- ✅ Manejo de Process Control Blocks
- ✅ Algoritmos de scheduling
- ✅ Optimización de rendimiento
- ✅ Debugging de sistemas concurrentes

### Próximos Pasos

En el siguiente capítulo exploraremos **Redes y Protocolos**, donde aprenderemos a implementar comunicación entre sistemas y protocolos de red básicos.

---

*"El multitasking no es sobre hacer más cosas a la vez, sino sobre hacer las cosas correctas en el momento correcto."* - Principio de Sistemas Operativos

**¡Felicitaciones!** 🎉 Has completado el capítulo de Multitasking y Scheduler. Ahora tienes los conocimientos fundamentales para implementar sistemas operativos con capacidad de ejecución concurrente.