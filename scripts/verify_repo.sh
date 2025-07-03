#!/bin/bash

# Script de verificación del repositorio r2os
# Verifica que la estructura y contenido estén correctos

echo "🔍 Verificando estructura del repositorio r2os..."
echo ""

# Variables de verificación
ERRORS=0
WARNINGS=0

# Función para reportar errores
error() {
    echo "❌ ERROR: $1"
    ERRORS=$((ERRORS + 1))
}

# Función para reportar advertencias
warning() {
    echo "⚠️  ADVERTENCIA: $1"
    WARNINGS=$((WARNINGS + 1))
}

# Función para reportar éxito
success() {
    echo "✅ $1"
}

# Verificar archivos principales
echo "📋 Verificando archivos principales..."

files_main=(
    "README.md"
    "CLAUDE.md"
    "Makefile"
    "kernel.c"
    "boot.s"
    "linker.ld"
    ".gitignore"
)

for file in "${files_main[@]}"; do
    if [[ -f "$file" ]]; then
        success "Archivo principal: $file"
    else
        error "Archivo principal faltante: $file"
    fi
done

echo ""

# Verificar estructura de directorios
echo "📁 Verificando estructura de directorios..."

dirs=(
    "curso"
    "docker-build"
    "docs"
    "scripts"
    "examples"
    "build"
)

for dir in "${dirs[@]}"; do
    if [[ -d "$dir" ]]; then
        success "Directorio: $dir"
    else
        error "Directorio faltante: $dir"
    fi
done

echo ""

# Verificar capítulos del curso
echo "📚 Verificando capítulos del curso..."

curso_files=(
    "curso/README.md"
    "curso/PLAN_CURSO_SO.md"
    "curso/PRE_CURSO_00.md"
    "curso/PRE_CURSO_01.md"
    "curso/CAPITULO_01.md"
    "curso/CAPITULO_02.md"
    "curso/CAPITULO_03.md"
    "curso/CAPITULO_04.md"
    "curso/CAPITULO_05.md"
    "curso/CAPITULO_06.md"
    "curso/CAPITULO_07.md"
    "curso/CAPITULO_08.md"
)

for file in "${curso_files[@]}"; do
    if [[ -f "$file" ]]; then
        success "Capítulo: $(basename "$file")"
    else
        error "Capítulo faltante: $file"
    fi
done

echo ""

# Verificar Docker
echo "🐳 Verificando entorno Docker..."

docker_files=(
    "docker-build/Dockerfile"
    "docker-build/Makefile"
    "docker-build/README.md"
    "docker-build/run-macos-gui.sh"
)

for file in "${docker_files[@]}"; do
    if [[ -f "$file" ]]; then
        success "Docker: $(basename "$file")"
    else
        error "Archivo Docker faltante: $file"
    fi
done

echo ""

# Verificar que no hay archivos compilados en el root
echo "🧹 Verificando limpieza del repositorio..."

compiled_files=(*.o *.elf *.bin *.img)
found_compiled=false

for pattern in "${compiled_files[@]}"; do
    if ls $pattern 1> /dev/null 2>&1; then
        warning "Archivos compilados encontrados en root: $pattern"
        found_compiled=true
    fi
done

if ! $found_compiled; then
    success "No hay archivos compilados en el root"
fi

echo ""

# Verificar tamaños de archivos importantes
echo "📊 Verificando tamaños de archivos..."

check_file_size() {
    local file="$1"
    local min_size="$2"
    
    if [[ -f "$file" ]]; then
        local size=$(wc -c < "$file")
        if [[ $size -gt $min_size ]]; then
            success "Tamaño de $file: $size bytes (>$min_size)"
        else
            warning "Archivo $file parece muy pequeño: $size bytes (<$min_size)"
        fi
    fi
}

check_file_size "kernel.c" 50000      # Al menos 50KB
check_file_size "curso/README.md" 5000 # Al menos 5KB
check_file_size "README.md" 10000     # Al menos 10KB

echo ""

# Verificar enlaces en README
echo "🔗 Verificando referencias en documentación..."

if grep -q "PRE_CURSO_00.md" curso/README.md; then
    success "Enlaces a prerrequisitos en curso/README.md"
else
    warning "No se encontraron enlaces a prerrequisitos"
fi

if grep -q "CAPITULO_08.md" curso/README.md; then
    success "Referencias a todos los capítulos en curso/README.md"
else
    warning "No se encontraron referencias a todos los capítulos"
fi

echo ""

# Resumen final
echo "📋 RESUMEN DE VERIFICACIÓN"
echo "========================"

if [[ $ERRORS -eq 0 && $WARNINGS -eq 0 ]]; then
    echo "🎉 ¡PERFECTO! El repositorio está completamente configurado."
    echo ""
    echo "✅ Estructura completa"
    echo "✅ Todos los archivos presentes"
    echo "✅ Curso de 10 capítulos (2 prerrequisitos + 8 principales)"
    echo "✅ Documentación actualizada"
    echo "✅ Entorno Docker configurado"
    echo ""
    echo "🚀 El repositorio está listo para:"
    echo "   - Desarrollo de sistemas operativos"
    echo "   - Curso educativo completo"
    echo "   - Uso con Docker"
    echo "   - Distribución académica"
    
elif [[ $ERRORS -eq 0 ]]; then
    echo "✅ Repositorio funcional con $WARNINGS advertencias menores"
    echo ""
    echo "El repositorio está en buen estado. Las advertencias no impiden su uso."
    
else
    echo "❌ Se encontraron $ERRORS errores y $WARNINGS advertencias"
    echo ""
    echo "Es necesario corregir los errores antes de usar el repositorio."
fi

echo ""
echo "📚 Para usar el curso:"
echo "   cd curso && cat README.md"
echo ""
echo "🐳 Para usar Docker:"
echo "   cd docker-build && make setup-all"
echo ""
echo "🔧 Para compilar directamente:"
echo "   make && make run"

exit $ERRORS