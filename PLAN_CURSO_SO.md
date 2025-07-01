# Plan de Curso: Sistemas Operativos Avanzado
## 4 Meses - Basado en Mini Kernel Educativo

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

### **MES 4: Redes, Seguridad y Optimización**

#### **Semana 13: Redes Básicas**
**Teoría (3 horas):**
- Stack de protocolos: Ethernet, IP, TCP/UDP
- Socket programming en kernel space
- Network device drivers
- Routing y switching básico

**Práctica (3 horas):**
- Driver básico para tarjeta de red (simulada)
- Implementación de ping simple
- Stack TCP/IP minimalista
- Comando `netstat` básico

**Entregable:** Capacidades básicas de red

---

#### **Semana 14: Seguridad y Protección**
**Teoría (3 horas):**
- User mode vs kernel mode
- System calls y privilege escalation
- Access control lists (ACLs)
- Vulnerability mitigation: ASLR, stack canaries

**Práctica (3 horas):**
- Implementación de user mode básico
- System call interface
- Sistema de permisos para archivos
- Protección contra buffer overflows

**Entregable:** Sistema con separación user/kernel

---

#### **Semana 15: Rendimiento y Optimización**
**Teoría (3 horas):**
- Profiling y benchmarking
- Cache optimization
- Lock-free programming
- Real-time systems constraints

**Práctica (3 horas):**
- Herramientas de profiling del kernel
- Optimización de algoritmos críticos
- Implementación de estructuras lock-free básicas
- Benchmarks de rendimiento

**Entregable:** Sistema optimizado con métricas de rendimiento

---

#### **Semana 16: Proyecto Final y Presentación**
**Teoría (2 horas):**
- Review de conceptos avanzados
- Tendencias futuras en sistemas operativos
- Microkernel vs monolithic design
- OS para sistemas embebidos e IoT

**Práctica (4 horas):**
- Integración de todos los componentes
- Testing exhaustivo del sistema completo
- Documentación técnica completa
- Presentación de proyectos finales

**Entregable:** Sistema operativo completo con documentación

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