#!/bin/bash
# Script para ejecutar GUI en macOS con Docker

# Colores
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m'

echo -e "${YELLOW}🍎 Configurando GUI para macOS...${NC}"

# Verificar si XQuartz está instalado
if ! command -v xquartz &> /dev/null; then
    echo -e "${RED}❌ XQuartz no está instalado${NC}"
    echo -e "${YELLOW}💡 Instala XQuartz desde: https://www.xquartz.org/${NC}"
    echo -e "${YELLOW}💡 O usa Homebrew: brew install --cask xquartz${NC}"
    exit 1
fi

# Verificar si XQuartz está ejecutándose
if ! pgrep -x "Xquartz" > /dev/null; then
    echo -e "${YELLOW}⚠️  XQuartz no está ejecutándose${NC}"
    echo -e "${YELLOW}💡 Iniciando XQuartz...${NC}"
    open -a XQuartz
    echo -e "${YELLOW}💡 Esperando que XQuartz inicie...${NC}"
    sleep 3
fi

# Obtener IP del host para macOS
HOST_IP=$(ifconfig en0 | grep inet | awk '$1=="inet" {print $2}')
if [ -z "$HOST_IP" ]; then
    HOST_IP="localhost"
fi

echo -e "${GREEN}✅ Configuración GUI lista${NC}"
echo -e "${YELLOW}📱 Host IP: $HOST_IP${NC}"

# Permitir conexiones X11 desde Docker
xhost +$HOST_IP

# Ejecutar contenedor con GUI
echo -e "${YELLOW}🚀 Ejecutando Mini-Kernel con GUI...${NC}"
docker run --rm -it \
    -e DISPLAY=$HOST_IP:0 \
    -v /tmp/.X11-unix:/tmp/.X11-unix:rw \
    -v $(dirname $PWD):/workspace \
    -w /workspace \
    minikernel-dev:latest \
    make run-gui

# Limpiar permisos X11
xhost -$HOST_IP

echo -e "${GREEN}✅ Sesión GUI terminada${NC}"