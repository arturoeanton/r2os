# 📚 Curso de Sistemas Operativos - Material Educativo Completo

## 🎯 Descripción del Curso

Este directorio contiene el material educativo completo para un **curso de Sistemas Operativos de 4 meses (8 capítulos)** basado en el mini-kernel educativo. El curso está diseñado para estudiantes universitarios y autodidactas que desean comprender los fundamentos de los sistemas operativos desde una perspectiva práctica, cubriendo desde arquitectura x86 hasta virtualización moderna.

## 📁 Estructura del Curso

### 📋 Plan General
- **`PLAN_CURSO_SO.md`** - Plan completo del curso de 16 semanas
  - Cronograma semanal detallado
  - Objetivos de aprendizaje por mes
  - Sistema de evaluación
  - Proyecto final integrador

### 📖 Capítulos Teórico-Prácticos (8 Capítulos Completos)

#### 🔧 **Capítulo 1: Arquitectura x86 y Bootloaders**
- **`CAPITULO_01.md`**
- **Temas**: Procesador x86, proceso de boot, Multiboot, cross-compilation
- **Práctica**: Setup del entorno, análisis del bootloader, debugging
- **Ejercicios**: 4 ejercicios progresivos + 1 proyecto avanzado

#### 💾 **Capítulo 2: Gestión de Memoria y Entrada/Salida**
- **`CAPITULO_02.md`**
- **Temas**: Memoria freestanding, puertos I/O, drivers VGA y PS/2
- **Práctica**: Implementación de drivers, optimización de memoria
- **Ejercicios**: 4 ejercicios desde básico hasta muy avanzado

#### 🗃️ **Capítulo 3: Sistemas de Archivos**
- **`CAPITULO_03.md`**
- **Temas**: FAT16, operaciones CRUD, algoritmos de asignación
- **Práctica**: Análisis del filesystem, operaciones avanzadas
- **Ejercicios**: 4 ejercicios incluyendo journaling básico

#### 🖥️ **Capítulo 4: Shell y Comunicación Inter-Proceso**
- **`CAPITULO_04.md`**
- **Temas**: Parsers de comandos, pipes, IPC, shells avanzados
- **Práctica**: Sistema de pipes, variables de entorno, job control
- **Ejercicios**: 4 ejercicios culminando en sistema de aliases

#### ⚡ **Capítulo 5: Interrupciones y Excepciones**
- **`CAPITULO_05.md`**
- **Temas**: IDT, PIC/APIC, handlers de interrupciones, excepciones x86
- **Práctica**: Configuración completa de interrupciones, manejo de eventos
- **Ejercicios**: 4 ejercicios desde básico hasta implementación de APIC

#### 🔄 **Capítulo 6: Multitasking y Scheduler**
- **`CAPITULO_06.md`**
- **Temas**: PCB, context switching, algoritmos de scheduling, sincronización
- **Práctica**: Scheduler multinivel, procesos concurrentes, semáforos
- **Ejercicios**: 4 ejercicios culminando en scheduler híbrido completo

#### 🌐 **Capítulo 7: Redes y Protocolos**
- **`CAPITULO_07.md`**
- **Temas**: Stack TCP/IP, Ethernet, drivers de red, sockets
- **Práctica**: Implementación de protocolos, servidor web básico
- **Ejercicios**: 4 ejercicios desde sniffer hasta stack TCP completo

#### 🔒 **Capítulo 8: Seguridad y Virtualización**
- **`CAPITULO_08.md`**
- **Temas**: Control de acceso, protección de memoria, hypervisors, contenedores
- **Práctica**: Sistema de usuarios, ASLR, VM básica, sandbox
- **Ejercicios**: 4 ejercicios culminando en sistema de contenedores

## 🎓 Metodología Pedagógica

### Enfoque Teórico-Práctico
Cada capítulo sigue una estructura consistente:

1. **📚 Parte Teórica**
   - Conceptos fundamentales explicados desde cero
   - Diagramas y ejemplos ilustrativos
   - Conexión con sistemas operativos reales

2. **🔧 Parte Práctica**
   - Análisis detallado del código del mini-kernel
   - Implementación paso a paso de funcionalidades
   - Explicación línea por línea de algoritmos críticos

3. **📝 Ejercicios Graduados**
   - **Básico**: Modificaciones simples para familiarizarse
   - **Intermedio**: Implementación de nuevas funcionalidades
   - **Avanzado**: Proyectos complejos con múltiples componentes
   - **Extremo**: Características de sistemas operativos reales

### Progresión del Aprendizaje (8 Capítulos)

```
Semanas 1-4:   Fundamentos (Cap. 1-2)
    ↓           Arquitectura x86 + Memoria/I/O
Semanas 5-8:   Sistemas (Cap. 3-4)
    ↓           Filesystems + Shell/IPC
Semanas 9-12:  Concurrencia (Cap. 5-6)
    ↓           Interrupciones + Multitasking
Semanas 13-16: Avanzado (Cap. 7-8)
    ↓           Redes + Seguridad/Virtualización
Proyecto Final: Integración de los 8 Capítulos
```

## 🚀 Características Destacadas

### ✅ **Completamente en Español**
- Todo el material está escrito en español técnico claro
- Comentarios del código en español
- Terminología técnica con explicaciones

### ✅ **Código Real y Funcional**
- Basado en un kernel que realmente funciona
- Más de 2000 líneas de código C documentado
- 35+ comandos de shell implementados

### ✅ **Ejercicios Progresivos Completos**
- 32 ejercicios principales distribuidos en 8 capítulos
- 4 ejercicios por capítulo con dificultad creciente
- Proyectos semanales con evaluación
- Trabajo final integrador de máxima complejidad

### ✅ **Conexión con la Industria**
- Patrones de diseño usados en sistemas reales
- Herramientas profesionales (GCC, QEMU, GDB)
- Preparación para contribuir a proyectos open source

## 📊 Sistema de Evaluación

### Distribución de Calificaciones (PLAN_CURSO_SO.md)
- **40%** - Proyectos semanales (16 semanas)
- **30%** - Exámenes parciales (2 exámenes)
- **20%** - Proyecto final integrador
- **10%** - Participación y laboratorios

### Criterios de Evaluación
- **Funcionalidad**: ¿El código funciona correctamente?
- **Calidad**: ¿Está bien documentado y es mantenible?
- **Innovación**: ¿Incluye mejoras o características creativas?
- **Comprensión**: ¿El estudiante entiende los conceptos?

## 🎯 Resultados de Aprendizaje

Al completar este curso, los estudiantes serán capaces de:

### Conocimientos Técnicos (8 Capítulos)
- Explicar la arquitectura completa de un sistema operativo
- Implementar todos los componentes fundamentales del kernel
- Desarrollar drivers de dispositivos complejos
- Diseñar sistemas de archivos eficientes con journaling
- Crear shells con funcionalidades avanzadas e IPC
- Programar sistemas de interrupciones y excepciones
- Implementar multitasking preemptivo con sincronización
- Desarrollar protocolos de red y stacks TCP/IP
- Diseñar sistemas de seguridad y virtualización

### Habilidades Prácticas
- Programar en C para sistemas embebidos
- Usar herramientas de desarrollo profesionales
- Debuggear problemas de bajo nivel
- Optimizar código para rendimiento
- Trabajar con especificaciones técnicas

### Competencias Transversales
- Resolución de problemas complejos
- Pensamiento sistemático y estructurado
- Documentación técnica clara
- Trabajo con código legacy
- Análisis de trade-offs en diseño

## 📚 Recursos Adicionales

### Libros de Referencia
- "Operating System Concepts" - Silberschatz, Galvin, Gagne
- "Modern Operating Systems" - Andrew Tanenbaum
- "Operating Systems: Three Easy Pieces" - Remzi Arpaci-Dusseau

### Herramientas Requeridas
- Cross-compiler i686-elf-gcc
- Emulador QEMU
- Debugger GDB
- Editor de texto con syntax highlighting
- Git para control de versiones

### Enlaces Útiles
- [OSDev Wiki](https://wiki.osdev.org/) - Documentación técnica
- [Intel Software Developer Manuals](https://software.intel.com/content/www/us/en/develop/articles/intel-sdm.html)
- [MIT 6.828](https://pdos.csail.mit.edu/6.828/) - Curso de referencia

## 🚀 Cómo Usar Este Material

### Para Instructores
1. Seguir el `PLAN_CURSO_SO.md` como guía semanal
2. Adaptar ejercicios según el nivel de los estudiantes
3. Usar el mini-kernel como base para demostraciones
4. Evaluar usando los criterios proporcionados

### Para Estudiantes
1. Leer cada capítulo antes de la clase correspondiente
2. Completar ejercicios en orden de dificultad
3. Experimentar con modificaciones del kernel
4. Documentar aprendizajes y dificultades

### Para Autodidactas
1. Configurar entorno de desarrollo siguiendo Capítulo 1
2. Avanzar a ritmo propio manteniendo la secuencia
3. Buscar comunidades online para resolver dudas
4. Contribuir mejoras al proyecto

## 📄 Licencia Educativa

Este material está diseñado para uso educativo y académico. Se permite:
- Uso en instituciones educativas
- Modificación para adaptación a cursos específicos
- Distribución no comercial
- Traducción a otros idiomas

Se requiere:
- Atribución a los autores originales
- Mantenimiento de la licencia en distribuciones
- Contribución de mejoras al proyecto original

---

## ✨ **¡Comienza tu viaje en el desarrollo de sistemas operativos!**

```bash
# Empezar es fácil:
cd ../
make run

# En el shell del kernel:
shell> help
shell> echo "¡Hola, mundo de los OS!" | rev
```

**¡Explora, experimenta y domina los fundamentos de los sistemas operativos modernos!** 🎓💻