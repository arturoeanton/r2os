# Makefile para Mini-Kernel Educativo usando Docker
# Este Makefile permite compilar el kernel sin instalar herramientas localmente

# Configuración
DOCKER_IMAGE = minikernel-dev
DOCKER_TAG = latest
CONTAINER_NAME = minikernel-container
PROJECT_ROOT = $(shell dirname $(PWD))

# Colores para output
GREEN = \033[0;32m
YELLOW = \033[1;33m
RED = \033[0;31m
NC = \033[0m # No Color

# Objetivos principales
.PHONY: help build-image run-container compile run clean debug shell info check-docker

# Objetivo por defecto
all: help

help:
	@echo "$(GREEN)🎓 Mini-Kernel Educativo - Docker Build System$(NC)"
	@echo ""
	@echo "$(YELLOW)⭐ Comandos principales (Docker + QEMU host):$(NC)"
	@echo "  $(GREEN)setup$(NC)          - Construir imagen Docker (solo primera vez)"
	@echo "  $(GREEN)compile$(NC)        - Compilar kernel en Docker"
	@echo "  $(GREEN)run-gui$(NC)        - Compilar en Docker + Ejecutar en QEMU host (⭐ SIN XQuartz)"
	@echo "  $(GREEN)run-debug-host$(NC) - Compilar en Docker + Debug en terminal host"
	@echo ""
	@echo "$(YELLOW)🐳 Comandos solo Docker (requiere X11):$(NC)"
	@echo "  $(GREEN)run-debug$(NC)      - Ejecutar con salida serial en Docker"
	@echo "  $(GREEN)run-gui-linux$(NC)  - GUI con X11 forwarding (Linux)"
	@echo "  $(GREEN)run-gui-macos$(NC)  - GUI con XQuartz (macOS)"
	@echo ""
	@echo "$(YELLOW)🔧 Herramientas:$(NC)"
	@echo "  $(GREEN)shell$(NC)          - Shell interactivo en contenedor"
	@echo "  $(GREEN)debug$(NC)          - Ejecutar con soporte GDB"
	@echo "  $(GREEN)clean$(NC)          - Limpiar archivos compilados"
	@echo "  $(GREEN)info$(NC)           - Información del entorno"
	@echo ""
	@echo "$(YELLOW)🚀 Uso típico (configuración automática):$(NC)"
	@echo "  1. $(GREEN)make setup-all$(NC)    # Configura todo automáticamente"
	@echo "  2. $(GREEN)make run-gui$(NC)      # ¡Compilar y ejecutar!"
	@echo ""
	@echo "$(YELLOW)🔧 Configuración manual:$(NC)"
	@echo "  1. $(GREEN)make setup$(NC)        # Solo Docker (10-15 min)"
	@echo "  2. $(GREEN)make install-qemu$(NC) # Solo QEMU"
	@echo "  3. $(GREEN)make run-gui$(NC)      # Ejecutar"
	@echo ""
	@echo "$(YELLOW)📋 Requisitos:$(NC)"
	@echo "  - Docker (ya instalado ✅)"
	@echo "  - QEMU: automático con $(GREEN)make setup-all$(NC)"
	@echo ""

# Verificar que Docker esté disponible
check-docker:
	@which docker > /dev/null || (echo "$(RED)❌ Docker no está instalado$(NC)" && exit 1)
	@docker info > /dev/null 2>&1 || (echo "$(RED)❌ Docker no está ejecutándose$(NC)" && exit 1)

# Verificar que QEMU esté disponible en el host
check-qemu-host:
	@which qemu-system-i386 > /dev/null || (echo "$(RED)❌ QEMU no está instalado en el host$(NC)" && \
		echo "$(YELLOW)💡 Instala QEMU:$(NC)" && \
		echo "$(YELLOW)   macOS: brew install qemu$(NC)" && \
		echo "$(YELLOW)   Ubuntu: sudo apt install qemu-system-x86$(NC)" && \
		echo "$(YELLOW)   O ejecuta: make install-qemu$(NC)" && \
		echo "$(YELLOW)   O usa: make run-debug (sin GUI)$(NC)" && exit 1)

# Instalar QEMU automáticamente
install-qemu:
	@echo "$(YELLOW)🔧 Instalando QEMU...$(NC)"
	@if [ "$$(uname)" = "Darwin" ]; then \
		if command -v brew >/dev/null 2>&1; then \
			echo "$(YELLOW)🍺 Usando Homebrew para instalar QEMU...$(NC)"; \
			brew install qemu; \
		else \
			echo "$(RED)❌ Homebrew no está instalado$(NC)"; \
			echo "$(YELLOW)💡 Instala Homebrew desde: https://brew.sh/$(NC)"; \
			echo "$(YELLOW)💡 O instala QEMU manualmente$(NC)"; \
			exit 1; \
		fi \
	elif [ "$$(uname)" = "Linux" ]; then \
		echo "$(YELLOW)🐧 Detectado Linux...$(NC)"; \
		if command -v apt >/dev/null 2>&1; then \
			echo "$(YELLOW)📦 Usando apt para instalar QEMU...$(NC)"; \
			sudo apt update && sudo apt install -y qemu-system-x86; \
		elif command -v yum >/dev/null 2>&1; then \
			echo "$(YELLOW)📦 Usando yum para instalar QEMU...$(NC)"; \
			sudo yum install -y qemu-system-x86; \
		else \
			echo "$(RED)❌ Gestor de paquetes no soportado$(NC)"; \
			exit 1; \
		fi \
	else \
		echo "$(RED)❌ Sistema operativo no soportado$(NC)"; \
		exit 1; \
	fi
	@echo "$(GREEN)✅ QEMU instalado correctamente$(NC)"

# Configuración completa automática
setup-all: check-docker
	@echo "$(GREEN)🚀 Configuración completa del entorno Mini-Kernel$(NC)"
	@echo "$(YELLOW)📦 Paso 1/2: Configurando Docker...$(NC)"
	@$(MAKE) setup
	@echo "$(YELLOW)📦 Paso 2/2: Instalando QEMU...$(NC)"
	@if ! command -v qemu-system-i386 >/dev/null 2>&1; then \
		$(MAKE) install-qemu; \
	else \
		echo "$(GREEN)✅ QEMU ya está instalado$(NC)"; \
	fi
	@echo ""
	@echo "$(GREEN)🎉 ¡CONFIGURACIÓN COMPLETA!$(NC)"
	@echo "$(YELLOW)💡 Ahora puedes usar:$(NC)"
	@echo "  $(GREEN)make run-gui$(NC)    # Compilar y ejecutar con ventana"
	@echo "  $(GREEN)make compile$(NC)    # Solo compilar"
	@echo ""

# Construir imagen Docker
setup: check-docker
	@echo "$(YELLOW)🔨 Construyendo imagen Docker...$(NC)"
	@echo "$(YELLOW)⚠️  Esto puede tomar 10-15 minutos la primera vez$(NC)"
	docker build -t $(DOCKER_IMAGE):$(DOCKER_TAG) .
	@echo "$(GREEN)✅ Imagen Docker construida exitosamente$(NC)"

# Verificar si la imagen existe
check-image:
	@docker image inspect $(DOCKER_IMAGE):$(DOCKER_TAG) > /dev/null 2>&1 || \
		(echo "$(RED)❌ Imagen Docker no encontrada. Ejecuta: make setup$(NC)" && exit 1)

# Compilar kernel usando Docker (alias para compile-only)
compile: compile-only
	@echo "$(GREEN)✅ Compilación completada$(NC)"
	@echo "$(YELLOW)💡 Usa 'make run-gui' para ejecutar con ventana$(NC)"
	@echo "$(YELLOW)💡 Usa 'make run-debug' para ejecutar en terminal$(NC)"

# Ejecutar kernel en QEMU (modo texto)
run: check-docker check-image
	@echo "$(YELLOW)🚀 Ejecutando Mini-Kernel en QEMU (modo texto)...$(NC)"
	@echo "$(YELLOW)💡 Presiona Ctrl+A, luego X para salir de QEMU$(NC)"
	docker run --rm -it \
		-v $(PROJECT_ROOT):/workspace \
		-w /workspace \
		$(DOCKER_IMAGE):$(DOCKER_TAG) \
		make run

# Ejecutar kernel con QEMU del host (sin X11)
run-gui: check-docker check-image check-qemu-host
	@echo "$(YELLOW)🚀 Compilando en Docker, ejecutando en QEMU del host...$(NC)"
	@echo "$(YELLOW)💡 Se abrirá ventana de QEMU nativa$(NC)"
	docker run --rm \
		-v $(PROJECT_ROOT):/workspace \
		-w /workspace \
		$(DOCKER_IMAGE):$(DOCKER_TAG) \
		make
	@echo "$(GREEN)✅ Compilación completada, ejecutando QEMU...$(NC)"
	cd $(PROJECT_ROOT) && qemu-system-i386 -kernel myos.elf -m 32

# Ejecutar GUI forzado (Linux style)
run-gui-linux: check-docker check-image
	@echo "$(YELLOW)🐧 Ejecutando con configuración Linux...$(NC)"
	docker run --rm -it \
		--env="DISPLAY=$${DISPLAY}" \
		--volume="/tmp/.X11-unix:/tmp/.X11-unix:rw" \
		--net=host \
		-v $(PROJECT_ROOT):/workspace \
		-w /workspace \
		$(DOCKER_IMAGE):$(DOCKER_TAG) \
		make run-gui

# Ejecutar GUI forzado (macOS style)
run-gui-macos: check-docker check-image
	@echo "$(YELLOW)🍎 Ejecutando con configuración macOS...$(NC)"
	./run-macos-gui.sh

# Ejecutar kernel con debugging serial (dentro de Docker)
run-debug: check-docker check-image
	@echo "$(YELLOW)🐛 Ejecutando kernel con salida de debug...$(NC)"
	@echo "$(YELLOW)💡 Salida serial redirigida a terminal$(NC)"
	docker run --rm -it \
		-v $(PROJECT_ROOT):/workspace \
		-w /workspace \
		$(DOCKER_IMAGE):$(DOCKER_TAG) \
		bash -c "make && qemu-system-i386 -kernel myos.elf -m 32 -display none -serial stdio"

# Ejecutar QEMU en el host con debugging serial
run-debug-host: check-docker check-image check-qemu-host
	@echo "$(YELLOW)🐛 Compilando en Docker, debug en QEMU del host...$(NC)"
	docker run --rm \
		-v $(PROJECT_ROOT):/workspace \
		-w /workspace \
		$(DOCKER_IMAGE):$(DOCKER_TAG) \
		make
	@echo "$(GREEN)✅ Compilación completada, ejecutando QEMU con debug...$(NC)"
	cd $(PROJECT_ROOT) && qemu-system-i386 -kernel myos.elf -m 32 -nographic

# Solo compilar (sin ejecutar)
compile-only: check-docker check-image
	@echo "$(YELLOW)🔨 Solo compilando (sin ejecutar)...$(NC)"
	docker run --rm \
		-v $(PROJECT_ROOT):/workspace \
		-w /workspace \
		$(DOCKER_IMAGE):$(DOCKER_TAG) \
		make

# Ejecutar kernel con debugging
debug: check-docker check-image
	@echo "$(YELLOW)🐛 Ejecutando kernel en modo debug...$(NC)"
	@echo "$(YELLOW)💡 GDB conectará en puerto 1234$(NC)"
	@echo "$(YELLOW)💡 En otra terminal: docker exec -it $(CONTAINER_NAME) gdb myos.elf$(NC)"
	docker run --rm -it \
		--name $(CONTAINER_NAME) \
		-p 1234:1234 \
		-v $(PROJECT_ROOT):/workspace \
		-w /workspace \
		$(DOCKER_IMAGE):$(DOCKER_TAG) \
		run-kernel-debug

# Abrir shell interactivo en contenedor
shell: check-docker check-image
	@echo "$(YELLOW)🖥️  Abriendo shell en contenedor de desarrollo...$(NC)"
	@echo "$(YELLOW)💡 Usa 'exit' para salir$(NC)"
	docker run --rm -it \
		-v $(PROJECT_ROOT):/workspace \
		-w /workspace \
		$(DOCKER_IMAGE):$(DOCKER_TAG) \
		/bin/bash

# Limpiar archivos compilados
clean: check-docker check-image
	@echo "$(YELLOW)🧹 Limpiando archivos compilados...$(NC)"
	docker run --rm \
		-v $(PROJECT_ROOT):/workspace \
		-w /workspace \
		$(DOCKER_IMAGE):$(DOCKER_TAG) \
		make clean
	@echo "$(GREEN)✅ Archivos limpiados$(NC)"

# Limpiar todo incluyendo imagen Docker
clean-all: clean
	@echo "$(YELLOW)🗑️  Eliminando imagen Docker...$(NC)"
	docker rmi $(DOCKER_IMAGE):$(DOCKER_TAG) || true
	@echo "$(GREEN)✅ Limpieza completa$(NC)"

# Mostrar información del entorno
info: check-docker
	@echo "$(GREEN)🎓 Mini-Kernel Educativo - Información del Entorno$(NC)"
	@echo ""
	@echo "$(YELLOW)Docker:$(NC)"
	@docker --version
	@echo ""
	@echo "$(YELLOW)Imagen del proyecto:$(NC)"
	@docker image inspect $(DOCKER_IMAGE):$(DOCKER_TAG) > /dev/null 2>&1 && \
		echo "  ✅ $(DOCKER_IMAGE):$(DOCKER_TAG) - Disponible" || \
		echo "  ❌ $(DOCKER_IMAGE):$(DOCKER_TAG) - No encontrada (ejecuta: make setup)"
	@echo ""
	@echo "$(YELLOW)Directorio del proyecto:$(NC)"
	@echo "  📁 $(PROJECT_ROOT)"
	@echo ""
	@echo "$(YELLOW)Comandos rápidos:$(NC)"
	@echo "  🔨 make compile  - Compilar kernel"
	@echo "  🚀 make run      - Ejecutar kernel"
	@echo "  🐛 make debug    - Debug con GDB"
	@echo "  🖥️  make shell    - Shell interactivo"

# Objetivo para verificar herramientas dentro del contenedor
check-tools: check-docker check-image
	@echo "$(YELLOW)🔧 Verificando herramientas en el contenedor...$(NC)"
	docker run --rm \
		$(DOCKER_IMAGE):$(DOCKER_TAG) \
		check-environment

# Objetivo para desarrolladores: reconstruir imagen forzadamente
rebuild: check-docker
	@echo "$(YELLOW)🔄 Reconstruyendo imagen Docker (sin cache)...$(NC)"
	docker build --no-cache -t $(DOCKER_IMAGE):$(DOCKER_TAG) .
	@echo "$(GREEN)✅ Imagen reconstruida$(NC)"

# Objetivo para ver logs de compilación detallados
verbose-compile: check-docker check-image
	@echo "$(YELLOW)🔨 Compilación verbose usando Docker...$(NC)"
	docker run --rm \
		-v $(PROJECT_ROOT):/workspace \
		-w /workspace \
		$(DOCKER_IMAGE):$(DOCKER_TAG) \
		make V=1

# Objetivo para ejecutar tests (si los hay)
test: check-docker check-image
	@echo "$(YELLOW)🧪 Ejecutando tests...$(NC)"
	docker run --rm \
		-v $(PROJECT_ROOT):/workspace \
		-w /workspace \
		$(DOCKER_IMAGE):$(DOCKER_TAG) \
		bash -c "make clean && make && echo 'Tests básicos: compilación exitosa'"

# Objetivo para generar documentación del entorno
docs:
	@echo "$(GREEN)📚 Documentación del Entorno Docker$(NC)" > DOCKER_USAGE.md
	@echo "" >> DOCKER_USAGE.md
	@echo "## Instalación y Uso" >> DOCKER_USAGE.md
	@echo "" >> DOCKER_USAGE.md
	@echo "### Primer Uso" >> DOCKER_USAGE.md
	@echo "\`\`\`bash" >> DOCKER_USAGE.md
	@echo "cd docker-build" >> DOCKER_USAGE.md
	@echo "make setup      # Construir imagen (una sola vez)" >> DOCKER_USAGE.md
	@echo "make compile    # Compilar kernel" >> DOCKER_USAGE.md
	@echo "make run        # Ejecutar kernel" >> DOCKER_USAGE.md
	@echo "\`\`\`" >> DOCKER_USAGE.md
	@echo "" >> DOCKER_USAGE.md
	@echo "### Desarrollo Diario" >> DOCKER_USAGE.md
	@echo "\`\`\`bash" >> DOCKER_USAGE.md
	@echo "make compile    # Recompilar después de cambios" >> DOCKER_USAGE.md
	@echo "make run        # Probar kernel" >> DOCKER_USAGE.md
	@echo "make debug      # Debugging con GDB" >> DOCKER_USAGE.md
	@echo "make shell      # Shell para exploración" >> DOCKER_USAGE.md
	@echo "\`\`\`" >> DOCKER_USAGE.md
	@echo "$(GREEN)✅ Documentación generada en DOCKER_USAGE.md$(NC)"

# Limpieza de contenedores huérfanos
clean-containers:
	@echo "$(YELLOW)🧹 Limpiando contenedores Docker huérfanos...$(NC)"
	@docker container prune -f
	@docker image prune -f
	@echo "$(GREEN)✅ Contenedores limpiados$(NC)"

# Mostrar tamaño de la imagen
image-size: check-image
	@echo "$(YELLOW)📦 Tamaño de la imagen Docker:$(NC)"
	@docker image ls $(DOCKER_IMAGE):$(DOCKER_TAG) --format "table {{.Repository}}\t{{.Tag}}\t{{.Size}}"

# Exportar imagen para compartir
export-image: check-image
	@echo "$(YELLOW)📤 Exportando imagen Docker...$(NC)"
	docker save $(DOCKER_IMAGE):$(DOCKER_TAG) | gzip > minikernel-dev.tar.gz
	@echo "$(GREEN)✅ Imagen exportada como minikernel-dev.tar.gz$(NC)"
	@echo "$(YELLOW)💡 Para importar: docker load < minikernel-dev.tar.gz$(NC)"

# Importar imagen desde archivo
import-image:
	@echo "$(YELLOW)📥 Importando imagen Docker...$(NC)"
	@test -f minikernel-dev.tar.gz || (echo "$(RED)❌ Archivo minikernel-dev.tar.gz no encontrado$(NC)" && exit 1)
	docker load < minikernel-dev.tar.gz
	@echo "$(GREEN)✅ Imagen importada$(NC)"