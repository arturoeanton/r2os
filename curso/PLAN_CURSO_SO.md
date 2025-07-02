# Plan de Curso: Sistemas Operativos Avanzado
## 4 Meses - Basado en Mini Kernel Educativo (8 Capítulos Completos)

---

## 📋 Información General

**Duración**: 16 semanas (4 meses)  
**Modalidad**: Teórico-práctica  
**Prerrequisitos**: Programación en C, Arquitectura de computadoras básica  
**Proyecto Base**: Mini Kernel Educativo en C  

---

## 🎯 Objetivos del Curso

### Objetivo General
Desarrollar competencias avanzadas en diseño e implementación de sistemas operativos mediante la extensión progresiva de un kernel educativo.

### Objetivos Específicos
- Comprender la arquitectura interna de un sistema operativo
- Implementar componentes fundamentales: gestión de memoria, procesos, I/O
- Desarrollar drivers de dispositivos básicos
- Diseñar e implementar sistemas de archivos
- Aplicar conceptos de concurrencia y sincronización
- Evaluar y optimizar el rendimiento del sistema

---

## 📅 Plan Semanal Detallado

### **MES 1: Fundamentos y Arquitectura del Sistema**

#### **Semana 1: Introducción y Setup del Entorno**
**Teoría (3 horas):**
- Historia y evolución de los sistemas operativos
- Arquitectura x86: modos de operación, registros, interrupciones
- Proceso de boot: BIOS, bootloaders, Multiboot specification
- Cross-compilation y toolchains

**Práctica (3 horas):**
- Setup del entorno de desarrollo (i686-elf-gcc, QEMU)
- Compilación y ejecución del mini kernel base
- Análisis del código fuente: boot.s y kernel.c
- Modificación básica: cambiar mensaje de bienvenida

**Entregable:** Reporte de instalación y primera modificación del kernel

---

#### **Semana 2: Gestión de Memoria Básica**
**Teoría (3 horas):**
- Modelo de memoria x86: segmentación y paginación
- Layout de memoria del kernel
- Gestión de memoria fija vs dinámica
- Stack, heap y memoria estática

**Práctica (3 horas):**
- Análisis del linker.ld y layout de memoria
- Implementación de funciones básicas de memoria (malloc/free simple)
- Comando `memory` para mostrar mapa de memoria
- Detección de corrupción de memoria básica

**Entregable:** Implementación de gestor de memoria simple

---

#### **Semana 3: Sistema de Archivos - Parte I**
**Teoría (3 horas):**
- Conceptos de sistemas de archivos: FAT, ext2, NTFS
- Estructura de FAT16: boot sector, FAT, directorio raíz
- Operaciones básicas: crear, leer, escribir, eliminar
- Metadata y organización de datos

**Práctica (3 horas):**
- Análisis detallado del sistema FAT16 implementado
- Mejora del comando `ls`: mostrar tamaño, añadir columnas
- Implementación de comando `du` (disk usage)
- Verificación de integridad del sistema de archivos

**Entregable:** Sistema de archivos mejorado con nuevos comandos

---

#### **Semana 4: Entrada/Salida y Drivers**
**Teoría (3 horas):**
- Arquitectura de I/O: polling, interrupciones, DMA
- Controladores de dispositivos: abstracción hardware/software
- PS/2 keyboard protocol en detalle
- VGA text mode y gráficos básicos

**Práctica (3 horas):**
- Mejora del driver de teclado: soporte completo de caracteres especiales
- Implementación básica de mouse PS/2
- Comando `devinfo` para mostrar dispositivos detectados
- Timer básico usando PIT (Programmable Interval Timer)

**Entregable:** Drivers mejorados con soporte para mouse y timer

---

### **MES 2: Procesos y Concurrencia**

#### **Semana 5: Conceptos de Procesos**
**Teoría (3 horas):**
- Process Control Block (PCB)
- Estados de procesos: running, ready, blocked
- Context switching y save/restore de registros
- Creación de procesos: fork(), exec()

**Práctica (3 horas):**
- Diseño de estructura de datos para PCB
- Implementación básica de context switching
- Comando `ps` para listar procesos
- Proceso simple que ejecuta en background

**Entregable:** Framework básico de multitasking

---

#### **Semana 6: Scheduling y Dispatching**
**Teoría (3 horas):**
- Algoritmos de scheduling: FIFO, SJF, Round Robin, prioridades
- Preemptive vs non-preemptive scheduling
- Métricas de rendimiento: throughput, latency, fairness
- Real-time scheduling concepts

**Práctica (3 horas):**
- Implementación de scheduler Round Robin
- Timer interrupt para preemption
- Comando `top` para mostrar estadísticas de procesos
- Proceso de prueba que consume CPU

**Entregable:** Sistema multitasking con scheduler Round Robin

---

#### **Semana 7: Sincronización - Parte I**
**Teoría (3 horas):**
- Race conditions y regiones críticas
- Semáforos: binarios y contadores
- Mutexes y locks
- Deadlock: detección y prevención

**Práctica (3 horas):**
- Implementación de semáforos básicos
- Ejemplo clásico: productor-consumidor
- Comando `semstat` para mostrar estado de semáforos
- Detección simple de deadlock

**Entregable:** Sistema de sincronización con semáforos

---

#### **Semana 8: Comunicación Inter-Proceso**
**Teoría (3 horas):**
- Pipes y FIFOs
- Message queues
- Shared memory
- Signals y event handling

**Práctica (3 horas):**
- Implementación de pipes básicos en el shell
- Comando pipe: `cat archivo.txt | grep palabra`
- Shared memory entre procesos
- Sistema básico de signals

**Entregable:** IPC funcional con pipes y shared memory

---

### **MES 3: Gestión Avanzada de Memoria y I/O**

#### **Semana 9: Memoria Virtual - Parte I**
**Teoría (3 horas):**
- Conceptos de memoria virtual vs física
- Paginación: page tables, TLB, page faults
- Algoritmos de reemplazo: LRU, FIFO, Clock
- Working set y localidad de referencia

**Práctica (3 horas):**
- Setup básico de paginación x86
- Page fault handler básico
- Comando `vmstat` para estadísticas de memoria virtual
- Simulación de swapping básico

**Entregable:** Sistema de paginación básico

---

#### **Semana 10: Memoria Virtual - Parte II**
**Teoría (3 horas):**
- Memory mapped files
- Copy-on-write (COW)
- Protection y seguridad de memoria
- NUMA y memoria distribuida

**Práctica (3 horas):**
- Implementación de mmap básico
- Copy-on-write para fork()
- Protección de páginas (read-only, execute)
- Herramientas de debug de memoria

**Entregable:** Sistema de memoria virtual completo

---

#### **Semana 11: Sistema de Archivos - Parte II**
**Teoría (3 horas):**
- Journaling y consistencia
- Índices y B-trees
- Cachéing y buffering
- Sistemas de archivos distribuidos

**Práctica (3 horas):**
- Buffer cache para el sistema de archivos
- Implementación de subdirectorios en FAT16
- Comando `fsck` para verificación del filesystem
- Links simbólicos básicos

**Entregable:** Sistema de archivos jerárquico con caché

---

#### **Semana 12: Drivers Avanzados**
**Teoría (3 horas):**
- DMA programming
- Interrupt handling avanzado
- Device driver architecture
- Hot-plugging y device management

**Práctica (3 horas):**
- Driver básico para disco (simulado)
- Sistema de interrupciones mejorado
- PCI device enumeration básico
- Framework para drivers dinámicos

**Entregable:** Sistema de drivers extensible

---

### **MES 4: Redes, Seguridad y Virtualización**

#### **Semana 13: Redes y Protocolos**
**Teoría (3 horas):** [CAPÍTULO 7]
- Stack de protocolos: Ethernet, IP, TCP/UDP, ICMP
- Socket programming en kernel space
- Network device drivers y NIC management
- Routing, switching y protocolos de red

**Práctica (3 horas):**
- Driver básico para tarjeta de red (simulada)
- Implementación completa de ping y protocolos ICMP
- Stack TCP/IP con UDP y sockets básicos
- Comandos de red: `ping`, `netstat`, `ifconfig`, `arp`

**Entregable:** Stack de red funcional con múltiples protocolos

---

#### **Semana 14: Seguridad y Virtualización - Parte I**
**Teoría (3 horas):** [CAPÍTULO 8 - Parte I]
- User mode vs kernel mode y anillos de protección
- System calls y privilege escalation
- Access control: permisos, usuarios, grupos
- Vulnerability mitigation: ASLR, stack canaries, NX bit

**Práctica (3 horas):**
- Implementación de user mode y separación de privilegios
- Sistema completo de usuarios y permisos estilo UNIX
- Protección contra buffer overflows y ataques comunes
- Comandos de seguridad: `login`, `su`, `chmod`, `whoami`

**Entregable:** Sistema con seguridad robusta y control de acceso

---

#### **Semana 15: Virtualización y Contenedores**
**Teoría (3 horas):** [CAPÍTULO 8 - Parte II]
- Tipos de virtualización: Type 1, Type 2, para-virtualización
- Hypervisors y tecnologías Intel VT-x/AMD-V
- Contenedores: namespaces, cgroups, aislamiento
- Docker-style containers vs VMs

**Práctica (3 horas):**
- Implementación de hypervisor básico para VMs
- Sistema de contenedores con aislamiento de procesos
- Gestión de recursos virtualizados
- Comandos: `vm_create`, `vm_start`, `container_run`

**Entregable:** Sistema con capacidades de virtualización

---

#### **Semana 16: Integración Final y Proyecto Avanzado**
**Teoría (2 horas):**
- Review completo de los 8 capítulos implementados
- Sistemas operativos modernos: Linux, Windows, macOS
- Tendencias: microservices, edge computing, IoT OS
- Futuro: OS para AI/ML, quantum computing

**Práctica (4 horas):**
- Integración de todos los componentes (8 capítulos)
- Sistema operativo educativo completo y funcional
- Testing exhaustivo y benchmarks de rendimiento
- Documentación técnica completa y presentaciones

**Entregable:** Sistema operativo educativo completo con todos los componentes

---

## 🚀 TRABAJO PRÁCTICO FINAL INTEGRADOR
### "Sistema Operativo Completo con 8 Componentes Avanzados"

#### **Objetivo:**
Integrar todos los 8 capítulos del curso en un sistema operativo educativo completo, implementando desde bootloader hasta virtualización, con énfasis en funcionalidad real y robustez.

#### **Requerimientos del Proyecto (Obligatorios - Basados en los 8 Capítulos):**

**1. Arquitectura x86 y Bootloader Avanzado (CAPÍTULO 1)**
- Bootloader Multiboot completo con detección de hardware
- Soporte para múltiples modos de CPU (Real, Protected, Long)
- Detección automática de memoria y dispositivos
- Carga de módulos iniciales y configuración dinámica

**2. Gestión de Memoria Completa (CAPÍTULO 2)**
- Implementar paginación completa con gestión de page faults
- Sistema de memoria virtual con swap básico
- Heap dinámico con malloc/free optimizado y detección de leaks
- Protección de memoria con segmentación y control de acceso

**3. Sistema de Archivos Jerárquico (CAPÍTULO 3)**
- FAT16 extendido con subdirectorios completos
- Operaciones avanzadas: enlaces, permisos, metadata
- Sistema de cache de archivos para rendimiento
- Journaling básico para integridad y recuperación

**4. Shell y IPC Completo (CAPÍTULO 4)**
- Shell con +40 comandos, pipes múltiples, y scripting básico
- Sistema completo de IPC: pipes, shared memory, signals
- Job control avanzado (background, foreground, suspend)
- Redirección completa de E/S y variables de entorno

**5. Interrupciones y Manejo de Eventos (CAPÍTULO 5)**
- IDT completo con handlers para todas las excepciones x86
- Sistema de interrupciones anidadas y prioridades
- Drivers completos para PIT, PIC, keyboard, mouse
- Manejo robusto de errores y recovery automático

**6. Multitasking Preemptivo (CAPÍTULO 6)**
- Scheduler multinivel con Round Robin, Priority, y MLFQ
- Context switching optimizado con soporte para FPU
- Sistema completo de procesos con fork(), exec(), wait()
- Sincronización con semáforos, mutexes, y condition variables

**7. Stack de Red Funcional (CAPÍTULO 7)**
- Protocolos Ethernet, IP, TCP, UDP, ICMP completamente implementados
- Driver de red simulado con buffer management
- Socket API básico para aplicaciones de red
- Herramientas de red: ping, netstat, ifconfig, servidor echo

**8. Seguridad y Virtualización (CAPÍTULO 8)**
- Sistema de usuarios con autenticación y permisos UNIX-style
- Protecciones contra buffer overflow, ASLR, stack canaries
- Hypervisor básico capaz de ejecutar VMs simples
- Sistema de contenedores con aislamiento de namespaces

#### **Características Técnicas Obligatorias:**

**A. Gestión de Memoria Avanzada**
```c
// Implementar heap dinámico con malloc/free
void* malloc(size_t size);
void free(void* ptr);
void* realloc(void* ptr, size_t new_size);

// Sistema de páginas básico
int mmap(void* addr, size_t length, int prot);
int munmap(void* addr, size_t length);
```

**B. Sistema de Procesos**
```c
// Fork básico para crear procesos
pid_t fork(void);
int exec(const char* path, char* argv[]);
void exit(int status);
pid_t wait(int* status);

// Scheduler con al menos 3 algoritmos seleccionables
void set_scheduler(int algorithm); // 0=FIFO, 1=RR, 2=Priority
```

**C. Drivers de Dispositivos**
```c
// Driver de mouse con eventos
typedef struct {
    int x, y;
    int buttons;
    int wheel;
} mouse_event_t;

int mouse_read(mouse_event_t* event);

// Driver de sonido básico (beep/tono)
void play_tone(int frequency, int duration_ms);
void play_melody(int* frequencies, int* durations, int count);
```

**D. Sistema de Red**
```c
// Socket básico para red
int socket(int domain, int type, int protocol);
int bind(int sockfd, struct sockaddr* addr, socklen_t addrlen);
int listen(int sockfd, int backlog);
int accept(int sockfd, struct sockaddr* addr, socklen_t* addrlen);
int connect(int sockfd, struct sockaddr* addr, socklen_t addrlen);
```

#### **Aplicaciones Requeridas (Mínimo 5):**

**1. Shell Avanzado**
- Soporte para variables de entorno
- Redirección avanzada (`>`, `>>`, `<`, `2>`)
- Pipes múltiples (`cmd1 | cmd2 | cmd3`)
- Job control (`jobs`, `fg`, `bg`, `nohup`)
- Autocompletado de comandos y archivos

**2. Editor de Texto Gráfico**
- Interfaz con menús y botones
- Soporte para archivos de múltiples páginas
- Find/Replace básico
- Syntax highlighting para al menos 2 lenguajes

**3. Administrador de Archivos**
- Vista de íconos y lista
- Operaciones drag & drop básicas
- Previsualización de archivos de texto
- Propiedades de archivos con edición

**4. Monitor del Sistema**
- Vista en tiempo real de procesos activos
- Gráficos de uso de CPU y memoria
- Kill/pause/resume de procesos
- Información de dispositivos conectados

**5. Aplicación de Red**
- Cliente chat básico
- Navegador web minimalista (solo HTML básico)
- Transferencia de archivos P2P
- Monitor de red con estadísticas

#### **Criterios de Evaluación (100 puntos):**

**Funcionalidad Básica (30 puntos)**
- Sistema arranca y funciona estable (10 pts)
- Todos los componentes obligatorios implementados (20 pts)

**Calidad Técnica (25 puntos)**
- Código bien documentado y modular (10 pts)
- Manejo adecuado de errores (5 pts)
- Eficiencia en algoritmos críticos (10 pts)

**Interfaz y Usabilidad (20 puntos)**
- Interfaz gráfica funcional y atractiva (10 pts)
- Aplicaciones fáciles de usar (10 pts)

**Innovación y Extras (15 puntos)**
- Características no requeridas pero útiles (10 pts)
- Soluciones creativas a problemas técnicos (5 pts)

**Documentación y Presentación (10 puntos)**
- Manual de usuario completo (5 pts)
- Presentación técnica clara (5 pts)

#### **Entregables y Fechas:**

**Milestone 1 (Semana 4):** Sistema de archivos avanzado funcional
**Milestone 2 (Semana 8):** Multitasking con al menos 2 procesos concurrentes
**Milestone 3 (Semana 12):** Interfaz gráfica con 2 aplicaciones funcionando
**Entrega Final (Semana 16):** Sistema completo con todas las aplicaciones

#### **Recursos y Soporte:**

**Código Base Proporcionado:**
- Framework básico para modo gráfico VGA
- Skeleton de drivers de mouse y red
- Bibliotecas auxiliares para matemáticas y string handling
- Suite de testing automatizado

**Documentación Técnica:**
- Especificaciones detalladas de cada componente
- Ejemplos de código para patrones complejos
- Diagramas de arquitectura del sistema
- Referencias a implementaciones reales

#### **Bonificaciones Adicionales (+20 puntos):**

**Implementaciones Avanzadas:**
- Sistema de audio con reproducción de archivos WAV (+5 pts)
- Soporte para USB básico (teclado/mouse) (+10 pts)
- Compilador C básico que genere código para el kernel (+15 pts)
- Emulador de máquina virtual dentro del OS (+20 pts)

**Este proyecto desafía a los estudiantes a aplicar todos los conceptos del curso mientras implementan funcionalidades reales de un sistema operativo moderno.**

---

## 📚 Recursos de Estudio

### Libros de Texto Principal
1. **"Operating System Concepts"** - Silberschatz, Galvin, Gagne (10th Edition)
2. **"Modern Operating Systems"** - Andrew Tanenbaum (4th Edition)
3. **"Operating Systems: Three Easy Pieces"** - Remzi Arpaci-Dusseau (Free online)

### Recursos Online
- [OSDev Wiki](https://wiki.osdev.org/) - Documentación técnica detallada
- [Intel Software Developer Manuals](https://software.intel.com/content/www/us/en/develop/articles/intel-sdm.html)
- [MIT 6.828 Operating System Engineering](https://pdos.csail.mit.edu/6.828/)

### Herramientas de Desarrollo
- GCC Cross-compiler (i686-elf)
- QEMU System Emulator
- GDB para debugging
- Valgrind para análisis de memoria
- Git para control de versiones

---

## 📝 Sistema de Evaluación

### Distribución de Calificaciones
- **Proyectos Semanales**: 40%
- **Exámenes Parciales**: 30% (2 exámenes - Mes 2 y Mes 3)
- **Proyecto Final**: 20%
- **Participación y Laboratorios**: 10%

### Proyectos Principales
1. **Mes 1**: Gestor de memoria y sistema de archivos mejorado (10%)
2. **Mes 2**: Sistema multitasking con scheduling (10%)
3. **Mes 3**: Memoria virtual y drivers avanzados (10%)
4. **Mes 4**: Sistema completo con redes y seguridad (10%)

### Criterios de Evaluación
- **Funcionalidad**: Cumplimiento de requisitos técnicos
- **Calidad del Código**: Estilo, documentación, modularidad
- **Innovación**: Implementaciones creativas y eficientes
- **Documentación**: Claridad en la explicación técnica

---

## 🎯 Competencias Desarrolladas

### Técnicas
- Programación de sistemas en C y ensamblador
- Diseño de arquitecturas de sistemas operativos
- Debugging y profiling de sistemas complejos
- Desarrollo de drivers de dispositivos
- Implementación de algoritmos de sistemas

### Transversales
- Resolución de problemas complejos
- Trabajo en equipo y colaboración
- Comunicación técnica efectiva
- Pensamiento crítico y analítico
- Gestión de proyectos de software

---

## 📈 Extensiones Avanzadas (Opcionales)

### Para Estudiantes Avanzados
1. **Hypervisor básico**: Virtualización de hardware
2. **Microkernel design**: Separación de servicios del kernel
3. **Distributed systems**: Clustering y replicación
4. **Embedded systems**: Adaptación para ARM/RISC-V
5. **Formal verification**: Pruebas matemáticas de correctitud

### Proyectos de Investigación
- Análisis comparativo de schedulers
- Optimización de cache para sistemas específicos
- Seguridad en sistemas embebidos
- Sistemas operativos para IoT
- Machine learning en gestión de recursos

---

## 🏆 Resultados Esperados

Al completar este curso, los estudiantes serán capaces de:

1. **Diseñar** arquitecturas de sistemas operativos modernas
2. **Implementar** componentes críticos del kernel
3. **Optimizar** el rendimiento de sistemas complejos
4. **Debuggear** problemas de bajo nivel eficientemente
5. **Evaluar** trade-offs en diseño de sistemas
6. **Contribuir** a proyectos de sistemas operativos reales

**¡Prepárate para dominar los fundamentos de los sistemas operativos!** 🚀