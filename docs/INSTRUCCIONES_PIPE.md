# 🔧 Instrucciones para probar el carácter PIPE "|"

## ¿Qué hemos implementado?

1. **Soporte mejorado para el carácter "|"**:
   - Mapeo en scancode 0x56 (Shift + Backslash)
   - Mapeo alternativo en scancode 0x2B para diferentes layouts
   - Manejo correcto del carácter en el shell

2. **Comando de test especial**: `testpipe`
   - Te ayuda a encontrar qué combinación de teclas genera el "|"
   - Muestra el código hexadecimal de cada carácter que presiones
   - Detecta automáticamente cuando encuentres el pipe

3. **Funcionalidad de pipes completa**:
   - `ls | wc` - Contar archivos
   - `cat archivo.txt | grep palabra` - Buscar en archivos
   - `echo "texto" | grep palabra` - Buscar en texto
   - `ls | head 3` - Mostrar primeros archivos

## 🧪 Cómo probar:

### Paso 1: Ejecutar el kernel
```bash
make run
```

### Paso 2: Usar el comando de test
```bash
shell> testpipe
```

### Paso 3: Probar diferentes combinaciones de teclas:

**En Mac, prueba estas combinaciones:**
- `Shift + \` (backslash) 
- `Alt + L`
- `Shift + Alt + L`
- `Option + 7` (en teclado español)
- `Shift + Option + L`

### Paso 4: El comando te dirá cuando encuentres el pipe
Verás algo como:
```
test> 0x7c('|')
¡EXCELENTE! Encontraste el caracter pipe: |
Ahora puedes usar pipes como: ls | wc
```

### Paso 5: Probar pipes reales
Una vez que sepas cómo escribir "|", prueba:
```bash
shell> touch test.txt
shell> copycon test.txt
# Escribe algunas líneas, termina con Ctrl+Z
shell> ls | wc          # Contar archivos
shell> cat test.txt | grep palabra    # Buscar texto
```

## 📋 Layout de teclado Mac vs PC

- **PC US**: Shift + \ = |
- **Mac US**: Shift + \ = |  
- **Mac Español**: Option + 1 = | (a veces)
- **Mac Internacional**: Pueden variar

¡El comando `testpipe` te ayudará a encontrar la combinación correcta para tu teclado!