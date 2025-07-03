# 🔧 Sistema de Pipes Mejorado - Mini Kernel Educativo

## ✅ **¡PROBLEMA RESUELTO!**

Ahora el sistema de pipes soporta **muchos más comandos**, incluyendo `rev` y `ls`. 

---

## 📋 **Comandos Soportados en Pipes**

### **🔵 Comandos de ENTRADA (cmd1)** - Generan datos
- `ls` - Lista archivos del directorio
- `cat <archivo>` - Lee contenido de archivo
- `echo <texto>` - Genera texto
- `rev <texto>` - Genera texto invertido
- `date` - Muestra fecha actual
- `whoami` - Muestra usuario actual
- `uname` - Muestra información del sistema

### **🟢 Comandos de SALIDA (cmd2)** - Procesan datos
- `grep <patrón>` - Busca texto/patrones
- `wc` - Cuenta líneas, palabras, caracteres
- `head [n]` - Muestra primeras n líneas (def: 10)
- `tail [n]` - Muestra últimas n líneas (def: 10)
- `rev` - Invierte todo el texto
- `sort` - Ordena líneas (básico)
- `uniq` - Muestra líneas únicas (básico)  
- `cut` - Extrae campos (básico)

---

## 🧪 **Ejemplos de Pipes que Ahora Funcionan**

### **Básicos**
```bash
ls | wc                    # Contar archivos
echo "hello world" | rev   # Invertir texto
date | rev                 # Fecha invertida
whoami | grep root         # Buscar usuario
```

### **Con Archivos**
```bash
cat archivo.txt | grep palabra     # Buscar en archivo
cat archivo.txt | wc               # Contar contenido de archivo
cat archivo.txt | head 5           # Primeras 5 líneas
```

### **Combinaciones Creativas**
```bash
rev "hello world" | grep oleh      # Invertir y buscar
echo "una\ndos\ntres" | tail 2     # Últimas 2 líneas
uname | rev                        # Info del sistema invertida
ls | head 3                        # Primeros 3 archivos
```

### **Análisis de Datos**
```bash
cat datos.txt | grep error | wc    # Contar errores en archivo
ls | grep .txt                     # Solo archivos .txt
echo "test\ntest\nunique" | uniq   # Líneas únicas
```

---

## 🔍 **Comando de Diagnóstico**

Si el carácter "|" no funciona en tu teclado:

```bash
shell> testpipe
```

Este comando te ayudará a:
- Encontrar qué combinación de teclas genera "|"
- Mostrar códigos hexadecimales de cada tecla
- Detectar automáticamente cuando encuentres el pipe
- Darte ejemplos de uso una vez que funcione

---

## 🛠️ **Cómo Funciona Internamente**

1. **Detección**: `strchr(cmdbuf, '|')` encuentra el carácter pipe
2. **Separación**: Divide el comando en cmd1 y cmd2
3. **Ejecución cmd1**: Captura salida en buffer de 4KB
4. **Procesamiento cmd2**: Aplica el segundo comando al buffer
5. **Resultado**: Muestra el resultado final

---

## 🎯 **Para Probar**

1. **Ejecuta el kernel:**
   ```bash
   make run
   ```

2. **Encuentra el carácter pipe (si es necesario):**
   ```bash
   shell> testpipe
   ```

3. **Prueba los nuevos pipes:**
   ```bash
   shell> echo "hello world" | rev
   shell> ls | wc
   shell> date | grep 2024
   ```

---

## 🚀 **¡Ahora funciona todo!**

- ✅ `rev` soportado como entrada Y salida
- ✅ `ls` funciona perfectamente en pipes  
- ✅ Muchos más comandos agregados
- ✅ Mensajes de error informativos
- ✅ Ejemplos y documentación completa

¡Disfruta experimentando con el sistema de pipes mejorado! 🎉