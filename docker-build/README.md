# 🐳 Entorno Docker para Mini-Kernel Educativo

## 📖 Descripción

Este directorio contiene un entorno de desarrollo completamente containerizado para el Mini-Kernel Educativo. Permite compilar y ejecutar el kernel sin necesidad de instalar herramientas de cross-compilation en el sistema host.

## 🎯 Ventajas del Entorno Docker

### ✅ **Sin Configuración Compleja**
- No requiere instalación manual de i686-elf-gcc
- No hay conflictos con herramientas del sistema
- Funciona en Linux, macOS y Windows

### ✅ **Entorno Consistente**
- Misma versión de herramientas para todos los desarrolladores
- Reproducibilidad garantizada entre diferentes máquinas
- Aislamiento completo del sistema host

### ✅ **Fácil Distribución**
- Imagen Docker portable
- Setup automático de todas las dependencias
- Ideal para aulas y laboratorios educativos

## 🚀 Inicio Rápido

### Prerrequisitos
- Docker instalado y ejecutándose
- Al menos 4GB de espacio libre
- Conexión a internet (solo para la primera vez)

### Primera Configuración
```bash
# 1. Navegar al directorio docker-build
cd docker-build

# 2. Construir la imagen Docker (solo la primera vez)
make setup

# 3. Compilar el kernel
make compile

# 4. Ejecutar el kernel
make run
```

### Uso Diario
```bash
# Compilar después de hacer cambios
make compile

# Ejecutar kernel con GUI (recomendado)
make run-gui

# Ejecutar kernel con salida en terminal
make run-debug

# Debugging con GDB
make debug

# Shell interactivo para exploración
make shell
```

## 🛠️ Comandos Disponibles

### Comandos Principales
| Comando | Descripción |
|---------|-------------|
| `make setup` | Construir imagen Docker (solo primera vez) |
| `make compile` | Compilar el kernel usando Docker |
| `make run-gui` | Ejecutar kernel con ventana QEMU (recomendado) |
| `make run-debug` | Ejecutar con salida en terminal |
| `make debug` | Ejecutar con soporte para GDB |
| `make shell` | Abrir shell interactivo en contenedor |
| `make clean` | Limpiar archivos compilados |

### Comandos de Mantenimiento
| Comando | Descripción |
|---------|-------------|
| `make info` | Mostrar información del entorno |
| `make check-tools` | Verificar herramientas instaladas |
| `make clean-all` | Limpiar todo (incluyendo imagen Docker) |
| `make rebuild` | Reconstruir imagen sin cache |
| `make image-size` | Mostrar tamaño de la imagen |

### Comandos Avanzados
| Comando | Descripción |
|---------|-------------|
| `make export-image` | Exportar imagen para compartir |
| `make import-image` | Importar imagen desde archivo |
| `make docs` | Generar documentación de uso |
| `make test` | Ejecutar tests básicos |

## 🐳 Detalles de la Imagen Docker

### Base: Ubuntu 22.04 LTS
- Sistema operativo estable y bien soportado
- Repositorios actualizados y seguros
- Compatibilidad excelente con herramientas de desarrollo

### Herramientas Incluidas

#### Cross-Compilation Toolchain
```
i686-elf-gcc 12.2.0    - Compilador C para target i686
i686-elf-as            - Ensamblador GNU
i686-elf-ld            - Linker GNU
i686-elf-objcopy       - Manipulación de objetos
i686-elf-objdump       - Análisis de binarios
```

#### Emulación y Debugging
```
QEMU system x86        - Emulador de sistema completo
GDB                    - Debugger GNU
```

#### Herramientas de Desarrollo
```
make                   - Sistema de construcción
git                    - Control de versiones
nano, vim, emacs       - Editores de texto
tree                   - Visualización de directorios
hexdump, strings       - Análisis de archivos
```

### Scripts Personalizados
- `check-environment` - Verificar instalación correcta
- `build-kernel` - Compilación optimizada con verificación
- `run-kernel-debug` - Ejecución con debugging automático

## 📁 Estructura de Archivos

```
docker-build/
├── Dockerfile              # Definición de la imagen
├── Makefile                # Comandos de construcción y uso
├── README.md               # Esta documentación
└── DOCKER_USAGE.md         # Guía rápida (generada automáticamente)
```

## 🍎 Configuración Especial para macOS

### Requisitos para GUI en macOS
Para usar `make run-gui` en macOS necesitas **XQuartz**:

```bash
# Opción 1: Descargar desde web
# Ir a: https://www.xquartz.org/

# Opción 2: Instalar con Homebrew
brew install --cask xquartz

# Después de instalar, reiniciar o ejecutar:
open -a XQuartz
```

### Configuración de XQuartz
1. Abrir XQuartz → Preferences → Security
2. ✅ Activar "Allow connections from network clients"
3. Reiniciar XQuartz

### Comandos específicos para macOS
```bash
# Auto-detección (recomendado)
make run-gui

# Forzar configuración macOS
make run-gui-macos

# Si hay problemas con GUI, usar modo debug
make run-debug
```

## 🔧 Configuración Avanzada

### Variables de Entorno
La imagen configura automáticamente:
```bash
TARGET=i686-elf
PREFIX=/usr/local/cross
PATH="$PREFIX/bin:$PATH"
CC=i686-elf-gcc
AS=i686-elf-as
LD=i686-elf-ld
```

### Volúmenes Docker
- `/workspace` - Directorio de trabajo (mapeado al proyecto)
- Todos los archivos del proyecto son accesibles desde el contenedor
- Los cambios se sincronizan automáticamente

### Puertos Expuestos
- `1234` - Puerto para debugging con GDB (solo en modo debug)

## 🐛 Debugging y Desarrollo

### Debugging con GDB
```bash
# Terminal 1: Ejecutar kernel en modo debug
make debug

# Terminal 2: Conectar GDB
docker exec -it minikernel-container gdb myos.elf
(gdb) target remote localhost:1234
(gdb) break kernel_main
(gdb) continue
```

### Shell Interactivo
```bash
# Abrir shell en el contenedor
make shell

# Dentro del contenedor
check-environment        # Verificar herramientas
build-kernel            # Compilar con verificación
ls -la                  # Explorar archivos
```

### Análisis de Binarios
```bash
# Dentro del shell del contenedor
objdump -d myos.elf | head -50    # Desensamblado
readelf -h myos.elf              # Headers ELF
hexdump -C myos.bin | head -10   # Volcado hexadecimal
```

## 📊 Rendimiento y Optimización

### Tiempos de Construcción
- **Primera vez**: 10-15 minutos (descarga + compilación de GCC)
- **Uso regular**: 2-5 segundos (solo compilación del kernel)
- **Reconstrucción**: 8-12 minutos (sin cache de Docker)

### Uso de Recursos
- **Imagen final**: ~2GB
- **RAM durante construcción**: ~4GB recomendados
- **CPU**: Usa todos los cores disponibles (`make -j$(nproc)`)

### Optimizaciones Incluidas
- Compilación paralela automática
- Cache de capas Docker optimizado
- Scripts de construcción eficientes
- Limpieza automática de archivos temporales

## 🔒 Seguridad

### Usuario No-Root
- Contenedor ejecuta como usuario `developer`
- Permisos sudo configurados para desarrollo
- Aislamiento del sistema host

### Redes
- Sin acceso de red durante ejecución (excepto debugging)
- Puertos expuestos solo cuando es necesario
- Comunicación limitada al proyecto

## 🚨 Resolución de Problemas

### Error: "Docker no está ejecutándose"
```bash
# Linux
sudo systemctl start docker

# macOS/Windows
# Iniciar Docker Desktop desde aplicaciones
```

### Error: "Imagen no encontrada"
```bash
# Verificar que la imagen se construyó correctamente
make info

# Si no existe, reconstruir
make setup
```

### Error: "Sin espacio en disco"
```bash
# Limpiar contenedores e imágenes no usadas
make clean-containers

# Limpiar todo el entorno
make clean-all
```

### Error: "Permisos de archivo"
```bash
# En Linux, puede ser necesario ajustar permisos
sudo chown -R $USER:$USER ../

# O ejecutar Docker con el usuario correcto
```

### Compilación Lenta
```bash
# Verificar recursos disponibles
docker stats

# Aumentar memoria asignada a Docker (en Docker Desktop)
# Usar compilación verbose para ver detalles
make verbose-compile
```

## 📚 Casos de Uso Educativos

### Para Instructores
1. **Setup de Laboratorio**:
   ```bash
   # En cada máquina del laboratorio
   git clone <repositorio>
   cd r2os/docker-build
   make setup
   ```

2. **Distribución de Entorno**:
   ```bash
   # Exportar imagen configurada
   make export-image
   # Distribuir minikernel-dev.tar.gz a estudiantes
   ```

3. **Evaluación Consistente**:
   - Mismo entorno para todos los estudiantes
   - Reproducibilidad garantizada de resultados
   - Sin problemas de configuración individual

### Para Estudiantes
1. **Desarrollo Individual**:
   ```bash
   make compile && make run    # Ciclo básico
   make debug                  # Para debugging
   make shell                  # Para exploración
   ```

2. **Colaboración**:
   - Entorno idéntico entre compañeros de equipo
   - Fácil intercambio de código y resultados
   - Sin "funciona en mi máquina"

3. **Experimentación**:
   ```bash
   make shell
   # Dentro del contenedor: modificar, compilar, probar
   # Sin riesgo para el sistema host
   ```

## 🔄 Actualizaciones y Mantenimiento

### Actualizar Herramientas
```bash
# Reconstruir imagen con versiones más nuevas
make rebuild
```

### Limpiar Regularmente
```bash
# Limpiar archivos de compilación
make clean

# Limpiar contenedores no usados
make clean-containers

# Limpieza completa (liberar espacio)
make clean-all
```

### Backup del Entorno
```bash
# Exportar imagen configurada
make export-image

# Guardar minikernel-dev.tar.gz como backup
```

## 📞 Soporte

### Comandos de Diagnóstico
```bash
make info           # Información general
make check-tools    # Verificar herramientas
docker --version    # Verificar Docker
docker info         # Estado del daemon Docker
```

### Logs y Debugging
```bash
# Ver logs de construcción detallados
make verbose-compile

# Logs de Docker
docker logs <container_name>

# Shell para investigación manual
make shell
```

### Recursos Adicionales
- [Docker Documentation](https://docs.docker.com/)
- [OSDev Wiki](https://wiki.osdev.org/)
- [GCC Cross-Compiler Guide](https://wiki.osdev.org/GCC_Cross-Compiler)

---

## ✨ **¡Tu entorno de desarrollo está listo!**

```bash
cd docker-build
make setup      # Solo la primera vez
make compile    # ¡Compilar y empezar!
make run        # ¡Ver tu kernel en acción!
```

**¡Desarrolla sistemas operativos sin complicaciones!** 🚀🐳