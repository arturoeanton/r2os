# 🚀 Inicio Rápido - Mini Kernel Educativo

## 🎯 ¿Qué es este proyecto?

**Un curso completo de Sistemas Operativos** con un mini-kernel funcional que incluye:
- 📖 **10 capítulos educativos** (2 prerrequisitos + 8 principales)
- 💻 **Kernel funcional** con 35+ comandos de shell
- 🔧 **Herramientas profesionales** con entorno Docker
- 🎓 **Curso de 4 meses** con 32 ejercicios prácticos

## ⚡ Empezar en 5 minutos

### Opción 1: Ver el kernel funcionando 🐳
```bash
# Usar Docker (sin instalar nada)
cd docker-build
make setup-all    # Solo la primera vez (10-15 min)
make run-gui      # ¡Kernel con ventana!
```

### Opción 2: Explorar el curso 📚
```bash
# Leer la documentación educativa
cd curso
cat README.md     # Guía completa del curso

# Prerrequisitos (si es necesario)
cat PRE_CURSO_00.md    # C y Ensamblador básico
cat PRE_CURSO_01.md    # Herramientas de desarrollo

# Primer capítulo
cat CAPITULO_01.md     # Arquitectura x86 y Bootloaders
```

### Opción 3: Compilación local 🔧
```bash
# Solo si tienes i686-elf-gcc instalado
make
make run
```

## 🗂️ ¿Qué hay en este repositorio?

```
r2os/
├── 🎓 curso/              # Curso completo de 10 capítulos
│   ├── PRE_CURSO_00.md    # C y Ensamblador (prerrequisito)
│   ├── PRE_CURSO_01.md    # Herramientas (prerrequisito)
│   ├── CAPITULO_01.md     # Arquitectura x86 y Bootloaders
│   ├── CAPITULO_02.md     # Gestión de Memoria y E/S
│   ├── CAPITULO_03.md     # Sistemas de Archivos
│   ├── CAPITULO_04.md     # Shell y IPC
│   ├── CAPITULO_05.md     # Interrupciones y Excepciones
│   ├── CAPITULO_06.md     # Multitasking y Scheduler
│   ├── CAPITULO_07.md     # Redes y Protocolos
│   ├── CAPITULO_08.md     # Seguridad y Virtualización
│   └── PLAN_CURSO_SO.md   # Plan de 16 semanas
│
├── 💻 kernel.c            # Código principal del kernel (2000+ líneas)
├── 🐳 docker-build/       # Entorno Docker completo
├── 📋 docs/               # Documentación de desarrollo
└── 🛠️ scripts/            # Herramientas y utilidades
```

## 📖 ¿Qué aprenderás?

### 🔰 Prerrequisitos (Semana 0)
- **C freestanding**: Programación sin bibliotecas estándar
- **Ensamblador x86**: Instrucciones básicas y registros
- **Herramientas**: Cross-compiler, QEMU, GDB, Makefiles

### 📚 Mes 1: Fundamentos
- **Arquitectura x86**: Registros, modos, bootloaders
- **Memoria y E/S**: Gestión directa, drivers VGA/PS2

### 🔧 Mes 2: Sistemas
- **Filesystems**: FAT16 completo con journaling
- **Shell avanzado**: 35+ comandos, pipes, IPC

### ⚡ Mes 3: Concurrencia
- **Interrupciones**: IDT, PIC, manejo de eventos
- **Multitasking**: Scheduler, context switching, sincronización

### 🌐 Mes 4: Avanzado
- **Redes**: Stack TCP/IP, protocolos, sockets
- **Seguridad**: Control de acceso, virtualización, contenedores

## 🎮 Prueba el kernel ahora

Si ya tienes el kernel corriendo, prueba estos comandos:

```bash
shell> help                    # Ver todos los comandos
shell> ls                      # Listar archivos
shell> echo "Hola" | rev       # Probar pipes
shell> cat README.md | head 5  # Ver primeras líneas
shell> touch test.txt          # Crear archivo
shell> copycon test.txt        # Escribir contenido
Escribe algo y presiona Ctrl+Z
shell> cat test.txt            # Ver contenido
shell> ls | wc                 # Contar archivos
shell> history                 # Ver historial
shell> clear                   # Limpiar pantalla
```

## 🎯 ¿Para quién es este curso?

### ✅ Perfecto para:
- **Estudiantes universitarios** de Ciencias de la Computación
- **Autodidactas** con conocimientos básicos de C
- **Profesionales** que quieren entender sistemas de bajo nivel
- **Instructores** buscando material educativo completo

### 📋 Prerrequisitos mínimos:
- Programación básica en C
- Conceptos básicos de computación
- Ganas de aprender sistemas de bajo nivel

### ⏰ Tiempo estimado:
- **Modo intensivo**: 2 meses (4-6 horas/semana)
- **Modo estándar**: 4 meses (2-3 horas/semana)
- **Modo relajado**: 6 meses (1-2 horas/semana)

## 🔧 Verificar que todo funciona

```bash
# Ejecutar verificación automática
./scripts/verify_repo.sh

# Debería mostrar:
# 🎉 ¡PERFECTO! El repositorio está completamente configurado.
```

## 📞 ¿Necesitas ayuda?

### 📚 Documentación
- `README.md` - Descripción completa del proyecto
- `curso/README.md` - Guía detallada del curso
- `docker-build/README.md` - Uso del entorno Docker

### 🐛 Problemas comunes
- **Docker no inicia**: Verificar que Docker Desktop esté ejecutándose
- **Compilación falla**: Leer `curso/PRE_CURSO_01.md` para instalar herramientas
- **QEMU no funciona**: Usar `make run-debug` para modo texto

### 🌐 Recursos adicionales
- [OSDev Wiki](https://wiki.osdev.org/) - Documentación técnica
- [Intel x86 Manual](https://software.intel.com/content/www/us/en/develop/articles/intel-sdm.html)
- [MIT 6.828](https://pdos.csail.mit.edu/6.828/) - Curso de referencia

## 🎉 ¡Listo para empezar!

### Paso 1: Decide tu ruta
- 🏃‍♂️ **Rápido**: `cd docker-build && make run-gui`
- 📖 **Educativo**: `cd curso && cat README.md`
- 🔧 **Manual**: Leer `curso/PRE_CURSO_01.md`

### Paso 2: Sumérgete
- Ejecutar el kernel y explorar comandos
- Leer el primer capítulo del curso
- Hacer el primer ejercicio

### Paso 3: Construir
- Seguir el plan de 16 semanas
- Completar los 32 ejercicios
- Crear tu propio sistema operativo

---

**¡Bienvenido al fascinante mundo del desarrollo de sistemas operativos!** 🎓💻⚡

*"El viaje de mil líneas de código comienza con un simple `printf`."*