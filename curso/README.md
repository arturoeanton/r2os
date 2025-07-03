# 📚 Curso Completo de Sistemas Operativos

## 🎯 Descripción del Curso

**Curso integral de Sistemas Operativos de 4 meses** basado en el desarrollo de un mini-kernel educativo funcional. Este curso cubre desde los fundamentos de arquitectura x86 hasta temas avanzados como virtualización y seguridad, proporcionando una experiencia práctica completa en el desarrollo de sistemas operativos.

## 📋 Información General

- **Duración**: 16 semanas (4 meses)
- **Modalidad**: Teórico-práctica
- **Idioma**: Español técnico
- **Nivel**: Universitario / Autodidacta avanzado
- **Prerrequisitos**: Programación en C básica

## 🗂️ Índice Completo del Curso

### 🔰 PRERREQUISITOS (Semana 0)

#### [PRE_CURSO_00.md](PRE_CURSO_00.md) - Fundamentos de C y Ensamblador
- **Objetivo**: Conocimientos mínimos de C freestanding y ensamblador x86
- **Contenido**:
  - C sin bibliotecas estándar
  - Punteros y manejo de memoria
  - Ensamblador x86 básico
  - Interacción C/ASM
- **Ejercicios**: 4 ejercicios progresivos
- **Duración**: 1 semana preparatoria

#### [PRE_CURSO_01.md](PRE_CURSO_01.md) - Herramientas y Entorno
- **Objetivo**: Configuración completa del entorno de desarrollo
- **Contenido**:
  - Cross-compiler i686-elf-gcc
  - QEMU y emulación
  - GDB y debugging
  - Makefiles y build systems
- **Ejercicios**: 4 ejercicios de configuración
- **Duración**: 1 semana preparatoria

---

### 📖 CAPÍTULOS PRINCIPALES (Semanas 1-16)

#### **MES 1: Fundamentos de Hardware y Software**

#### [CAPITULO_01.md](CAPITULO_01.md) - Arquitectura x86 y Bootloaders (Semanas 1-2)
- **Temas**:
  - Arquitectura x86: registros, modos, segmentación
  - Proceso de boot: BIOS, MBR, bootloaders
  - Multiboot specification
  - Cross-compilation y toolchains
- **Práctica**:
  - Análisis del bootloader
  - Debugging con QEMU y GDB
  - Modificación del proceso de boot
- **Ejercicios**: 4 ejercicios + 1 proyecto avanzado

#### [CAPITULO_02.md](CAPITULO_02.md) - Gestión de Memoria y E/S (Semanas 3-4)
- **Temas**:
  - Memoria en modo freestanding
  - Puertos de E/S y mapeo de memoria
  - Drivers VGA y PS/2
  - Gestión básica de memoria
- **Práctica**:
  - Implementación de drivers
  - Optimización de acceso a memoria
  - Sistema de printf personalizado
- **Ejercicios**: 4 ejercicios desde básico hasta muy avanzado

#### **MES 2: Sistemas de Archivos y Shell**

#### [CAPITULO_03.md](CAPITULO_03.md) - Sistemas de Archivos (Semanas 5-6)
- **Temas**:
  - Sistema de archivos FAT16
  - Operaciones CRUD en archivos
  - Algoritmos de asignación de espacio
  - Journaling básico
- **Práctica**:
  - Análisis del filesystem existente
  - Operaciones avanzadas de archivos
  - Sistema de cache
- **Ejercicios**: 4 ejercicios incluyendo journaling

#### [CAPITULO_04.md](CAPITULO_04.md) - Shell y IPC (Semanas 7-8)
- **Temas**:
  - Parsers de comandos avanzados
  - Sistema de pipes completo
  - Comunicación inter-proceso
  - Job control y variables de entorno
- **Práctica**:
  - Expansión del shell existente
  - Implementación de pipes múltiples
  - Sistema de aliases
- **Ejercicios**: 4 ejercicios culminando en shell avanzado

#### **MES 3: Concurrencia y Multitasking**

#### [CAPITULO_05.md](CAPITULO_05.md) - Interrupciones y Excepciones (Semanas 9-10)
- **Temas**:
  - IDT (Interrupt Descriptor Table)
  - PIC y APIC configuration
  - Handlers de interrupciones
  - Excepciones del procesador x86
- **Práctica**:
  - Configuración completa de interrupciones
  - Manejo robusto de eventos
  - Sistema de recovery automático
- **Ejercicios**: 4 ejercicios hasta implementación APIC

#### [CAPITULO_06.md](CAPITULO_06.md) - Multitasking y Scheduler (Semanas 11-12)
- **Temas**:
  - Process Control Block (PCB)
  - Context switching optimizado
  - Algoritmos de scheduling (RR, Priority, MLFQ)
  - Sincronización con semáforos
- **Práctica**:
  - Scheduler multinivel funcional
  - Procesos concurrentes
  - Sistema de sincronización
- **Ejercicios**: 4 ejercicios culminando en análisis de rendimiento

#### **MES 4: Redes y Seguridad Avanzada**

#### [CAPITULO_07.md](CAPITULO_07.md) - Redes y Protocolos (Semanas 13-14)
- **Temas**:
  - Stack TCP/IP completo
  - Protocolos: Ethernet, IP, TCP, UDP, ICMP
  - Drivers de red y NIC management
  - Socket API básico
- **Práctica**:
  - Implementación de protocolos desde cero
  - Servidor web básico
  - Herramientas de red
- **Ejercicios**: 4 ejercicios desde sniffer hasta stack TCP

#### [CAPITULO_08.md](CAPITULO_08.md) - Seguridad y Virtualización (Semanas 15-16)
- **Temas**:
  - Sistemas de seguridad y control de acceso
  - Protección de memoria (ASLR, stack canaries)
  - Hypervisors y virtualización
  - Contenedores y aislamiento
- **Práctica**:
  - Sistema completo de usuarios
  - Hypervisor básico funcional
  - Contenedores con namespaces
- **Ejercicios**: 4 ejercicios culminando en sistema de contenedores

---

## 📊 Estructura Pedagógica

### Metodología por Capítulo

Cada capítulo sigue una estructura consistente:

```
📚 PARTE TEÓRICA (40%)
├── Conceptos fundamentales
├── Diagramas explicativos  
├── Conexión con sistemas reales
└── Referencias técnicas

🔧 PARTE PRÁCTICA (40%)
├── Análisis del código existente
├── Implementación paso a paso
├── Explicación línea por línea
└── Integración con el sistema

🧪 EJERCICIOS (20%)
├── Básico: Familiarización
├── Intermedio: Nuevas funcionalidades  
├── Avanzado: Proyectos complejos
└── Extremo: Características profesionales
```

### Progresión del Aprendizaje

```
📖 PRERREQUISITOS → 🏗️ FUNDAMENTOS → 🔧 SISTEMAS → ⚡ CONCURRENCIA → 🔒 AVANZADO
   (C + Tools)      (HW + Memory)     (FS + Shell)   (Int + Multi)    (Net + Sec)
      ↓                   ↓               ↓              ↓               ↓
  Semana 0         Semanas 1-4      Semanas 5-8    Semanas 9-12   Semanas 13-16
                                                          ↓
                               🎓 PROYECTO FINAL INTEGRADOR
                                (Combina todos los capítulos)
```

## 🎯 Objetivos por Mes

### **Mes 1: Bases Sólidas**
- Configurar entorno profesional de desarrollo
- Dominar arquitectura x86 y proceso de boot
- Implementar gestión básica de memoria y E/S
- **Entregable**: Kernel básico con drivers funcionando

### **Mes 2: Sistemas Fundamentales**
- Crear sistema de archivos jerárquico completo
- Desarrollar shell avanzado con pipes múltiples
- Implementar comunicación inter-proceso robusta
- **Entregable**: Sistema operativo básico con shell funcional

### **Mes 3: Concurrencia y Rendimiento**
- Configurar sistema completo de interrupciones
- Implementar multitasking preemptivo con scheduler
- Crear mecanismos de sincronización entre procesos
- **Entregable**: OS multitarea con procesos concurrentes

### **Mes 4: Características Avanzadas**
- Desarrollar stack de red con protocolos TCP/IP
- Implementar sistemas de seguridad y control de acceso
- Crear capacidades de virtualización básica
- **Entregable**: Sistema operativo completo y funcional

## 📋 Plan de Estudios Detallado

Ver [PLAN_CURSO_SO.md](PLAN_CURSO_SO.md) para:
- Cronograma semanal detallado
- Distribución de horas teóricas y prácticas
- Sistema de evaluación completo
- Criterios de calificación
- Proyecto final integrador

## 📚 Recursos de Estudio

### Libros de Referencia
- **"Operating System Concepts"** - Silberschatz, Galvin, Gagne
- **"Modern Operating Systems"** - Andrew Tanenbaum
- **"Operating Systems: Three Easy Pieces"** - Remzi Arpaci-Dusseau

### Documentación Técnica
- [Intel x86 Software Developer Manuals](https://software.intel.com/content/www/us/en/develop/articles/intel-sdm.html)
- [OSDev Wiki](https://wiki.osdev.org/) - Recurso fundamental
- [Multiboot Specification](https://www.gnu.org/software/grub/manual/multiboot/)

### Herramientas Requeridas
- Cross-compiler: `i686-elf-gcc`
- Emulador: `qemu-system-i386`
- Debugger: `i686-elf-gdb`
- Build system: `make`
- Control de versiones: `git`

## 🎓 Evaluación y Certificación

### Distribución de Calificaciones
- **40%** - Ejercicios semanales (32 ejercicios)
- **30%** - Exámenes parciales (4 exámenes)
- **20%** - Proyecto final integrador
- **10%** - Participación y laboratorios

### Criterios de Evaluación
- **Funcionalidad**: ¿El código funciona correctamente?
- **Calidad**: ¿Está bien documentado y estructurado?
- **Innovación**: ¿Incluye mejoras creativas?
- **Comprensión**: ¿Demuestra entendimiento profundo?

## 🚀 Cómo Usar Este Curso

### Para Instructores
1. Seguir cronograma en `PLAN_CURSO_SO.md`
2. Adaptar ejercicios según nivel de estudiantes
3. Usar mini-kernel como base para demostraciones
4. Evaluar usando criterios proporcionados

### Para Estudiantes
1. Completar prerrequisitos antes de comenzar
2. Leer cada capítulo antes de la práctica
3. Realizar ejercicios en orden de dificultad
4. Documentar aprendizajes y dificultades

### Para Autodidactas
1. Configurar entorno siguiendo PRE_CURSO_01
2. Avanzar a ritmo propio manteniendo secuencia
3. Buscar comunidades online para apoyo
4. Contribuir mejoras al proyecto

## 🏆 Resultados de Aprendizaje

Al completar este curso, serás capaz de:

### Conocimientos Técnicos
- ✅ Explicar arquitectura completa de sistemas operativos
- ✅ Implementar todos los componentes fundamentales del kernel
- ✅ Desarrollar drivers de dispositivos complejos
- ✅ Diseñar sistemas de archivos con journaling
- ✅ Crear shells avanzados con IPC completo
- ✅ Programar sistemas de interrupciones robustos
- ✅ Implementar multitasking preemptivo con sincronización
- ✅ Desarrollar protocolos de red y stacks TCP/IP
- ✅ Diseñar sistemas de seguridad y virtualización

### Habilidades Profesionales
- ✅ Programación de sistemas de bajo nivel
- ✅ Debugging y profiling de kernels
- ✅ Análisis de rendimiento y optimización
- ✅ Trabajo con especificaciones técnicas
- ✅ Desarrollo con toolchains profesionales

### Preparación para la Industria
- ✅ Desarrollo de sistemas embebidos
- ✅ Ingeniería de sistemas operativos
- ✅ Arquitectura de software de sistemas
- ✅ Cyberseguridad y análisis de vulnerabilidades
- ✅ Cloud computing y virtualización

---

## 📞 Soporte y Comunidad

### Recursos de Ayuda
- Documentación técnica en cada capítulo
- Ejercicios con soluciones guiadas
- Referencias a implementaciones reales
- Links a comunidades especializadas

### Contribuciones
Este es un proyecto educativo abierto. Se agradecen:
- Correcciones y mejoras a la documentación
- Nuevos ejercicios y proyectos
- Traducciones a otros idiomas
- Casos de estudio adicionales

---

## ✨ ¡Comienza Tu Aventura en Sistemas Operativos!

```bash
# Primer paso: Prerrequisitos
📖 Leer PRE_CURSO_00.md (C y Ensamblador)
🛠️ Completar PRE_CURSO_01.md (Herramientas)

# Segundo paso: Fundamentos  
🔧 CAPITULO_01.md (Arquitectura x86)
💾 CAPITULO_02.md (Memoria y E/S)

# Continuar con todos los capítulos...
# ¡Y construir tu propio sistema operativo!
```

**¡Prepárate para dominar los sistemas operativos desde los fundamentos hasta las tecnologías más avanzadas!** 🎓🚀💻

*"El mejor momento para plantar un árbol fue hace 20 años. El segundo mejor momento es ahora. Lo mismo aplica para aprender sistemas operativos."*