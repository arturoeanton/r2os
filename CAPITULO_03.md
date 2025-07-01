# Capítulo 3: Sistemas de Archivos

## 📚 Introducción

Este capítulo explora la implementación de sistemas de archivos, uno de los componentes más críticos de cualquier sistema operativo. Utilizaremos el sistema FAT16 implementado en el mini-kernel educativo como caso de estudio para comprender cómo se organizan, almacenan y manipulan los datos de forma persistente.

## 🎯 Objetivos de Aprendizaje

Al completar este capítulo, el estudiante será capaz de:

1. **Explicar** la estructura y organización de sistemas de archivos
2. **Implementar** operaciones básicas CRUD sobre archivos
3. **Analizar** la estructura interna de FAT16 y sus componentes
4. **Diseñar** algoritmos eficientes para manipulación de archivos
5. **Optimizar** operaciones de E/S para mejorar rendimiento

---

## 🏛️ Parte Teórica

### 1.1 Fundamentos de Sistemas de Archivos

#### ¿Qué es un Sistema de Archivos?

Un sistema de archivos es la estructura lógica que permite al sistema operativo organizar, almacenar y recuperar datos en dispositivos de almacenamiento. Proporciona una abstracción sobre el hardware de almacenamiento raw.

```
┌─────────────────────────────────────────────────┐
│                 APLICACIONES                    │
├─────────────────────────────────────────────────┤
│            API del Sistema de Archivos          │
│     open(), read(), write(), close()           │
├─────────────────────────────────────────────────┤
│          IMPLEMENTACIÓN DEL FS                  │
│    FAT16, ext4, NTFS, ZFS, etc.               │
├─────────────────────────────────────────────────┤
│             CAPA DE BLOQUES                     │
│        Sectores, Clusters, Buffering           │
├─────────────────────────────────────────────────┤
│            HARDWARE DE ALMACENAMIENTO           │
│         HDD, SSD, Flash Memory                 │
└─────────────────────────────────────────────────┘
```

#### Funciones Principales

**1. Organización de Datos:**
- Estructuración jerárquica (directorios y archivos)
- Metadata (tamaño, fechas, permisos)
- Localización física de datos

**2. Gestión del Espacio:**
- Asignación y liberación de espacio
- Fragmentación y compactación
- Optimización de acceso

**3. Integridad y Consistencia:**
- Detección y corrección de errores
- Transacciones atómicas
- Journaling y rollback

**4. Rendimiento:**
- Caching y buffering
- Prefetching de datos
- Optimización de patrones de acceso

### 1.2 Taxonomía de Sistemas de Archivos

#### Clasificación por Estructura

**1. File Allocation Table (FAT)**
```
┌─────────────┬─────────────┬─────────────┬─────────────┐
│ Boot Sector │ FAT Tables  │ Root Dir    │ Data Area   │
│   (1 sect)  │ (variable)  │ (32 entries)│ (remaining) │
└─────────────┴─────────────┴─────────────┴─────────────┘
```
- **Ventajas**: Simplicidad, compatibilidad universal
- **Desventajas**: Fragmentación, límites de tamaño

**2. Ext (Extended File System)**
```
┌─────────────┬─────────────┬─────────────┬─────────────┐
│ Superblock  │ Group Desc  │ Inode Table │ Data Blocks │
│             │ Table       │             │             │
└─────────────┴─────────────┴─────────────┴─────────────┘
```
- **Ventajas**: Eficiencia, escalabilidad
- **Desventajas**: Complejidad, específico a Unix/Linux

**3. NTFS (New Technology File System)**
```
┌─────────────┬─────────────┬─────────────┬─────────────┐
│ Boot Sector │ Master File │ Metadata    │ File Data   │
│             │ Table (MFT) │             │             │
└─────────────┴─────────────┴─────────────┴─────────────┘
```
- **Ventajas**: Journaling, seguridad avanzada
- **Desventajas**: Complejidad, propietario

#### Clasificación por Características

**Sistemas Locales vs Distribuidos:**
- **Locales**: FAT, ext4, NTFS, ZFS
- **Distribuidos**: NFS, CIFS/SMB, AFS

**Con/Sin Journaling:**
- **Con journaling**: ext3/4, NTFS, ZFS, ReiserFS
- **Sin journaling**: FAT, ext2, original Unix FS

### 1.3 FAT16 en Detalle

#### Arquitectura General

FAT16 (File Allocation Table 16-bit) organiza el almacenamiento en las siguientes áreas:

```
Sector 0:    Boot Sector
Sectores 1-X: File Allocation Tables (FAT) - usualmente 2 copias
Sector X+1:  Root Directory (32 entries máximo)
Resto:       Data Area (clusters de datos)
```

#### Boot Sector

El boot sector contiene información crítica del sistema de archivos:

```c
struct fat16_boot_sector {
    uint8_t  jump[3];           // Instrución de salto
    uint8_t  oem_name[8];       // Nombre del OEM
    uint16_t bytes_per_sector;  // Bytes por sector (512)
    uint8_t  sectors_per_cluster; // Sectores por cluster
    uint16_t reserved_sectors;  // Sectores reservados
    uint8_t  num_fats;         // Número de FATs (2)
    uint16_t root_entries;     // Entradas en directorio raíz
    uint16_t total_sectors;    // Sectores totales
    uint8_t  media_type;       // Tipo de medio
    uint16_t sectors_per_fat;  // Sectores por FAT
    uint16_t sectors_per_track; // Sectores por pista
    uint16_t num_heads;        // Número de cabezas
    uint32_t hidden_sectors;   // Sectores ocultos
    uint32_t large_sectors;    // Sectores (si >65535)
    // ... más campos
} __attribute__((packed));
```

#### File Allocation Table (FAT)

La FAT es una tabla que mapea clusters a su estado:

```
Valor FAT16    Significado
0x0000        Cluster libre
0x0001        Reservado
0x0002-0xFFEF Cluster en uso (apunta al siguiente)
0xFFF0-0xFFF6 Reservado
0xFFF7        Cluster defectuoso
0xFFF8-0xFFFF Final de cadena de clusters
```

**Ejemplo de cadena de clusters:**
```
Archivo grande que ocupa clusters 2, 5, 6, 9:
FAT[2] = 5    (cluster 2 apunta a 5)
FAT[5] = 6    (cluster 5 apunta a 6)
FAT[6] = 9    (cluster 6 apunta a 9)
FAT[9] = 0xFFFF (cluster 9 es el final)
```

#### Directory Entries

Cada entrada de directorio ocupa 32 bytes:

```c
struct fat16_dir_entry {
    char     name[11];          // Nombre 8.3 (FILENAME.EXT)
    uint8_t  attr;             // Atributos del archivo
    uint8_t  reserved[10];     // Reservado
    uint16_t first_cluster;    // Primer cluster del archivo
    uint32_t size;             // Tamaño en bytes
} __attribute__((packed));
```

**Atributos de archivo:**
```
Bit 0: Read-only
Bit 1: Hidden
Bit 2: System
Bit 3: Volume label
Bit 4: Directory
Bit 5: Archive
Bit 6-7: Reservados
```

**Formato de nombre 8.3:**
```
"HELLO   TXT" → hello.txt
"DOCUMENT   " → document (sin extensión)
"TEST    C  " → test.c
```

### 1.4 Algoritmos de Asignación

#### Estrategias de Asignación de Clusters

**1. First Fit:**
```c
uint16_t find_free_cluster_first_fit(void) {
    for (uint16_t i = 2; i < total_clusters; i++) {
        if (fat_table[i] == 0) return i;
    }
    return 0; // No hay clusters libres
}
```
- **Ventaja**: Rápido
- **Desventaja**: Fragmentación externa

**2. Best Fit:**
```c
uint16_t find_free_cluster_best_fit(size_t needed_clusters) {
    uint16_t best_start = 0;
    size_t best_size = SIZE_MAX;
    
    for (uint16_t i = 2; i < total_clusters; i++) {
        if (fat_table[i] == 0) {
            size_t free_size = count_contiguous_free(i);
            if (free_size >= needed_clusters && free_size < best_size) {
                best_start = i;
                best_size = free_size;
            }
        }
    }
    return best_start;
}
```
- **Ventaja**: Minimiza fragmentación
- **Desventaja**: Más lento

**3. Next Fit:**
```c
static uint16_t last_allocated = 2;

uint16_t find_free_cluster_next_fit(void) {
    for (uint16_t i = last_allocated; i < total_clusters; i++) {
        if (fat_table[i] == 0) {
            last_allocated = i;
            return i;
        }
    }
    // Buscar desde el principio
    for (uint16_t i = 2; i < last_allocated; i++) {
        if (fat_table[i] == 0) {
            last_allocated = i;
            return i;
        }
    }
    return 0;
}
```
- **Ventaja**: Distribuye archivos uniformemente
- **Desventaja**: Puede aumentar fragmentación

#### Optimizaciones de Rendimiento

**1. Caching de la FAT:**
```c
struct fat_cache {
    uint16_t fat_table[MAX_CLUSTERS];
    bool dirty;
    uint32_t last_access_time;
};

static struct fat_cache fat_cache = {0};

void flush_fat_cache(void) {
    if (fat_cache.dirty) {
        write_sectors(FAT_START_SECTOR, fat_cache.fat_table, SECTORS_PER_FAT);
        fat_cache.dirty = false;
    }
}
```

**2. Prefetching de Directorios:**
```c
void prefetch_directory(uint16_t cluster) {
    // Cargar todo el directorio en memoria
    for (uint16_t c = cluster; c != 0xFFFF; c = fat_table[c]) {
        load_cluster_to_cache(c);
    }
}
```

---

## 🔧 Parte Práctica

### 2.1 Análisis del Sistema FAT16 del Mini-Kernel

#### Estructura de Datos Principal

```c
// kernel.c: líneas 122-130
typedef struct __attribute__((packed)) {
    char     name[11];      // Nombre en formato 8.3
    uint8_t  attr;         // Atributos del archivo
    uint8_t  reserved[10]; // Campos reservados
    uint16_t first_cluster; // Primer cluster de datos
    uint32_t size;         // Tamaño en bytes
} fat16_dir_entry;
```

#### Layout de Memoria del Sistema de Archivos

```c
// kernel.c: líneas 133-145
#define SECTOR_SIZE 512
#define TOTAL_SECTORS 289
#define FAT_SECTORS 256
#define ROOT_DIR_SECTORS 32
#define DATA_START_SECTOR (1 + FAT_SECTORS + ROOT_DIR_SECTORS)

// Simulación del disco en RAM
static uint8_t disk_image[TOTAL_SECTORS * SECTOR_SIZE];
```

**Distribución de sectores:**
```
Sector 0:      Boot sector (no usado en nuestra implementación)
Sectores 1-256: File Allocation Table
Sectores 257-288: Root directory (32 entradas)
Sector 289+:   Área de datos (clusters)
```

### 2.2 Operaciones Básicas del Sistema de Archivos

#### Inicialización

```c
// kernel.c: líneas 147-180
static void fs_init(void) {
    // Limpiar toda la imagen del disco
    memset(disk_image, 0, sizeof(disk_image));
    
    // Inicializar FAT: marcar clusters 0 y 1 como reservados
    uint16_t *fat = (uint16_t*)(disk_image + SECTOR_SIZE);
    fat[0] = 0xFFF8;  // Media descriptor
    fat[1] = 0xFFFF;  // End-of-chain marker
    
    printf("Inicializando filesystem FAT16... OK\n");
}
```

#### Buscar Archivo

```c
// kernel.c: líneas 225-250
static int fs_find(const char *name, fat16_dir_entry *e, int *idx) {
    // Convertir nombre a formato 8.3
    char fat_name[11];
    memset(fat_name, ' ', 11);
    
    // Procesar nombre.extensión
    int name_len = 0;
    int i = 0;
    
    // Copiar nombre (hasta 8 caracteres o punto)
    while (name[i] && name[i] != '.' && name_len < 8) {
        fat_name[name_len++] = name[i++];
    }
    
    // Si hay extensión, procesarla
    if (name[i] == '.') {
        i++; // saltar el punto
        int ext_len = 0;
        while (name[i] && ext_len < 3) {
            fat_name[8 + ext_len++] = name[i++];
        }
    }
    
    // Buscar en directorio raíz
    fat16_dir_entry *root_dir = (fat16_dir_entry*)(disk_image + 
                               (1 + FAT_SECTORS) * SECTOR_SIZE);
    
    for (int j = 0; j < 32; j++) {
        if (root_dir[j].name[0] == 0x00) break; // Fin de directorio
        if (root_dir[j].name[0] == 0xE5) continue; // Entrada borrada
        
        if (memcmp(root_dir[j].name, fat_name, 11) == 0) {
            if (e) *e = root_dir[j];
            if (idx) *idx = j;
            return 1; // Encontrado
        }
    }
    
    return 0; // No encontrado
}
```

#### Asignación de Clusters

```c
// kernel.c: líneas 182-200
static uint16_t fs_alloc_cluster(void) {
    uint16_t *fat = (uint16_t*)(disk_image + SECTOR_SIZE);
    
    // Buscar primer cluster libre (comenzando desde cluster 2)
    for (uint16_t i = 2; i < (FAT_SECTORS * SECTOR_SIZE) / 2; i++) {
        if (fat[i] == 0) {
            fat[i] = 0xFFFF; // Marcar como final de cadena
            return i;
        }
    }
    
    return 0; // No hay clusters libres
}
```

### 2.3 Operaciones CRUD

#### Create (Crear Archivo)

```c
// kernel.c: líneas 290-350
static int fs_create_internal(const char *name) {
    // Verificar si ya existe
    if (fs_find(name, NULL, NULL)) {
        return 0; // Ya existe
    }
    
    // Buscar entrada libre en directorio raíz
    fat16_dir_entry *root_dir = (fat16_dir_entry*)(disk_image + 
                               (1 + FAT_SECTORS) * SECTOR_SIZE);
    
    int free_entry = -1;
    for (int i = 0; i < 32; i++) {
        if (root_dir[i].name[0] == 0x00 || root_dir[i].name[0] == 0xE5) {
            free_entry = i;
            break;
        }
    }
    
    if (free_entry == -1) {
        return 0; // Directorio lleno
    }
    
    // Convertir nombre a formato 8.3
    char fat_name[11];
    memset(fat_name, ' ', 11);
    // ... conversión de nombre ...
    
    // Crear entrada de directorio
    memcpy(root_dir[free_entry].name, fat_name, 11);
    root_dir[free_entry].attr = 0x00;
    root_dir[free_entry].first_cluster = 0;
    root_dir[free_entry].size = 0;
    
    return 1; // Éxito
}
```

#### Read (Leer Archivo)

```c
// kernel.c: líneas 252-290
static int fs_read(const char *name, void *buf, uint32_t *size) {
    fat16_dir_entry entry;
    if (!fs_find(name, &entry, NULL)) {
        return 0; // Archivo no encontrado
    }
    
    *size = entry.size;
    if (entry.size == 0) {
        return 1; // Archivo vacío
    }
    
    // Leer datos siguiendo cadena de clusters
    uint8_t *output = (uint8_t*)buf;
    uint32_t bytes_read = 0;
    uint16_t cluster = entry.first_cluster;
    uint16_t *fat = (uint16_t*)(disk_image + SECTOR_SIZE);
    
    while (cluster != 0 && cluster < 0xFFF8 && bytes_read < entry.size) {
        // Calcular dirección del cluster
        uint32_t cluster_addr = (DATA_START_SECTOR + cluster - 2) * SECTOR_SIZE;
        
        // Leer cluster completo o resto del archivo
        uint32_t to_read = (entry.size - bytes_read > SECTOR_SIZE) ? 
                          SECTOR_SIZE : (entry.size - bytes_read);
        
        memcpy(output + bytes_read, disk_image + cluster_addr, to_read);
        bytes_read += to_read;
        
        // Siguiente cluster en la cadena
        cluster = fat[cluster];
    }
    
    return 1; // Éxito
}
```

#### Write (Escribir Archivo)

```c
// kernel.c: líneas 350-420
static void fs_write(const char *name, const void *buf, uint32_t size) {
    fat16_dir_entry entry;
    int entry_idx;
    
    // Si el archivo existe, liberar clusters antiguos
    if (fs_find(name, &entry, &entry_idx)) {
        // Liberar clusters existentes
        uint16_t *fat = (uint16_t*)(disk_image + SECTOR_SIZE);
        uint16_t cluster = entry.first_cluster;
        
        while (cluster != 0 && cluster < 0xFFF8) {
            uint16_t next = fat[cluster];
            fat[cluster] = 0; // Marcar como libre
            cluster = next;
        }
    } else {
        // Crear nuevo archivo
        fs_create_internal(name);
        fs_find(name, &entry, &entry_idx);
    }
    
    // Asignar clusters para los datos
    if (size > 0) {
        uint16_t first_cluster = fs_alloc_cluster();
        uint16_t current_cluster = first_cluster;
        uint32_t bytes_written = 0;
        uint8_t *input = (uint8_t*)buf;
        uint16_t *fat = (uint16_t*)(disk_image + SECTOR_SIZE);
        
        while (bytes_written < size) {
            // Escribir datos al cluster actual
            uint32_t cluster_addr = (DATA_START_SECTOR + current_cluster - 2) * SECTOR_SIZE;
            uint32_t to_write = (size - bytes_written > SECTOR_SIZE) ? 
                               SECTOR_SIZE : (size - bytes_written);
            
            memcpy(disk_image + cluster_addr, input + bytes_written, to_write);
            bytes_written += to_write;
            
            // Si necesitamos más espacio, asignar siguiente cluster
            if (bytes_written < size) {
                uint16_t next_cluster = fs_alloc_cluster();
                fat[current_cluster] = next_cluster;
                current_cluster = next_cluster;
            }
        }
        
        // Actualizar entrada de directorio
        fat16_dir_entry *root_dir = (fat16_dir_entry*)(disk_image + 
                                   (1 + FAT_SECTORS) * SECTOR_SIZE);
        root_dir[entry_idx].first_cluster = first_cluster;
        root_dir[entry_idx].size = size;
    }
}
```

#### Delete (Eliminar Archivo)

```c
// Función de eliminación (implementada en el shell)
static void fs_delete(const char *name) {
    fat16_dir_entry entry;
    int entry_idx;
    
    if (!fs_find(name, &entry, &entry_idx)) {
        printf("Archivo no encontrado: %s\n", name);
        return;
    }
    
    // Liberar clusters
    uint16_t *fat = (uint16_t*)(disk_image + SECTOR_SIZE);
    uint16_t cluster = entry.first_cluster;
    
    while (cluster != 0 && cluster < 0xFFF8) {
        uint16_t next = fat[cluster];
        fat[cluster] = 0; // Marcar como libre
        cluster = next;
    }
    
    // Marcar entrada de directorio como borrada
    fat16_dir_entry *root_dir = (fat16_dir_entry*)(disk_image + 
                               (1 + FAT_SECTORS) * SECTOR_SIZE);
    root_dir[entry_idx].name[0] = 0xE5; // Marca de archivo borrado
    
    printf("Archivo eliminado: %s\n", name);
}
```

### 2.4 Funcionalidades Avanzadas

#### Edición de Líneas

El mini-kernel incluye funcionalidades avanzadas para edición de archivos línea por línea:

```c
// Editar línea específica de un archivo
static void fs_edit_line(const char *name, int line_num, const char *new_text) {
    // Leer archivo completo
    uint8_t buffer[SECTOR_SIZE];
    uint32_t size;
    if (!fs_read(name, buffer, &size)) {
        // Si no existe, crear nuevo archivo
        fs_create_internal(name);
        size = 0;
    }
    
    // Separar en líneas
    char lines[100][80]; // Máximo 100 líneas de 80 caracteres
    int total_lines = 0;
    
    // Parse del contenido existente
    int pos = 0;
    for (int i = 0; i < size && total_lines < 100; i++) {
        if (buffer[i] == '\n' || pos >= 79) {
            lines[total_lines][pos] = '\0';
            total_lines++;
            pos = 0;
        } else {
            lines[total_lines][pos++] = buffer[i];
        }
    }
    
    // Asegurar que hay suficientes líneas
    while (total_lines < line_num) {
        lines[total_lines][0] = '\0';
        total_lines++;
    }
    
    // Reemplazar línea específica
    strncpy(lines[line_num - 1], new_text, 79);
    lines[line_num - 1][79] = '\0';
    
    // Reconstruir archivo
    char new_content[SECTOR_SIZE];
    int new_size = 0;
    for (int i = 0; i < total_lines && new_size < SECTOR_SIZE - 2; i++) {
        int len = strlen(lines[i]);
        memcpy(new_content + new_size, lines[i], len);
        new_size += len;
        if (new_size < SECTOR_SIZE - 1) {
            new_content[new_size++] = '\n';
        }
    }
    
    // Escribir archivo modificado
    fs_write(name, new_content, new_size);
}
```

---

## 📝 Ejercicios Prácticos

### Ejercicio 1: Implementar Fragmentación Inteligente
**Dificultad**: Intermedia

**Objetivo**: Mejorar el algoritmo de asignación de clusters para minimizar fragmentación.

**Instrucciones**:
1. Implementa función para encontrar bloques contiguos:
```c
uint16_t find_contiguous_clusters(size_t needed) {
    uint16_t *fat = (uint16_t*)(disk_image + SECTOR_SIZE);
    
    for (uint16_t start = 2; start < MAX_CLUSTERS - needed; start++) {
        bool found = true;
        
        // Verificar si 'needed' clusters consecutivos están libres
        for (size_t i = 0; i < needed; i++) {
            if (fat[start + i] != 0) {
                found = false;
                break;
            }
        }
        
        if (found) return start;
    }
    
    return 0; // No hay espacio contiguo
}
```

2. Modifica `fs_write()` para usar asignación contigua cuando sea posible
3. Implementa función de desfragmentación:
```c
void defragment_filesystem(void) {
    // TODO: Mover archivos para eliminar fragmentación
    // TODO: Compactar espacio libre
}
```

**Resultado esperado**: Archivos grandes deben almacenarse en clusters contiguos cuando sea posible.

---

### Ejercicio 2: Sistema de Backup y Recuperación
**Dificultad**: Avanzada

**Objetivo**: Implementar funcionalidades de backup y recuperación de archivos.

**Instrucciones**:
1. Crea comando `backup` que genere copia de seguridad:
```c
void backup_file(const char *filename) {
    // TODO: Crear archivo .bak con timestamp
    // TODO: Copiar contenido original
    // TODO: Mantener metadata (tamaño, atributos)
}
```

2. Implementa comando `restore` para recuperar backups:
```c
void restore_file(const char *backup_name) {
    // TODO: Localizar archivo de backup
    // TODO: Extraer nombre original
    // TODO: Restaurar contenido y metadata
}
```

3. Agrega comando `fsck` para verificación de integridad:
```c
int check_filesystem_integrity(void) {
    // TODO: Verificar consistencia de FAT
    // TODO: Detectar clusters perdidos
    // TODO: Validar entradas de directorio
    // TODO: Reparar errores automáticamente
}
```

**Características adicionales**:
- Detección de corrupción en la FAT
- Recuperación de archivos borrados recientemente
- Estadísticas de uso del sistema de archivos

---

### Ejercicio 3: Compresión de Archivos
**Dificultad**: Muy Avanzada

**Objetivo**: Implementar compresión transparente de archivos pequeños.

**Instrucciones**:
1. Implementa algoritmo de compresión RLE (Run-Length Encoding):
```c
size_t compress_rle(const uint8_t *input, size_t input_size, 
                   uint8_t *output, size_t output_max) {
    size_t out_pos = 0;
    size_t in_pos = 0;
    
    while (in_pos < input_size && out_pos < output_max - 2) {
        uint8_t current = input[in_pos];
        uint8_t count = 1;
        
        // Contar repeticiones consecutivas
        while (in_pos + count < input_size && 
               input[in_pos + count] == current && 
               count < 255) {
            count++;
        }
        
        // Escribir par (count, value)
        output[out_pos++] = count;
        output[out_pos++] = current;
        in_pos += count;
    }
    
    return out_pos;
}
```

2. Modifica el sistema de archivos para comprimir automáticamente:
```c
void fs_write_compressed(const char *name, const void *buf, uint32_t size) {
    uint8_t compressed[SECTOR_SIZE];
    size_t compressed_size = compress_rle(buf, size, compressed, SECTOR_SIZE);
    
    if (compressed_size < size * 0.75) {
        // Usar versión comprimida si ahorra >25%
        // TODO: Marcar archivo como comprimido en atributos
        fs_write(name, compressed, compressed_size);
    } else {
        // Usar versión sin comprimir
        fs_write(name, buf, size);
    }
}
```

3. Implementa descompresión transparente en lectura
4. Agrega comando `compress` y `decompress` para uso manual

**Criterios de evaluación**:
- Compresión funciona correctamente
- Decompresión es transparente al usuario
- Detección automática de archivos comprimidos
- Ahorro medible de espacio en archivos de prueba

---

### Ejercicio 4: Journaling Básico
**Dificultad**: Extrema

**Objetivo**: Implementar un sistema de journaling básico para garantizar consistencia.

**Instrucciones**:
1. Define estructura del journal:
```c
struct journal_entry {
    uint32_t transaction_id;
    uint32_t operation_type;    // CREATE, WRITE, DELETE
    char filename[11];
    uint32_t old_size;
    uint16_t old_first_cluster;
    uint32_t new_size;
    uint16_t new_first_cluster;
    uint32_t timestamp;
    uint32_t checksum;
} __attribute__((packed));
```

2. Implementa transacciones atómicas:
```c
uint32_t begin_transaction(void) {
    // TODO: Generar ID único de transacción
    // TODO: Escribir entrada BEGIN en journal
}

void commit_transaction(uint32_t txn_id) {
    // TODO: Escribir entrada COMMIT en journal
    // TODO: Aplicar cambios al sistema de archivos
}

void abort_transaction(uint32_t txn_id) {
    // TODO: Escribir entrada ABORT en journal
    // TODO: Deshacer cambios parciales
}
```

3. Implementa recuperación en caso de fallo:
```c
void recover_from_journal(void) {
    // TODO: Leer journal completo
    // TODO: Encontrar transacciones incompletas
    // TODO: Aplicar o deshacer según estado
}
```

**Características avanzadas**:
- Checkpoints periódicos para truncar journal
- Detección de corrupción del journal
- Recuperación automática en el arranque
- Métricas de rendimiento del journaling

---

## 📋 Resumen del Capítulo

### Conceptos Clave Aprendidos

1. **Arquitectura de Sistemas de Archivos**:
   - Organización jerárquica de datos y metadata
   - Abstracción sobre hardware de almacenamiento
   - Diferentes paradigmas: FAT, ext, NTFS, etc.

2. **Implementación de FAT16**:
   - Estructura de boot sector, FAT y directorio raíz
   - Algoritmos de asignación y liberación de clusters
   - Formato de nombres 8.3 y entradas de directorio

3. **Operaciones CRUD**:
   - Create: Asignación de entradas y clusters
   - Read: Seguimiento de cadenas de clusters
   - Update: Modificación eficiente de contenido
   - Delete: Liberación segura de recursos

4. **Optimizaciones y Funcionalidades Avanzadas**:
   - Caching de estructuras críticas
   - Edición línea por línea de archivos
   - Detección y manejo de errores
   - Estrategias anti-fragmentación

### Habilidades Técnicas Desarrolladas

- ✅ **Diseño** e implementación de sistemas de archivos básicos
- ✅ **Algoritmos** eficientes para gestión de almacenamiento
- ✅ **Optimización** de operaciones de E/S y memoria
- ✅ **Debugging** de problemas de consistencia de datos
- ✅ **Implementación** de operaciones atómicas y transaccionales

### Conexión con el Código del Mini-Kernel

En nuestro mini-kernel educativo, estos conceptos se implementan directamente:

- **Líneas 122-145**: Definición de estructuras FAT16
- **Líneas 147-180**: Inicialización del sistema de archivos
- **Líneas 182-200**: Algoritmos de asignación de clusters
- **Líneas 225-420**: Operaciones CRUD completas
- **Comandos del shell**: Interfaz de usuario para todas las operaciones

### Preparación para el Siguiente Capítulo

El **Capítulo 4** se basará en estos fundamentos para explorar:
- Shell avanzado y comunicación inter-proceso
- Sistema de pipes para conectar comandos
- Gestión de procesos y multitasking básico
- Parsing avanzado de comandos y scripting

Los ejercicios de este capítulo han establecido una base sólida de manipulación de datos persistentes que será esencial para implementar shells más sofisticados y sistemas de comunicación entre procesos.

### Impacto en el Desarrollo de Software

La comprensión profunda de sistemas de archivos es fundamental para:
- **Desarrollo de aplicaciones**: Entender limitaciones y optimizaciones
- **Administración de sistemas**: Diagnosticar y resolver problemas
- **Diseño de bases de datos**: Aprovechar características del almacenamiento
- **Desarrollo de sistemas embebidos**: Implementar soluciones eficientes

---

**📚 Lecturas Complementarias:**
- "File System Forensic Analysis" por Brian Carrier
- "Understanding the Linux Kernel" - Capítulo de VFS
- Microsoft FAT16 File System Specification
- "Modern Operating Systems" - Capítulo de File Systems