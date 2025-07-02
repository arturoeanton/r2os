# Capítulo 8: Seguridad y Virtualización

## 📚 Introducción

Este capítulo final explora dos aspectos críticos de los sistemas operativos modernos: la seguridad y la virtualización. Aprenderemos cómo proteger el sistema contra amenazas, implementar mecanismos de aislamiento y crear entornos virtualizados que permitan ejecutar múltiples sistemas operativos de manera segura y eficiente.

## 🎯 Objetivos de Aprendizaje

Al completar este capítulo, el estudiante será capaz de:

1. **Implementar** mecanismos básicos de seguridad en el kernel
2. **Diseñar** sistemas de control de acceso y permisos
3. **Crear** un hypervisor básico para virtualización
4. **Gestionar** el aislamiento de procesos y memoria
5. **Analizar** vulnerabilidades y implementar contramedidas

---

## 🏛️ Parte Teórica

### 1.1 Fundamentos de Seguridad

#### Principios de Seguridad

```
┌─────────────────────────────────────────────────────────────┐
│                 PRINCIPIOS DE SEGURIDAD                     │
│                                                             │
│  ┌─────────────────┐    ┌─────────────────┐                │
│  │ CONFIDENCIALIDAD│    │  INTEGRIDAD     │                │
│  │                 │    │                 │                │
│  │ • Encriptación  │    │ • Checksums     │                │
│  │ • Control acceso│    │ • Firmas        │                │
│  │ • Aislamiento   │    │ • Validación    │                │
│  └─────────────────┘    └─────────────────┘                │
│           │                       │                        │
│           └───────────┬───────────┘                        │
│                       ▼                                    │
│  ┌─────────────────────────────────────┐                  │
│  │          DISPONIBILIDAD              │                  │
│  │                                     │                  │
│  │  • Redundancia                      │                  │
│  │  • Recuperación                     │                  │
│  │  • Balanceadores                    │                  │
│  └─────────────────────────────────────┘                  │
│                                                             │
│  Elementos adicionales:                                    │
│  • AUTENTICACIÓN: Verificar identidad                     │
│  • AUTORIZACIÓN: Controlar permisos                       │
│  • AUDITORIA: Registrar actividades                       │
│  • NO REPUDIO: Probar acciones                            │
└─────────────────────────────────────────────────────────────┘
```

#### Amenazas Comunes en Sistemas Operativos

```
┌─────────────────────────────────────────────────────────────┐
│                    TIPOS DE AMENAZAS                        │
│                                                             │
│  1. BUFFER OVERFLOW                                         │
│  ┌─────────────────────────────────────────────────────────┐ │
│  │ char buffer[10];                                        │ │
│  │ strcpy(buffer, "Este string es demasiado largo!");     │ │
│  │                                                         │ │
│  │ Stack:                                                  │ │
│  │ [buffer][saved_ebp][return_addr] <-- SOBRESCRITO       │ │
│  └─────────────────────────────────────────────────────────┘ │
│                                                             │
│  2. PRIVILEGE ESCALATION                                    │
│  ┌─────────────────────────────────────────────────────────┐ │
│  │ Usuario normal → Root/Kernel                            │ │
│  │ Explotar vulnerabilidades en drivers                    │ │
│  │ Bugs en system calls                                    │ │
│  └─────────────────────────────────────────────────────────┘ │
│                                                             │
│  3. INJECTION ATTACKS                                       │
│  ┌─────────────────────────────────────────────────────────┐ │
│  │ SQL Injection, Command Injection                        │ │
│  │ Datos maliciosos → Ejecución de código                  │ │
│  └─────────────────────────────────────────────────────────┘ │
│                                                             │
│  4. SIDE-CHANNEL ATTACKS                                    │
│  ┌─────────────────────────────────────────────────────────┐ │
│  │ Timing attacks, Cache attacks                           │ │
│  │ Spectre, Meltdown                                       │ │
│  └─────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────┘
```

### 1.2 Mecanismos de Protección

#### Anillos de Protección (x86)

```
┌─────────────────────────────────────────────────────────────┐
│                   ANILLOS DE PROTECCIÓN x86                 │
│                                                             │
│         ┌─────────────────────────────────────┐             │
│         │            Ring 0                   │             │
│         │          (Kernel)                   │             │
│         │                                     │             │
│         │  • Acceso completo al hardware      │             │
│         │  • Control de memoria               │             │
│         │  • Gestión de interrupciones        │             │
│         │                                     │             │
│         └─────────────────────────────────────┘             │
│                         │                                   │
│           ┌─────────────────────────────────────┐           │
│           │            Ring 1                   │           │
│           │         (Device Drivers)            │           │
│           │                                     │           │
│           │  • Drivers de dispositivos          │           │
│           │  • Acceso controlado al hardware    │           │
│           │                                     │           │
│           └─────────────────────────────────────┘           │
│                         │                                   │
│             ┌─────────────────────────────────────┐         │
│             │            Ring 2                   │         │
│             │        (System Services)            │         │
│             │                                     │         │
│             │  • Servicios del sistema            │         │
│             │  • APIs privilegiadas               │         │
│             │                                     │         │
│             └─────────────────────────────────────┘         │
│                         │                                   │
│               ┌─────────────────────────────────────┐       │
│               │            Ring 3                   │       │
│               │        (User Applications)          │       │
│               │                                     │       │
│               │  • Aplicaciones de usuario          │       │
│               │  • Acceso limitado                  │       │
│               │  • Sin acceso directo al hardware   │       │
│               │                                     │       │
│               └─────────────────────────────────────┘       │
│                                                             │
│  Transiciones entre anillos:                               │
│  • System calls (Ring 3 → Ring 0)                         │
│  • Interrupts y excepciones                               │
│  • Call gates y task switches                             │
└─────────────────────────────────────────────────────────────┘
```

#### Memory Protection Unit (MPU)

```c
// Configuración de protección de memoria
typedef struct {
    uint32_t base_address;     // Dirección base
    uint32_t size;             // Tamaño de la región
    uint32_t permissions;      // Permisos de acceso
} memory_region_t;

// Permisos de memoria
#define PERM_READ       0x01
#define PERM_WRITE      0x02
#define PERM_EXECUTE    0x04
#define PERM_USER       0x08
#define PERM_CACHE      0x10

// Configuraciones típicas
#define PERM_KERNEL_CODE    (PERM_READ | PERM_EXECUTE)
#define PERM_KERNEL_DATA    (PERM_READ | PERM_WRITE)
#define PERM_USER_CODE      (PERM_READ | PERM_EXECUTE | PERM_USER)
#define PERM_USER_DATA      (PERM_READ | PERM_WRITE | PERM_USER)
#define PERM_USER_STACK     (PERM_READ | PERM_WRITE | PERM_USER)
```

### 1.3 Virtualización

#### Tipos de Virtualización

```
┌─────────────────────────────────────────────────────────────┐
│                   TIPOS DE VIRTUALIZACIÓN                   │
│                                                             │
│  1. VIRTUALIZACIÓN COMPLETA (Type 1 - Bare Metal)          │
│  ┌─────────────────────────────────────────────────────────┐ │
│  │  VM 1    │  VM 2    │  VM 3    │  VM 4                │ │
│  │ Windows  │  Linux   │  macOS   │  FreeBSD             │ │
│  ├─────────────────────────────────────────────────────────┤ │
│  │              HYPERVISOR (VMware vSphere)               │ │
│  ├─────────────────────────────────────────────────────────┤ │
│  │                   HARDWARE                              │ │
│  └─────────────────────────────────────────────────────────┘ │
│                                                             │
│  2. VIRTUALIZACIÓN HOSPEDADA (Type 2 - Hosted)            │
│  ┌─────────────────────────────────────────────────────────┐ │
│  │  VM 1    │  VM 2    │  VM 3    │                      │ │
│  │ Linux    │ Windows  │ Android  │                      │ │
│  ├─────────────────────────────────────────────────────────┤ │
│  │         HYPERVISOR (VirtualBox, VMware Player)         │ │
│  ├─────────────────────────────────────────────────────────┤ │
│  │                HOST OS (Windows/Linux)                  │ │
│  ├─────────────────────────────────────────────────────────┤ │
│  │                   HARDWARE                              │ │
│  └─────────────────────────────────────────────────────────┘ │
│                                                             │
│  3. PARA-VIRTUALIZACIÓN                                    │
│  ┌─────────────────────────────────────────────────────────┐ │
│  │  Guest OS modificado para trabajar con hypervisor      │ │
│  │  Mejor rendimiento que virtualización completa         │ │
│  │  Ejemplo: Xen paravirtualization                       │ │
│  └─────────────────────────────────────────────────────────┘ │
│                                                             │
│  4. CONTENEDORES (OS-Level Virtualization)                │
│  ┌─────────────────────────────────────────────────────────┐ │
│  │Container│Container│Container│Container│                │ │
│  │  App A  │  App B  │  App C  │  App D  │                │ │
│  ├─────────────────────────────────────────────────────────┤ │
│  │            CONTAINER RUNTIME (Docker)                  │ │
│  ├─────────────────────────────────────────────────────────┤ │
│  │                   HOST OS                               │ │
│  ├─────────────────────────────────────────────────────────┤ │
│  │                   HARDWARE                              │ │
│  └─────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────┘
```

#### Intel VT-x / AMD-V

```
┌─────────────────────────────────────────────────────────────┐
│               VIRTUALIZACIÓN POR HARDWARE                   │
│                                                             │
│  Intel VT-x (Virtualization Technology):                   │
│  ┌─────────────────────────────────────────────────────────┐ │
│  │                                                         │ │
│  │  VMX Root Mode          VMX Non-Root Mode               │ │
│  │  (Hypervisor)           (Guest OS)                      │ │
│  │                                                         │ │
│  │  ┌─────────────┐       ┌─────────────┐                │ │
│  │  │  Ring 0     │       │  Ring 0     │                │ │
│  │  │ Hypervisor  │       │  Guest OS   │                │ │
│  │  └─────────────┘       └─────────────┘                │ │
│  │  ┌─────────────┐       ┌─────────────┐                │ │
│  │  │  Ring 3     │       │  Ring 3     │                │ │
│  │  │ Host Apps   │       │ Guest Apps  │                │ │
│  │  └─────────────┘       └─────────────┘                │ │
│  │                                                         │ │
│  │         VM Exit  ←──→  VM Entry                        │ │
│  └─────────────────────────────────────────────────────────┘ │
│                                                             │
│  Características:                                          │
│  • VMCS (Virtual Machine Control Structure)                │
│  • EPT (Extended Page Tables)                              │
│  • VPID (Virtual Processor Identifiers)                    │
│  • VM Exit/Entry automático                                │
└─────────────────────────────────────────────────────────────┘
```

---

## 🔧 Parte Práctica

### 2.1 Sistema de Permisos y Control de Acceso

#### Estructura de Usuario y Permisos

```c
// security.h
#ifndef SECURITY_H
#define SECURITY_H

#include <stdint.h>
#include <stdbool.h>

// Identificadores de usuario
typedef uint32_t uid_t;
typedef uint32_t gid_t;

#define ROOT_UID    0
#define SYSTEM_GID  0

// Permisos de archivo estilo UNIX
typedef struct {
    uint16_t owner_read:1;
    uint16_t owner_write:1;
    uint16_t owner_execute:1;
    uint16_t group_read:1;
    uint16_t group_write:1;
    uint16_t group_execute:1;
    uint16_t other_read:1;
    uint16_t other_write:1;
    uint16_t other_execute:1;
    uint16_t setuid:1;
    uint16_t setgid:1;
    uint16_t sticky:1;
    uint16_t reserved:4;
} file_permissions_t;

// Información de usuario
typedef struct {
    uid_t uid;                 // User ID
    gid_t gid;                 // Primary Group ID
    gid_t groups[MAX_GROUPS];  // Grupos adicionales
    int num_groups;            // Número de grupos
    char username[32];         // Nombre de usuario
    bool is_admin;             // Es administrador
    uint32_t login_time;       // Tiempo de login
    uint32_t session_id;       // ID de sesión
} user_info_t;

// Control de acceso para procesos
typedef struct {
    uid_t real_uid;            // UID real
    uid_t effective_uid;       // UID efectivo
    uid_t saved_uid;           // UID guardado
    gid_t real_gid;            // GID real
    gid_t effective_gid;       // GID efectivo
    gid_t saved_gid;           // GID guardado
    
    // Capabilities (Linux-style)
    uint64_t capabilities;     // Capacidades del proceso
} process_credentials_t;

// Funciones de seguridad
int security_init(void);
int user_authenticate(const char *username, const char *password);
int user_authorize(uid_t uid, const char *resource, int permission);
int process_set_credentials(process_t *proc, uid_t uid, gid_t gid);
bool security_check_permission(file_t *file, process_t *proc, int access_type);

#endif
```

#### Implementación del Sistema de Seguridad

```c
// security.c
#include "security.h"
#include "kernel.h"
#include "filesystem.h"

// Base de datos de usuarios simple
static user_info_t users[MAX_USERS];
static int num_users = 0;
static user_info_t *current_user = NULL;

// Capabilities (simplificadas)
#define CAP_CHOWN       0x01    // Cambiar propietario de archivos
#define CAP_DAC_OVERRIDE 0x02   // Sobrepasar permisos de archivos
#define CAP_SETUID      0x04    // Cambiar UID
#define CAP_SETGID      0x08    // Cambiar GID
#define CAP_SYS_ADMIN   0x10    // Administración del sistema
#define CAP_NET_ADMIN   0x20    // Administración de red
#define CAP_SYS_MODULE  0x40    // Cargar/descargar módulos

int security_init(void) {
    printf("Inicializando sistema de seguridad...\n");
    
    // Crear usuario root
    user_info_t *root = &users[0];
    root->uid = ROOT_UID;
    root->gid = SYSTEM_GID;
    strcpy(root->username, "root");
    root->is_admin = true;
    root->num_groups = 1;
    root->groups[0] = SYSTEM_GID;
    
    // Crear usuario normal
    user_info_t *user = &users[1];
    user->uid = 1000;
    user->gid = 1000;
    strcpy(user->username, "user");
    user->is_admin = false;
    user->num_groups = 1;
    user->groups[0] = 1000;
    
    num_users = 2;
    current_user = root;  // Iniciar como root
    
    printf("Sistema de seguridad inicializado\n");
    printf("Usuarios: root (UID=0), user (UID=1000)\n");
    
    return 0;
}

int user_authenticate(const char *username, const char *password) {
    // Búsqueda simple por nombre de usuario
    for (int i = 0; i < num_users; i++) {
        if (strcmp(users[i].username, username) == 0) {
            // En un sistema real, aquí verificaríamos la contraseña
            // usando hash + salt
            printf("Usuario autenticado: %s\n", username);
            current_user = &users[i];
            current_user->login_time = get_system_time();
            return 0;
        }
    }
    
    printf("Error: Usuario '%s' no encontrado\n", username);
    return -1;
}

bool security_check_permission(file_t *file, process_t *proc, int access_type) {
    if (!file || !proc) {
        return false;
    }
    
    process_credentials_t *creds = &proc->credentials;
    file_permissions_t *perms = &file->permissions;
    
    // Root puede hacer todo
    if (creds->effective_uid == ROOT_UID) {
        return true;
    }
    
    // Verificar permisos por categoría
    bool has_permission = false;
    
    if (file->owner_uid == creds->effective_uid) {
        // Propietario del archivo
        switch (access_type) {
        case ACCESS_READ:
            has_permission = perms->owner_read;
            break;
        case ACCESS_WRITE:
            has_permission = perms->owner_write;
            break;
        case ACCESS_EXECUTE:
            has_permission = perms->owner_execute;
            break;
        }
    } else if (file->owner_gid == creds->effective_gid) {
        // Mismo grupo
        switch (access_type) {
        case ACCESS_READ:
            has_permission = perms->group_read;
            break;
        case ACCESS_WRITE:
            has_permission = perms->group_write;
            break;
        case ACCESS_EXECUTE:
            has_permission = perms->group_execute;
            break;
        }
    } else {
        // Otros usuarios
        switch (access_type) {
        case ACCESS_READ:
            has_permission = perms->other_read;
            break;
        case ACCESS_WRITE:
            has_permission = perms->other_write;
            break;
        case ACCESS_EXECUTE:
            has_permission = perms->other_execute;
            break;
        }
    }
    
    if (!has_permission) {
        printf("Acceso denegado: Usuario %d no tiene permisos %d para archivo '%s'\n",
               creds->effective_uid, access_type, file->name);
    }
    
    return has_permission;
}

int process_set_credentials(process_t *proc, uid_t uid, gid_t gid) {
    if (!proc) {
        return -1;
    }
    
    process_credentials_t *creds = &proc->credentials;
    
    // Solo root o el mismo proceso puede cambiar credenciales
    if (current_user->uid != ROOT_UID && creds->real_uid != current_user->uid) {
        printf("Error: Sin permisos para cambiar credenciales\n");
        return -1;
    }
    
    // Configurar credenciales
    creds->real_uid = uid;
    creds->effective_uid = uid;
    creds->saved_uid = uid;
    creds->real_gid = gid;
    creds->effective_gid = gid;
    creds->saved_gid = gid;
    
    // Asignar capabilities según el usuario
    if (uid == ROOT_UID) {
        creds->capabilities = 0xFFFFFFFF;  // Todas las capabilities
    } else {
        creds->capabilities = 0;  // Sin capabilities especiales
    }
    
    printf("Credenciales actualizadas: PID=%d, UID=%d, GID=%d\n", 
           proc->pid, uid, gid);
    
    return 0;
}
```

### 2.2 Protección de Memoria

#### Stack Protection (Canaries)

```c
// stack_protection.c
#include "security.h"

// Valor canario para detectar stack overflow
static uint32_t stack_canary = 0xDEADBEEF;

// Estructura para función protegida
typedef struct {
    uint32_t canary;           // Canario al inicio del stack frame
    uint32_t saved_ebp;        // EBP guardado
    uint32_t return_address;   // Dirección de retorno
    // Variables locales van aquí
} protected_stack_frame_t;

void __attribute__((no_instrument_function)) 
stack_smash_handler(void) {
    printf("*** STACK SMASHING DETECTADO ***\n");
    printf("Proceso terminado por seguridad\n");
    
    // Terminar proceso actual
    if (scheduler.current_process) {
        process_exit(scheduler.current_process);
    }
    
    // En un sistema real, esto podría:
    // - Enviar alerta al sistema de logging
    // - Crear core dump para análisis
    // - Bloquear futuras ejecuciones del binario
}

// Macros para protección automática
#define STACK_PROTECT_START() \
    uint32_t __stack_canary = stack_canary; \
    asm volatile("" ::: "memory");

#define STACK_PROTECT_END() \
    asm volatile("" ::: "memory"); \
    if (__stack_canary != stack_canary) { \
        stack_smash_handler(); \
    }

// Ejemplo de función protegida
void secure_string_copy(char *dest, const char *src, size_t max_len) {
    STACK_PROTECT_START();
    
    char buffer[256];  // Buffer local protegido
    
    // Copia segura
    size_t len = strlen(src);
    if (len >= max_len) {
        len = max_len - 1;
    }
    
    strncpy(buffer, src, len);
    buffer[len] = '\0';
    strcpy(dest, buffer);
    
    STACK_PROTECT_END();
}
```

#### Address Space Layout Randomization (ASLR)

```c
// aslr.c
#include "memory.h"
#include "security.h"

// Generador de números pseudo-aleatorios simple
static uint32_t aslr_seed = 0x12345678;

uint32_t aslr_random(void) {
    // Linear congruential generator
    aslr_seed = aslr_seed * 1103515245 + 12345;
    return aslr_seed;
}

uint32_t aslr_randomize_address(uint32_t base_address, uint32_t range) {
    uint32_t random_offset = aslr_random() % range;
    
    // Alinear a página
    random_offset &= ~(PAGE_SIZE - 1);
    
    return base_address + random_offset;
}

int aslr_setup_process_memory(process_t *process) {
    // Randomizar direcciones base
    uint32_t code_base = aslr_randomize_address(0x08000000, 0x10000000);
    uint32_t heap_base = aslr_randomize_address(0x20000000, 0x20000000);
    uint32_t stack_base = aslr_randomize_address(0x80000000, 0x40000000);
    
    // Configurar regiones de memoria
    process->memory_layout.code_base = code_base;
    process->memory_layout.heap_base = heap_base;
    process->memory_layout.stack_base = stack_base;
    
    printf("ASLR aplicado al proceso %d:\n", process->pid);
    printf("  Código: 0x%08x\n", code_base);
    printf("  Heap:   0x%08x\n", heap_base);
    printf("  Stack:  0x%08x\n", stack_base);
    
    return 0;
}
```

### 2.3 Hypervisor Básico

#### Estructura del Hypervisor

```c
// hypervisor.h
#ifndef HYPERVISOR_H
#define HYPERVISOR_H

#include <stdint.h>
#include <stdbool.h>

// Estados de la VM
typedef enum {
    VM_STATE_STOPPED,
    VM_STATE_RUNNING,
    VM_STATE_PAUSED,
    VM_STATE_SUSPENDED
} vm_state_t;

// Estructura de una máquina virtual
typedef struct {
    uint32_t vm_id;            // ID de la VM
    char name[64];             // Nombre de la VM
    vm_state_t state;          // Estado actual
    
    // Recursos asignados
    uint32_t memory_size;      // RAM asignada
    uint32_t cpu_quota;        // % de CPU asignado
    uint32_t disk_size;        // Tamaño del disco virtual
    
    // Estado del procesador virtual
    cpu_context_t vcpu_state;  // Estado de la VCPU
    uint32_t *page_directory;  // Tabla de páginas de la VM
    
    // Memoria física asignada
    uint32_t physical_memory_base;
    uint32_t physical_memory_size;
    
    // Dispositivos virtuales
    struct virtual_device *devices;
    
    // Estadísticas
    uint32_t uptime;
    uint32_t cpu_time_used;
    uint32_t memory_used;
    
    struct vm *next;           // Lista enlazada de VMs
} virtual_machine_t;

// Estructura del hypervisor
typedef struct {
    uint32_t num_vms;          // Número de VMs activas
    virtual_machine_t *vm_list; // Lista de VMs
    
    // Recursos totales
    uint32_t total_memory;
    uint32_t available_memory;
    uint32_t total_cpu_time;
    
    // VM actualmente ejecutándose
    virtual_machine_t *current_vm;
    
    // Scheduler de VMs
    uint32_t vm_quantum;       // Quantum para cada VM
    uint32_t vm_scheduling_algorithm;
} hypervisor_t;

// Funciones del hypervisor
int hypervisor_init(void);
virtual_machine_t *vm_create(const char *name, uint32_t memory_mb);
int vm_start(virtual_machine_t *vm);
int vm_stop(virtual_machine_t *vm);
int vm_pause(virtual_machine_t *vm);
int vm_resume(virtual_machine_t *vm);
void hypervisor_schedule(void);

#endif
```

#### Implementación del Hypervisor

```c
// hypervisor.c
#include "hypervisor.h"
#include "memory.h"
#include "kernel.h"

static hypervisor_t hypervisor;

int hypervisor_init(void) {
    printf("Inicializando hypervisor...\n");
    
    // Verificar soporte de virtualización
    if (!check_virtualization_support()) {
        printf("Error: CPU no soporta virtualización por hardware\n");
        return -1;
    }
    
    // Inicializar hypervisor
    hypervisor.num_vms = 0;
    hypervisor.vm_list = NULL;
    hypervisor.current_vm = NULL;
    hypervisor.vm_quantum = 100;  // 100ms por VM
    hypervisor.total_memory = get_total_memory();
    hypervisor.available_memory = hypervisor.total_memory;
    
    printf("Hypervisor inicializado\n");
    printf("Memoria total: %d MB\n", hypervisor.total_memory / (1024 * 1024));
    
    return 0;
}

virtual_machine_t *vm_create(const char *name, uint32_t memory_mb) {
    if (memory_mb * 1024 * 1024 > hypervisor.available_memory) {
        printf("Error: No hay suficiente memoria para la VM\n");
        return NULL;
    }
    
    virtual_machine_t *vm = kmalloc(sizeof(virtual_machine_t));
    if (!vm) {
        printf("Error: No se pudo crear VM\n");
        return NULL;
    }
    
    // Configurar VM
    vm->vm_id = hypervisor.num_vms + 1;
    strncpy(vm->name, name, 63);
    vm->name[63] = '\0';
    vm->state = VM_STATE_STOPPED;
    vm->memory_size = memory_mb * 1024 * 1024;
    vm->cpu_quota = 25;  // 25% por defecto
    
    // Asignar memoria física
    vm->physical_memory_base = allocate_physical_memory(vm->memory_size);
    if (vm->physical_memory_base == 0) {
        printf("Error: No se pudo asignar memoria física para VM\n");
        kfree(vm);
        return NULL;
    }
    
    // Crear tabla de páginas para la VM
    vm->page_directory = create_vm_page_directory(vm);
    
    // Inicializar estado de VCPU
    memset(&vm->vcpu_state, 0, sizeof(cpu_context_t));
    vm->vcpu_state.esp = vm->memory_size - 4;  // Stack al final de la memoria
    vm->vcpu_state.eflags = 0x200;  // Interrupts enabled
    
    // Agregar a lista de VMs
    vm->next = hypervisor.vm_list;
    hypervisor.vm_list = vm;
    hypervisor.num_vms++;
    hypervisor.available_memory -= vm->memory_size;
    
    printf("VM creada: ID=%d, Nombre='%s', RAM=%d MB\n", 
           vm->vm_id, vm->name, memory_mb);
    
    return vm;
}

int vm_start(virtual_machine_t *vm) {
    if (!vm || vm->state == VM_STATE_RUNNING) {
        return -1;
    }
    
    printf("Iniciando VM: %s\n", vm->name);
    
    // Cargar imagen del sistema operativo guest
    if (load_guest_os_image(vm) != 0) {
        printf("Error: No se pudo cargar imagen del guest OS\n");
        return -1;
    }
    
    // Configurar dispositivos virtuales
    setup_virtual_devices(vm);
    
    // Marcar como ejecutándose
    vm->state = VM_STATE_RUNNING;
    vm->uptime = get_system_time();
    
    printf("VM %s iniciada correctamente\n", vm->name);
    return 0;
}

void hypervisor_schedule(void) {
    if (hypervisor.num_vms == 0) {
        return;
    }
    
    // Scheduler round-robin simple para VMs
    static virtual_machine_t *last_scheduled = NULL;
    
    virtual_machine_t *vm = last_scheduled ? last_scheduled->next : hypervisor.vm_list;
    if (!vm) {
        vm = hypervisor.vm_list;
    }
    
    // Buscar próxima VM ejecutable
    virtual_machine_t *start_vm = vm;
    do {
        if (vm->state == VM_STATE_RUNNING) {
            // Cambiar contexto a esta VM
            vm_context_switch(hypervisor.current_vm, vm);
            hypervisor.current_vm = vm;
            last_scheduled = vm;
            
            printf("Ejecutando VM: %s (ID=%d)\n", vm->name, vm->vm_id);
            return;
        }
        
        vm = vm->next ? vm->next : hypervisor.vm_list;
    } while (vm != start_vm);
    
    // No hay VMs ejecutables
    hypervisor.current_vm = NULL;
}

void vm_context_switch(virtual_machine_t *old_vm, virtual_machine_t *new_vm) {
    if (old_vm) {
        // Guardar estado de la VM anterior
        save_vm_state(old_vm);
    }
    
    if (new_vm) {
        // Cargar estado de la nueva VM
        load_vm_state(new_vm);
        
        // Cambiar tabla de páginas
        set_page_directory(new_vm->page_directory);
        
        // Configurar timer para quantum
        set_timer_quantum(hypervisor.vm_quantum);
    }
}
```

### 2.4 Contenedores Básicos

#### Namespaces y Control Groups

```c
// containers.c
#include "containers.h"
#include "security.h"

// Tipos de namespaces
typedef enum {
    NS_PID,        // Process IDs
    NS_NET,        // Network
    NS_MOUNT,      // Mount points
    NS_UTS,        // Hostname
    NS_USER,       // User/Group IDs
    NS_IPC,        // Inter-process communication
    NS_MAX
} namespace_type_t;

// Estructura de namespace
typedef struct {
    namespace_type_t type;
    uint32_t ns_id;
    uint32_t ref_count;
    void *private_data;
} namespace_t;

// Contenedor
typedef struct {
    uint32_t container_id;
    char name[64];
    
    // Namespaces
    namespace_t *namespaces[NS_MAX];
    
    // Límites de recursos (cgroups)
    uint32_t memory_limit;
    uint32_t cpu_limit;
    uint32_t pids_limit;
    
    // Estadísticas de uso
    uint32_t memory_used;
    uint32_t cpu_used;
    uint32_t pids_count;
    
    // Procesos en el contenedor
    process_t **processes;
    uint32_t num_processes;
    
    // Estado
    bool is_running;
} container_t;

container_t *container_create(const char *name) {
    container_t *container = kmalloc(sizeof(container_t));
    if (!container) {
        return NULL;
    }
    
    // Configurar contenedor
    container->container_id = generate_container_id();
    strncpy(container->name, name, 63);
    container->name[63] = '\0';
    
    // Crear namespaces
    for (int i = 0; i < NS_MAX; i++) {
        container->namespaces[i] = create_namespace(i);
    }
    
    // Límites por defecto
    container->memory_limit = 512 * 1024 * 1024;  // 512 MB
    container->cpu_limit = 50;  // 50%
    container->pids_limit = 100;
    
    container->is_running = false;
    
    printf("Contenedor creado: %s (ID=%d)\n", name, container->container_id);
    return container;
}

int container_run_process(container_t *container, const char *command) {
    if (!container) {
        return -1;
    }
    
    // Crear proceso en el contenedor
    process_t *proc = process_create(command, NULL);
    if (!proc) {
        return -1;
    }
    
    // Asignar al contenedor
    proc->container = container;
    
    // Configurar namespaces para el proceso
    for (int i = 0; i < NS_MAX; i++) {
        attach_to_namespace(proc, container->namespaces[i]);
    }
    
    // Aplicar límites de recursos
    apply_resource_limits(proc, container);
    
    // Agregar a lista de procesos del contenedor
    add_process_to_container(container, proc);
    
    container->is_running = true;
    
    printf("Proceso '%s' ejecutado en contenedor '%s'\n", command, container->name);
    return 0;
}

void apply_resource_limits(process_t *proc, container_t *container) {
    // Límite de memoria
    proc->memory_limit = container->memory_limit / container->pids_limit;
    
    // Límite de CPU (prioridad)
    if (container->cpu_limit < 25) {
        proc->priority = PRIORITY_LOW;
    } else if (container->cpu_limit < 75) {
        proc->priority = PRIORITY_NORMAL;
    } else {
        proc->priority = PRIORITY_HIGH;
    }
    
    // Configurar cgroups
    setup_cgroup_limits(proc, container);
}
```

---

## 🧪 Ejercicios Prácticos

### Ejercicio 1: Sistema de Login (Nivel: Intermedio)

**Objetivo**: Implementar un sistema de autenticación básico.

**Instrucciones**:
1. Crear base de datos de usuarios
2. Implementar hash de contraseñas
3. Sesiones de usuario
4. Comando `login` y `logout`

**Código Base**:
```c
// Completar la implementación
void cmd_login(char *args) {
    char username[32], password[32];
    
    printf("Username: ");
    getline(username, sizeof(username));
    
    printf("Password: ");
    // TODO: Implementar input oculto
    getline(password, sizeof(password));
    
    // TODO: Verificar credenciales
    // TODO: Crear sesión
}

// Hash simple para contraseñas (en producción usar bcrypt/scrypt)
uint32_t simple_hash(const char *password) {
    // TODO: Implementar función hash segura
    return 0;
}
```

### Ejercicio 2: Protección contra Buffer Overflow (Nivel: Avanzado)

**Objetivo**: Implementar mecanismos de protección de stack.

**Instrucciones**:
1. Implementar stack canaries
2. Detectar overflow automáticamente
3. Terminar proceso de forma segura
4. Logging de intentos de ataque

**Código Base**:
```c
// Marco de protección de función
#define PROTECTED_FUNCTION(name) \
    void name() { \
        uint32_t canary = STACK_CANARY; \
        /* función aquí */ \
        if (canary != STACK_CANARY) stack_smash_handler(); \
    }

// TODO: Implementar detección automática
// TODO: Añadir randomización de canaries
// TODO: Integrar con sistema de logging
```

### Ejercicio 3: Hypervisor Simple (Nivel: Extremo)

**Objetivo**: Crear un hypervisor que ejecute VMs simples.

**Instrucciones**:
1. Implementar aislamiento de memoria
2. Scheduler de máquinas virtuales
3. Emulación de dispositivos básicos
4. Interfaz de gestión de VMs

**Código Base**:
```c
// Comandos de gestión de VMs
void cmd_vm_create(char *args) {
    // TODO: Parsear parámetros
    // TODO: Crear VM
    // TODO: Configurar recursos
}

void cmd_vm_list(char *args) {
    // TODO: Mostrar lista de VMs
    // TODO: Estado y recursos de cada VM
}

void cmd_vm_start(char *args) {
    // TODO: Iniciar VM específica
    // TODO: Verificar recursos disponibles
}
```

### Ejercicio 4: Sistema de Contenedores (Nivel: Experto)

**Objetivo**: Implementar aislamiento estilo Docker.

**Instrucciones**:
1. Crear namespaces para aislamiento
2. Implementar cgroups para límites
3. Sistema de imágenes de contenedor
4. Networking entre contenedores

**Código Base**:
```c
// Estructura de imagen de contenedor
typedef struct {
    char name[64];
    char version[16];
    uint32_t size;
    uint8_t *filesystem_image;
    char entrypoint[256];
} container_image_t;

// TODO: Implementar funciones de contenedor
int container_build_image(const char *dockerfile_path);
int container_run_from_image(const char *image_name);
int container_exec(const char *container_name, const char *command);
int container_stop(const char *container_name);
```

---

## 🔧 Integración con el Shell

### Comandos de Seguridad

```c
// Comandos de seguridad para el shell
void cmd_whoami(char *args) {
    if (current_user) {
        printf("%s (UID=%d, GID=%d)\n", 
               current_user->username, current_user->uid, current_user->gid);
    } else {
        printf("No hay usuario autenticado\n");
    }
}

void cmd_chmod(char *args) {
    char filename[64];
    int mode;
    
    if (sscanf(args, "%o %s", &mode, filename) != 2) {
        printf("Uso: chmod <modo_octal> <archivo>\n");
        return;
    }
    
    file_t *file = filesystem_open(filename);
    if (!file) {
        printf("Error: Archivo '%s' no encontrado\n", filename);
        return;
    }
    
    // Verificar permisos para cambiar atributos
    if (!security_check_permission(file, scheduler.current_process, ACCESS_WRITE)) {
        printf("Error: Sin permisos para modificar '%s'\n", filename);
        filesystem_close(file);
        return;
    }
    
    // Aplicar nuevos permisos
    set_file_permissions(file, mode);
    printf("Permisos cambiados: %s -> %03o\n", filename, mode);
    
    filesystem_close(file);
}

void cmd_su(char *args) {
    char username[32];
    
    if (strlen(args) == 0) {
        strcpy(username, "root");
    } else {
        strcpy(username, args);
    }
    
    printf("Cambiando a usuario: %s\n", username);
    
    // En un sistema real pediría contraseña
    if (user_authenticate(username, "password") == 0) {
        printf("Usuario cambiado exitosamente\n");
    } else {
        printf("Error: No se pudo cambiar de usuario\n");
    }
}

void cmd_ps_secure(char *args) {
    printf("=== PROCESOS (Vista Segura) ===\n");
    printf("PID\tUID\tGID\tNOMBRE\t\tESTADO\n");
    
    process_t *current = scheduler.ready_queue;
    if (current) {
        do {
            // Solo mostrar procesos que el usuario puede ver
            if (can_view_process(current_user, current)) {
                printf("%d\t%d\t%d\t%s\t\t%d\n",
                       current->pid,
                       current->credentials.effective_uid,
                       current->credentials.effective_gid,
                       current->name,
                       current->state);
            }
            current = current->next;
        } while (current != scheduler.ready_queue);
    }
    
    printf("===============================\n");
}
```

### Comandos de Virtualización

```c
void cmd_vm_create(char *args) {
    char name[64];
    int memory_mb;
    
    if (sscanf(args, "%s %d", name, &memory_mb) != 2) {
        printf("Uso: vm_create <nombre> <memoria_mb>\n");
        return;
    }
    
    virtual_machine_t *vm = vm_create(name, memory_mb);
    if (vm) {
        printf("VM '%s' creada con %d MB de RAM\n", name, memory_mb);
    } else {
        printf("Error: No se pudo crear la VM\n");
    }
}

void cmd_vm_list(char *args) {
    printf("=== MÁQUINAS VIRTUALES ===\n");
    printf("ID\tNOMBRE\t\tESTADO\t\tRAM\tCPU%%\n");
    
    virtual_machine_t *vm = hypervisor.vm_list;
    while (vm) {
        const char *state_str;
        switch (vm->state) {
        case VM_STATE_STOPPED:   state_str = "DETENIDA"; break;
        case VM_STATE_RUNNING:   state_str = "EJECUTANDO"; break;
        case VM_STATE_PAUSED:    state_str = "PAUSADA"; break;
        case VM_STATE_SUSPENDED: state_str = "SUSPENDIDA"; break;
        default:                 state_str = "DESCONOCIDO"; break;
        }
        
        printf("%d\t%-12s\t%s\t%dMB\t%d%%\n",
               vm->vm_id, vm->name, state_str,
               vm->memory_size / (1024 * 1024), vm->cpu_quota);
        
        vm = vm->next;
    }
    printf("===========================\n");
}

void cmd_container_run(char *args) {
    char image_name[64], container_name[64];
    
    if (sscanf(args, "%s %s", image_name, container_name) != 2) {
        printf("Uso: container_run <imagen> <nombre_contenedor>\n");
        return;
    }
    
    container_t *container = container_create(container_name);
    if (container) {
        if (container_run_process(container, image_name) == 0) {
            printf("Contenedor '%s' ejecutándose\n", container_name);
        } else {
            printf("Error: No se pudo ejecutar contenedor\n");
        }
    } else {
        printf("Error: No se pudo crear contenedor\n");
    }
}
```

---

## 📊 Análisis de Vulnerabilidades

### Herramientas de Auditoría

```c
// Auditoría de seguridad del sistema
void security_audit(void) {
    printf("\n=== AUDITORÍA DE SEGURIDAD ===\n");
    
    // 1. Verificar configuración de usuarios
    audit_user_configuration();
    
    // 2. Verificar permisos de archivos críticos
    audit_file_permissions();
    
    // 3. Verificar procesos en ejecución
    audit_running_processes();
    
    // 4. Verificar configuración de red
    audit_network_configuration();
    
    // 5. Verificar logs de seguridad
    audit_security_logs();
    
    printf("==============================\n");
}

void audit_user_configuration(void) {
    printf("\n--- Configuración de Usuarios ---\n");
    
    for (int i = 0; i < num_users; i++) {
        user_info_t *user = &users[i];
        printf("Usuario: %s (UID=%d)\n", user->username, user->uid);
        
        // Verificar configuración peligrosa
        if (user->uid == 0 && strcmp(user->username, "root") != 0) {
            printf("  ⚠️  ADVERTENCIA: Usuario no-root con UID 0\n");
        }
        
        if (user->is_admin && user->uid != 0) {
            printf("  ⚠️  ADVERTENCIA: Usuario admin sin UID 0\n");
        }
    }
}

void audit_file_permissions(void) {
    printf("\n--- Permisos de Archivos ---\n");
    
    // Lista de archivos críticos a verificar
    const char *critical_files[] = {
        "/etc/passwd", "/etc/shadow", "/bin/su", "/bin/sudo",
        "/boot/kernel", "/dev/mem", NULL
    };
    
    for (int i = 0; critical_files[i]; i++) {
        file_t *file = filesystem_open(critical_files[i]);
        if (file) {
            printf("Archivo: %s\n", critical_files[i]);
            
            // Verificar permisos peligrosos
            if (file->permissions.other_write) {
                printf("  🚨 CRÍTICO: Archivo crítico escribible por todos\n");
            }
            
            if (file->permissions.setuid && file->owner_uid != 0) {
                printf("  ⚠️  ADVERTENCIA: SETUID en archivo no-root\n");
            }
            
            filesystem_close(file);
        }
    }
}
```

---

## 🎯 Proyecto Final: Sistema Seguro Completo

### Especificaciones

Implementar un sistema operativo con características de seguridad avanzadas:

1. **Sistema de Autenticación**:
   - Usuarios y grupos
   - Contraseñas hasheadas
   - Sesiones seguras
   - Two-factor authentication básico

2. **Control de Acceso**:
   - Permisos estilo UNIX
   - Capabilities granulares
   - MAC (Mandatory Access Control)
   - Sandboxing de procesos

3. **Protección de Memoria**:
   - Stack canaries
   - ASLR (Address Space Layout Randomization)
   - NX bit emulation
   - Heap protection

4. **Virtualización**:
   - Hypervisor tipo 1 básico
   - Contenedores con namespaces
   - Aislamiento de recursos
   - Migration de VMs

5. **Auditoría y Logging**:
   - Sistema de logs centralizados
   - Detección de intrusiones
   - Alertas de seguridad
   - Forense básico

### Criterios de Evaluación

1. **Seguridad** (40%):
   - Resistencia a ataques comunes
   - Implementación correcta de protecciones
   - Principio de menor privilegio

2. **Virtualización** (30%):
   - Aislamiento efectivo
   - Rendimiento de VMs/contenedores
   - Gestión de recursos

3. **Usabilidad** (20%):
   - Interfaz de usuario intuitiva
   - Documentación clara
   - Facilidad de administración

4. **Código** (10%):
   - Calidad y estructura
   - Manejo de errores
   - Comentarios y documentación

---

## 🏆 Resumen del Curso Completo

### Conocimientos Adquiridos

A lo largo de este curso de 8 capítulos, hemos cubierto:

1. **Capítulo 1 - Arquitectura x86 y Bootloaders**: Fundamentos del hardware y arranque
2. **Capítulo 2 - Gestión de Memoria y E/S**: Manejo de memoria y controladores
3. **Capítulo 3 - Sistemas de Archivos**: Implementación de FAT16 y operaciones de archivo
4. **Capítulo 4 - Shell y IPC**: Intérprete de comandos y comunicación inter-proceso
5. **Capítulo 5 - Interrupciones y Excepciones**: Manejo de eventos y hardware
6. **Capítulo 6 - Multitasking y Scheduler**: Ejecución concurrente y planificación
7. **Capítulo 7 - Redes y Protocolos**: Comunicación de red y protocolos
8. **Capítulo 8 - Seguridad y Virtualización**: Protección y entornos virtualizados

### Habilidades Desarrolladas

- ✅ Programación de sistemas de bajo nivel
- ✅ Arquitectura de computadores x86
- ✅ Desarrollo de kernels de sistemas operativos
- ✅ Implementación de drivers de hardware
- ✅ Gestión de memoria y procesos
- ✅ Protocolos de comunicación de red
- ✅ Sistemas de seguridad y protección
- ✅ Tecnologías de virtualización
- ✅ Debugging y optimización de sistemas

### Proyectos Completados

1. **Bootloader funcional** con soporte Multiboot
2. **Kernel educativo** con más de 2000 líneas de código
3. **Sistema de archivos FAT16** completamente funcional
4. **Shell interactivo** con más de 35 comandos
5. **Sistema de pipes** para comunicación entre comandos
6. **Stack de red** con protocolos TCP/IP, UDP, ICMP
7. **Scheduler multinivel** con soporte para multitasking
8. **Sistema de seguridad** con usuarios, permisos y protecciones

### Preparación Profesional

Este curso te ha preparado para:

- **Desarrollo de sistemas embebidos**
- **Ingeniería de sistemas operativos**
- **Desarrollo de drivers de dispositivos**
- **Arquitectura de software de sistemas**
- **Cyberseguridad y análisis de vulnerabilidades**
- **Cloud computing y virtualización**
- **Investigación en sistemas computacionales**

---

## 📚 Recursos para Continuar

### Libros Avanzados

1. **"Understanding the Linux Kernel" - Bovet & Cesati**
2. **"Windows Internals" - Russinovich & Solomon**
3. **"The Art of Computer Systems Performance Analysis" - Jain**
4. **"Hacking: The Art of Exploitation" - Erickson**

### Proyectos de Código Abierto

- **Linux Kernel**: https://kernel.org/
- **FreeBSD**: https://www.freebsd.org/
- **MINIX**: https://www.minix3.org/
- **ReactOS**: https://reactos.org/

### Cursos Avanzados

- MIT 6.828 Operating System Engineering
- Stanford CS140 Operating Systems
- UC Berkeley CS162 Operating Systems
- CMU 15-410 Operating System Design

---

*"La seguridad no es un producto, es un proceso."* - Bruce Schneier

**¡Felicitaciones!** 🎉 Has completado el curso completo de Sistemas Operativos. Ahora tienes los conocimientos fundamentales para desarrollar, analizar y asegurar sistemas operativos modernos.

**¡El viaje hacia la maestría en sistemas operativos apenas comienza!** 🚀