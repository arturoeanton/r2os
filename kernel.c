// kernel.c: núcleo en C con filesystem FAT16 y shell educativo

#include <stdint.h>
#include <stdarg.h>

// =============================================================================
// IMPLEMENTACIÓN BÁSICA DE FUNCIONES DE LIBRERÍA ESTÁNDAR
// =============================================================================
// Como estamos en modo freestanding (sin biblioteca C estándar), 
// debemos implementar nuestras propias funciones básicas de manipulación 
// de memoria y cadenas.
// Copia n bytes desde src hacia dest
// Es fundamental en cualquier sistema operativo para mover datos en memoria
static inline void *memcpy(void *dest, const void *src, unsigned int n) {
    unsigned char *d = dest;
    const unsigned char *s = src;
    for (unsigned int i = 0; i < n; i++) d[i] = s[i];
    return dest;
}
static inline void *memset(void *s, int c, unsigned int n) {
    unsigned char *p = s;
    for (unsigned int i = 0; i < n; i++) p[i] = (unsigned char)c;
    return s;
}
static inline int memcmp(const void *a, const void *b, unsigned int n) {
    const unsigned char *p = a, *q = b;
    for (unsigned int i = 0; i < n; i++) {
        if (p[i] != q[i]) return p[i] - q[i];
    }
    return 0;
}
static inline unsigned int strlen(const char *s) {
    unsigned int len = 0;
    while (s[len]) len++;
    return len;
}
static inline char *strchr(const char *s, int c) {
    for (; *s; s++) if (*s == (char)c) return (char *)s;
    return 0;
}
static inline int strcmp(const char *a, const char *b) {
    while (*a && (*a == *b)) { a++; b++; }
    return (unsigned char)*a - (unsigned char)*b;
}
static inline char *strstr(const char *haystack, const char *needle) {
    if (!*needle) return (char*)haystack;
    while (*haystack) {
        const char *h = haystack;
        const char *n = needle;
        while (*h && *n && (*h == *n)) { h++; n++; }
        if (!*n) return (char*)haystack;
        haystack++;
    }
    return (char*)0;
}
static inline int strncmp(const char *s1, const char *s2, unsigned int n) {
    for (unsigned int i = 0; i < n; i++) {
        if (s1[i] != s2[i]) return (unsigned char)s1[i] - (unsigned char)s2[i];
        if (s1[i] == '\0') return 0;
    }
    return 0;
}

// =============================================================================
// ENTRADA/SALIDA DE PUERTOS (I/O PORTS)
// =============================================================================
// En x86, los dispositivos se comunican a través de puertos I/O.
// outb(): envía un byte a un puerto específico
// inb(): lee un byte de un puerto específico
static inline void outb(uint16_t port, uint8_t val) {
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// =============================================================================
// FUNCIONES AUXILIARES DE CADENAS
// =============================================================================
// Definir NULL
#define NULL ((void*)0)

// Implementación básica de strtok
static char *strtok(char *str, const char *delim) {
    static char *last_token = NULL;
    char *token_start;
    
    // Si str es NULL, continuar con el último token
    if (str == NULL) {
        str = last_token;
    }
    
    if (str == NULL) {
        return NULL;
    }
    
    // Saltar delimitadores al inicio
    while (*str && strchr(delim, *str)) {
        str++;
    }
    
    if (*str == '\0') {
        last_token = NULL;
        return NULL;
    }
    
    token_start = str;
    
    // Encontrar el final del token
    while (*str && !strchr(delim, *str)) {
        str++;
    }
    
    if (*str) {
        *str = '\0';
        last_token = str + 1;
    } else {
        last_token = NULL;
    }
    
    return token_start;
}

// Convierte una cadena a entero (implementación básica de atoi)
static int atoi(const char *str) {
    if (!str) return 0;  // Protección contra NULL
    
    int result = 0;
    int sign = 1;
    
    // Saltar espacios en blanco
    while (*str == ' ' || *str == '\t') str++;
    
    // Verificar que hay algo después de los espacios
    if (*str == '\0') return 0;
    
    // Manejar signo
    if (*str == '-') {
        sign = -1;
        str++;
    } else if (*str == '+') {
        str++;
    }
    
    // Convertir dígitos
    int found_digit = 0;
    while (*str >= '0' && *str <= '9') {
        result = result * 10 + (*str - '0');
        str++;
        found_digit = 1;
    }
    
    // Si no encontramos ningún dígito, devolver 0
    if (!found_digit) return 0;
    
    return sign * result;
}

// =============================================================================
// CONTROLADOR DE TECLADO PS/2
// =============================================================================
// El teclado PS/2 envía 'scancodes' cuando se presiona una tecla.
// Necesitamos convertir estos códigos a caracteres ASCII utilizables.
// Puerto 0x60: datos del teclado, Puerto 0x64: estado del controlador
static const char scancode_map[128] = {
    // Números y símbolos de la fila superior
    [0x02] = '1', [0x03] = '2', [0x04] = '3', [0x05] = '4',
    [0x06] = '5', [0x07] = '6', [0x08] = '7', [0x09] = '8',
    [0x0A] = '9', [0x0B] = '0',
    [0x0C] = '-', [0x0D] = '=',  // Guión y igual
    
    // Primera fila de letras
    [0x10] = 'q', [0x11] = 'w', [0x12] = 'e', [0x13] = 'r',
    [0x14] = 't', [0x15] = 'y', [0x16] = 'u', [0x17] = 'i',
    [0x18] = 'o', [0x19] = 'p',
    [0x1A] = '[', [0x1B] = ']',  // Corchetes
    
    // Segunda fila de letras
    [0x1E] = 'a', [0x1F] = 's', [0x20] = 'd', [0x21] = 'f',
    [0x22] = 'g', [0x23] = 'h', [0x24] = 'j', [0x25] = 'k',
    [0x26] = 'l',
    [0x27] = ';', [0x28] = '\'', // Punto y coma, comilla simple
    
    // Tercera fila de letras
    [0x2C] = 'z', [0x2D] = 'x', [0x2E] = 'c', [0x2F] = 'v',
    [0x30] = 'b', [0x31] = 'n', [0x32] = 'm',
    [0x33] = ',', [0x34] = '.', [0x35] = '/',  // Coma, punto, barra
    
    // Teclas adicionales
    [0x29] = '`', [0x56] = '\\',  // Tilde y backslash
    [0x2B] = '\\', // Backslash alternativo en algunos layouts
    
    // Teclas especiales
    [0x39] = ' ',   // Espacio
    [0x1C] = '\n',  // Enter
    [0x0E] = '\b'   // Backspace
};
// Códigos especiales para teclas de navegación
#define KEY_UP    0x80
#define KEY_DOWN  0x81  
#define KEY_LEFT  0x82
#define KEY_RIGHT 0x83
#define KEY_DELETE 0x84

// Obtiene un carácter del teclado PS/2
// Espera hasta que haya una tecla disponible y la convierte a ASCII
// Maneja teclas especiales como backspace, delete, flechas y secuencias Ctrl
static unsigned char keyboard_getchar(void) {
    static int ctrl_pressed = 0;  // Estado de la tecla Ctrl
    static int shift_pressed = 0; // Estado de la tecla Shift
    static int extended = 0;      // Estado para secuencias extendidas (0xE0)
    
    while (1) {
        // Verificar si hay datos disponibles en el buffer del teclado
        if (inb(0x64) & 1) {  // Bit 0 del puerto 0x64 indica datos listos
            uint8_t code = inb(0x60);  // Leer el scancode del puerto 0x60
            
            // Manejar secuencias extendidas (teclas especiales como flechas)
            if (code == 0xE0) {
                extended = 1;
                continue;
            }
            
            // Manejar teclas de control especiales
            if (code == 0x1D) {  // Ctrl presionado
                ctrl_pressed = 1;
                continue;
            } else if (code == 0x9D) {  // Ctrl liberado
                ctrl_pressed = 0;
                continue;
            } else if (code == 0x2A || code == 0x36) {  // Shift izquierdo/derecho presionado
                shift_pressed = 1;
                continue;
            } else if (code == 0xAA || code == 0xB6) {  // Shift izquierdo/derecho liberado
                shift_pressed = 0;
                continue;
            }
            
            // Si el bit 7 está en 0, es una tecla presionada (no liberada)
            if (!(code & 0x80)) {
                // Manejar teclas extendidas (flechas, delete, etc.)
                if (extended) {
                    extended = 0;
                    switch (code) {
                        case 0x48: return KEY_UP;     // Flecha arriba
                        case 0x50: return KEY_DOWN;   // Flecha abajo  
                        case 0x4B: return KEY_LEFT;   // Flecha izquierda
                        case 0x4D: return KEY_RIGHT;  // Flecha derecha
                        case 0x53: return KEY_DELETE; // Delete
                        default: continue;  // Ignorar otras teclas extendidas
                    }
                }
                
                // Manejar backspace (scancode 0x0E) y Delete en Mac (puede usar 0x0E también)
                if (code == 0x0E) {
                    return 0x08;  // ASCII backspace
                }
                
                char c = scancode_map[code];  // Convertir scancode a ASCII
                if (c) {
                    // Debug deshabilitado temporalmente para compilación
                    
                    // Si Ctrl está presionado, generar código de control
                    if (ctrl_pressed && c >= 'a' && c <= 'z') {
                        return c - 'a' + 1;  // Ctrl+A = 0x01, Ctrl+B = 0x02, etc.
                    }
                    if (ctrl_pressed && c >= 'A' && c <= 'Z') {
                        return c - 'A' + 1;  // Ctrl+A = 0x01, Ctrl+B = 0x02, etc.
                    }
                    
                    // Si Shift está presionado, convertir a mayúsculas o símbolos
                    if (shift_pressed) {
                        if (c >= 'a' && c <= 'z') {
                            return c - 'a' + 'A';  // Convertir a mayúscula
                        } else {
                            // Manejo de símbolos con Shift
                            switch (c) {
                                case '1': return '!';
                                case '2': return '@';
                                case '3': return '#';
                                case '4': return '$';
                                case '5': return '%';
                                case '6': return '^';
                                case '7': return '&';
                                case '8': return '*';
                                case '9': return '(';
                                case '0': return ')';
                                case '-': return '_';
                                case '=': return '+';
                                case '[': return '{';
                                case ']': return '}';
                                case ';': return ':';
                                case '\'': return '"';
                                case ',': return '<';
                                case '.': return '>';
                                case '/': return '?';
                                case '\\': return '|';  // Backslash + Shift = Pipe
                                case '`': return '~';
                                default: return c;
                            }
                        }
                    }
                    
                    return c;
                }
            } else {
                // Tecla liberada - resetear estado extendido si es necesario
                extended = 0;
            }
        }
    }
}
#define getchar_stub keyboard_getchar

// =============================================================================
// CONTROLADOR DE PANTALLA VGA EN MODO TEXTO
// =============================================================================
// La memoria VGA en modo texto está en 0xB8000.
// Cada carácter ocupa 2 bytes: [carácter][atributo]
// El atributo 0x07 = texto blanco sobre fondo negro
// La pantalla es de 80 columnas × 25 filas = 2000 caracteres
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
static uint16_t *const VGA_BUFFER = (uint16_t *)0xB8000;
static uint8_t cursor_x = 0, cursor_y = 0;
static void update_cursor(void) {
    uint16_t pos = cursor_y * VGA_WIDTH + cursor_x;
    outb(0x3D4, 0x0E);
    outb(0x3D5, pos >> 8);
    outb(0x3D4, 0x0F);
    outb(0x3D5, pos);
}
// Función para hacer scroll hacia arriba cuando se llena la pantalla
static void scroll_up(void) {
    // Mover todas las líneas una posición hacia arriba
    for (int y = 0; y < VGA_HEIGHT - 1; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            VGA_BUFFER[y * VGA_WIDTH + x] = VGA_BUFFER[(y + 1) * VGA_WIDTH + x];
        }
    }
    // Limpiar la última línea
    for (int x = 0; x < VGA_WIDTH; x++) {
        VGA_BUFFER[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = (uint16_t)' ' | 0x0700;
    }
    cursor_y = VGA_HEIGHT - 1;  // Posicionar en la última línea
}

// Imprime un carácter en la pantalla VGA
// Maneja saltos de línea y el desbordamiento de pantalla con scroll
static void putchar(char c) {
    if (c == '\n') { 
        cursor_x = 0; cursor_y++;  // Nueva línea
    } else if (c == '\r') {
        cursor_x = 0;  // Retorno de carro sin nueva línea
    } else if (c == '\b') {
        // Backspace: mover cursor hacia atrás si es posible
        if (cursor_x > 0) {
            cursor_x--;
        }
    } else {
        // Escribir carácter + atributo (0x0700 = blanco sobre negro)
        VGA_BUFFER[cursor_y * VGA_WIDTH + cursor_x] = (uint16_t)c | 0x0700;
        cursor_x++;
        // Si llegamos al final de la línea, pasar a la siguiente
        if (cursor_x >= VGA_WIDTH) { cursor_x = 0; cursor_y++; }
    }
    // Si llegamos al final de la pantalla, hacer scroll hacia arriba
    if (cursor_y >= VGA_HEIGHT) {
        scroll_up();
    }
    update_cursor();
}
static void prints(const char *s) { while (*s) putchar(*s++); }
static void printnum(unsigned int n, int base) {
    char buf[32]; int i = 0;
    if (!n) { putchar('0'); return; }
    while (n) { int d = n % base; buf[i++] = (d < 10 ? '0'+d : 'a'+d-10); n /= base; }
    while (i--) putchar(buf[i]);
}
void printf(const char *fmt, ...) {
    va_list args; va_start(args, fmt);
    for (const char *p = fmt; *p; p++) {
        if (*p != '%') { putchar(*p); continue; }
        p++;
        switch (*p) {
            case 's': prints(va_arg(args, char*)); break;
            case 'c': putchar((char)va_arg(args,int)); break;
            case 'd': printnum(va_arg(args,int),10); break;
            case 'u': printnum(va_arg(args,unsigned),10); break;  // Soporte para %u
            case 'x': printnum(va_arg(args,unsigned),16); break;
            case '%': putchar('%'); break;
            default: putchar('?'); break;
        }
    }
    va_end(args);
}

// =============================================================================
// SISTEMA DE ARCHIVOS FAT16 SIMPLIFICADO
// =============================================================================
// FAT16 es un sistema de archivos simple usado en discos pequeños.
// Estructura: [Boot Sector][FAT][Root Directory][Data Area]
// - Boot Sector: 1 sector con información del sistema de archivos
// - FAT: tabla que indica qué clusters están ocupados (256 sectores)
// - Root Directory: entradas de archivos en el directorio raíz (512 entradas)
// - Data Area: contenido real de los archivos
#define SECTOR_SIZE    512
#define ROOT_ENTRIES   512
#define FAT_SECTORS    256
#define TOTAL_SECTORS  289
#define DATA_START     ((1 + FAT_SECTORS) * SECTOR_SIZE + ROOT_ENTRIES * sizeof(fat16_dir_entry))

typedef struct __attribute__((packed)) {
    char     name[11];
    uint8_t  attr;
    uint8_t  reserved[10];
    uint16_t first_cluster;
    uint32_t size;
} fat16_dir_entry;

// Referencia al área de disco definida en boot.s
extern uint8_t disk_image_start[];
static uint8_t *disk_image = disk_image_start;

// =============================================================================
// SIMULACIÓN DE DISCO EN MEMORIA RAM
// =============================================================================
// En lugar de acceder a un disco real, simulamos todo en memoria.
// Esto simplifica enormemente el desarrollo pero mantiene la lógica intacta.
static void read_sector(uint32_t lba, void *buf) {
    memcpy(buf, disk_image + lba * SECTOR_SIZE, SECTOR_SIZE);
}
static void write_sector(uint32_t lba, const void *buf) {
    memcpy(disk_image + lba * SECTOR_SIZE, buf, SECTOR_SIZE);
}

// =============================================================================
// MANIPULACIÓN DE ENTRADAS DEL DIRECTORIO RAÍZ
// =============================================================================
// Cada archivo tiene una entrada de 32 bytes en el directorio raíz.
// Contiene: nombre (11 bytes), atributos, cluster inicial, tamaño, etc.
static void read_root_entry(int idx, fat16_dir_entry *e) {
    uint32_t base = 1 + FAT_SECTORS;
    uint32_t off = idx * sizeof(*e);
    uint8_t  sec[SECTOR_SIZE];
    read_sector(base + off / SECTOR_SIZE, sec);
    memcpy(e, sec + (off % SECTOR_SIZE), sizeof(*e));
}
static void write_root_entry(int idx, const fat16_dir_entry *e) {
    uint32_t base = 1 + FAT_SECTORS;
    uint32_t off = idx * sizeof(*e);
    uint8_t  sec[SECTOR_SIZE];
    read_sector(base + off / SECTOR_SIZE, sec);
    memcpy(sec + (off % SECTOR_SIZE), e, sizeof(*e));
    write_sector(base + off / SECTOR_SIZE, sec);
}

// =============================================================================
// INICIALIZACIÓN DEL SISTEMA DE ARCHIVOS
// =============================================================================
// Inicializa las estructuras básicas del sistema de archivos FAT16 en memoria
static void fs_init(void) {
    // Limpiar toda la imagen de disco
    memset(disk_image, 0, TOTAL_SECTORS * SECTOR_SIZE);
    
    // Inicializar el sector de arranque (boot sector)
    // Por simplicidad, solo ponemos algunos valores básicos
    uint8_t boot_sector[SECTOR_SIZE];
    memset(boot_sector, 0, SECTOR_SIZE);
    
    // Signature del boot sector
    boot_sector[510] = 0x55;
    boot_sector[511] = 0xAA;
    
    write_sector(0, boot_sector);
    
    // Inicializar la tabla FAT
    // Marcar los primeros clusters como reservados
    uint8_t fat_sector[SECTOR_SIZE];
    memset(fat_sector, 0, SECTOR_SIZE);
    
    // FAT16: cada entrada es de 16 bits (2 bytes)
    // Cluster 0: reservado (0xFFF8)
    // Cluster 1: reservado (0xFFFF) 
    fat_sector[0] = 0xF8; fat_sector[1] = 0xFF;  // Cluster 0
    fat_sector[2] = 0xFF; fat_sector[3] = 0xFF;  // Cluster 1
    
    write_sector(1, fat_sector);  // Escribir el primer sector de la FAT
    
    // Limpiar el resto de sectores FAT
    memset(fat_sector, 0, SECTOR_SIZE);
    for (int i = 2; i <= FAT_SECTORS; i++) {
        write_sector(i, fat_sector);
    }
    
    // Inicializar el directorio raíz (limpiar todas las entradas)
    uint8_t root_sector[SECTOR_SIZE];
    memset(root_sector, 0, SECTOR_SIZE);
    
    // El directorio raíz empieza después de la FAT
    uint32_t root_start = 1 + FAT_SECTORS;
    uint32_t root_sectors = (ROOT_ENTRIES * sizeof(fat16_dir_entry) + SECTOR_SIZE - 1) / SECTOR_SIZE;
    
    for (uint32_t i = 0; i < root_sectors; i++) {
        write_sector(root_start + i, root_sector);
    }
    
    printf("Sistema de archivos inicializado.\n");
}

// =============================================================================
// GESTIÓN DE CLUSTERS
// =============================================================================
// Encuentra el próximo cluster libre y lo marca como ocupado
static uint16_t fs_alloc_cluster(void) {
    uint8_t fat_sector[SECTOR_SIZE];
    
    // Empezar desde el cluster 2 (los primeros dos están reservados)
    for (uint16_t cluster = 2; cluster < 1000; cluster++) {
        uint32_t fat_offset = cluster * 2;  // FAT16: 2 bytes por entrada
        uint32_t fat_sector_num = 1 + (fat_offset / SECTOR_SIZE);
        uint32_t fat_entry_offset = fat_offset % SECTOR_SIZE;
        
        read_sector(fat_sector_num, fat_sector);
        
        // Leer entrada FAT (16 bits)
        uint16_t fat_entry = fat_sector[fat_entry_offset] | (fat_sector[fat_entry_offset + 1] << 8);
        
        if (fat_entry == 0) {  // Cluster libre
            // Marcarlo como fin de cadena (0xFFFF)
            fat_sector[fat_entry_offset] = 0xFF;
            fat_sector[fat_entry_offset + 1] = 0xFF;
            write_sector(fat_sector_num, fat_sector);
            return cluster;
        }
    }
    
    return 0;  // No hay clusters libres
}

// =============================================================================
// OPERACIONES BÁSICAS DEL SISTEMA DE ARCHIVOS
// =============================================================================
// Estas funciones implementan las operaciones esenciales que esperamos
// de cualquier sistema de archivos: buscar, listar, crear, leer, escribir, etc.
// Busca un archivo por nombre en el directorio raíz
// Los nombres en FAT16 son de exactamente 11 caracteres (8.3 format)
// Retorna 1 si lo encuentra, 0 si no existe
static int fs_find(const char *name, fat16_dir_entry *e, int *idx) {
    char buf[11]; 
    memset(buf, ' ', 11);  // Llenar con espacios (formato FAT16)
    // Copiar el nombre, máximo 11 caracteres
    for (int i = 0; i < 11 && name[i]; i++) buf[i] = name[i];
    
    // Buscar en todas las entradas del directorio raíz
    for (int i = 0; i < ROOT_ENTRIES; i++) {
        read_root_entry(i, e);
        if (!memcmp(e->name, buf, 11)) { 
            *idx = i;  // Guardar el índice donde se encontró
            return 1;  // Encontrado
        }
    }
    return 0;  // No encontrado
}
static void	fs_ls(void) {
    fat16_dir_entry e; char fname[12];
    for (int i = 0; i < ROOT_ENTRIES; i++) {
        read_root_entry(i, &e);
        if (e.name[0] == 0x00) break;
        if ((uint8_t)e.name[0] == 0xE5) continue;
        memcpy(fname, e.name, 11);
        fname[11] = '\0';
        printf("%s  %u bytes\n", fname, e.size);
    }
}
static void fs_touch(const char *name) {
    fat16_dir_entry e; char buf[11]; memset(buf, ' ', 11);
    for (int i = 0; i < 11 && name[i]; i++) buf[i] = name[i];
    
    // Verificar si el archivo ya existe
    int idx;
    if (fs_find(name, &e, &idx)) {
        printf("El archivo ya existe: %s\n", name);
        return;
    }
    
    // Encontrar una entrada libre en el directorio raíz
    for (int i = 0; i < ROOT_ENTRIES; i++) {
        read_root_entry(i, &e);
        if (e.name[0] == 0x00 || (uint8_t)e.name[0] == 0xE5) {
            // Asignar un cluster libre
            uint16_t cluster = fs_alloc_cluster();
            if (cluster == 0) {
                printf("Error: No hay espacio disponible\n");
                return;
            }
            
            memset(&e, 0, sizeof(e));
            memcpy(e.name, buf, 11);
            e.first_cluster = cluster;
            e.size = 0;
            write_root_entry(i, &e);
            
            // Limpiar el sector de datos del cluster
            uint8_t zero[SECTOR_SIZE]; 
            memset(zero, 0, SECTOR_SIZE);
            uint32_t data_sector = DATA_START / SECTOR_SIZE + (cluster - 2);
            write_sector(data_sector, zero);
            
            printf("Archivo creado: %s\n", name);
            return;
        }
    }
    printf("Error: Directorio raíz lleno\n");
}
static int fs_read(const char *name, void *buf, uint32_t *size) {
    fat16_dir_entry e; int idx;
    if (!fs_find(name, &e, &idx)) return 0;
    read_sector(DATA_START / SECTOR_SIZE + (e.first_cluster - 2), buf);
    *size = e.size;
    return 1;
}
// Crear archivo sin verificar si existe (para uso interno)
static int fs_create_internal(const char *name) {
    fat16_dir_entry e; 
    char buf[11]; 
    memset(buf, ' ', 11);
    for (int i = 0; i < 11 && name[i]; i++) buf[i] = name[i];
    
    // Encontrar una entrada libre en el directorio raíz
    for (int i = 0; i < ROOT_ENTRIES; i++) {
        read_root_entry(i, &e);
        if (e.name[0] == 0x00 || (uint8_t)e.name[0] == 0xE5) {
            // Asignar un cluster libre
            uint16_t cluster = fs_alloc_cluster();
            if (cluster == 0) {
                return 0;  // No hay espacio
            }
            
            memset(&e, 0, sizeof(e));
            memcpy(e.name, buf, 11);
            e.first_cluster = cluster;
            e.size = 0;
            write_root_entry(i, &e);
            
            // Limpiar el sector de datos del cluster
            uint8_t zero[SECTOR_SIZE]; 
            memset(zero, 0, SECTOR_SIZE);
            uint32_t data_sector = DATA_START / SECTOR_SIZE + (cluster - 2);
            write_sector(data_sector, zero);
            
            return 1;  // Éxito
        }
    }
    return 0;  // Directorio lleno
}

static void fs_write(const char *name, const void *buf, uint32_t size) {
    fat16_dir_entry e; int idx;
    if (!fs_find(name, &e, &idx)) {
        // Si el archivo no existe, crearlo primero
        if (!fs_create_internal(name)) {
            printf("Error: No se pudo crear el archivo %s\n", name);
            return;
        }
        if (!fs_find(name, &e, &idx)) {
            printf("Error: Archivo creado pero no encontrado\n");
            return;
        }
    }
    
    // Escribir los datos al sector correspondiente
    uint32_t data_sector = DATA_START / SECTOR_SIZE + (e.first_cluster - 2);
    write_sector(data_sector, buf);
    
    // Actualizar el tamaño del archivo en la entrada del directorio
    e.size = size;
    write_root_entry(idx, &e);
}
// Renombrar un archivo en el sistema de archivos
// Busca el archivo con el nombre antiguo y le cambia el nombre al nuevo
static void fs_mv(const char *old, const char *new) {
    fat16_dir_entry e; int idx;
    if (!fs_find(old, &e, &idx)) return;
    memset(e.name, ' ', 11);  // Limpiar el campo del nombre
    memcpy(e.name, new, strlen(new));  // Copiar el nuevo nombre
    write_root_entry(idx, &e);  // Escribir la entrada modificada
}

// Eliminar un archivo del sistema de archivos
// Marca la entrada del directorio como eliminada usando el código 0xE5
static void fs_delete(const char *name) {
    fat16_dir_entry e; int idx;
    if (!fs_find(name, &e, &idx)) {
        printf("Archivo no encontrado: %s\n", name);
        return;
    }
    // Marcar como eliminado con el código especial 0xE5
    e.name[0] = 0xE5;
    write_root_entry(idx, &e);
    printf("Archivo eliminado: %s\n", name);
}

// Editar una línea específica de un archivo
// Si el archivo no existe, lo crea. Si la línea no existe, extiende el archivo
static void fs_edit_line(const char *name, int line_num, const char *new_text) {
    uint8_t buffer[SECTOR_SIZE];
    uint32_t file_size = 0;
    
    // Intentar leer el archivo existente
    int file_exists = fs_read(name, buffer, &file_size);
    if (!file_exists) {
        // Si no existe, crear archivo vacío
        memset(buffer, 0, SECTOR_SIZE);
        file_size = 0;
        fs_touch(name);  // Crear entrada de directorio
    }
    
    // Crear un nuevo buffer para el archivo modificado
    uint8_t new_buffer[SECTOR_SIZE];
    memset(new_buffer, 0, SECTOR_SIZE);
    
    int current_line = 1;
    uint32_t src_pos = 0;
    uint32_t dst_pos = 0;
    int line_replaced = 0;
    
    // Procesar el archivo línea por línea
    while (src_pos < file_size && dst_pos < SECTOR_SIZE - 1) {
        if (current_line == line_num && !line_replaced) {
            // Reemplazar con el nuevo texto
            int text_len = strlen(new_text);
            if (dst_pos + text_len < SECTOR_SIZE - 1) {
                memcpy(new_buffer + dst_pos, new_text, text_len);
                dst_pos += text_len;
                new_buffer[dst_pos++] = '\n';
            }
            line_replaced = 1;
            
            // Saltar la línea original hasta el siguiente \n
            while (src_pos < file_size && buffer[src_pos] != '\n') {
                src_pos++;
            }
            if (src_pos < file_size && buffer[src_pos] == '\n') {
                src_pos++; // Saltar el \n
            }
            current_line++;
        } else {
            // Copiar línea existente
            if (buffer[src_pos] == '\n') {
                current_line++;
            }
            new_buffer[dst_pos++] = buffer[src_pos++];
        }
    }
    
    // Si la línea no existía, añadirla al final
    if (!line_replaced) {
        // Asegurar que hay líneas vacías hasta llegar a la línea deseada
        while (current_line < line_num && dst_pos < SECTOR_SIZE - 2) {
            new_buffer[dst_pos++] = '\n';
            current_line++;
        }
        
        // Añadir el nuevo texto
        int text_len = strlen(new_text);
        if (dst_pos + text_len < SECTOR_SIZE - 1) {
            memcpy(new_buffer + dst_pos, new_text, text_len);
            dst_pos += text_len;
            new_buffer[dst_pos++] = '\n';
        }
    }
    
    // Escribir el archivo modificado
    fs_write(name, new_buffer, dst_pos);
    printf("Linea %d editada en archivo: %s\n", line_num, name);
}

// Eliminar una línea específica de un archivo
static void fs_delete_line(const char *name, int line_num) {
    uint8_t buffer[SECTOR_SIZE];
    uint32_t file_size = 0;
    
    // Leer el archivo existente
    if (!fs_read(name, buffer, &file_size)) {
        printf("Archivo no encontrado: %s\n", name);
        return;
    }
    
    // Crear un nuevo buffer para el archivo sin la línea eliminada
    uint8_t new_buffer[SECTOR_SIZE];
    memset(new_buffer, 0, SECTOR_SIZE);
    
    int current_line = 1;
    uint32_t src_pos = 0;
    uint32_t dst_pos = 0;
    int line_found = 0;
    
    // Procesar el archivo línea por línea
    while (src_pos < file_size && dst_pos < SECTOR_SIZE - 1) {
        if (current_line == line_num) {
            // Saltar esta línea (no copiarla)
            line_found = 1;
            while (src_pos < file_size && buffer[src_pos] != '\n') {
                src_pos++;
            }
            if (src_pos < file_size && buffer[src_pos] == '\n') {
                src_pos++; // Saltar el \n también
            }
            current_line++;
        } else {
            // Copiar línea normal
            if (buffer[src_pos] == '\n') {
                current_line++;
            }
            new_buffer[dst_pos++] = buffer[src_pos++];
        }
    }
    
    if (!line_found) {
        printf("Linea %d no existe en el archivo %s\n", line_num, name);
        return;
    }
    
    // Escribir el archivo modificado
    fs_write(name, new_buffer, dst_pos);
    printf("Linea %d eliminada del archivo: %s\n", line_num, name);
}

// Insertar una línea en blanco en una posición específica
static void fs_insert_line(const char *name, int line_num) {
    uint8_t buffer[SECTOR_SIZE];
    uint32_t file_size = 0;
    
    // Intentar leer el archivo existente
    int file_exists = fs_read(name, buffer, &file_size);
    if (!file_exists) {
        // Si no existe, crear archivo nuevo con líneas vacías hasta la posición
        memset(buffer, 0, SECTOR_SIZE);
        file_size = 0;
        fs_touch(name);  // Crear entrada de directorio
    }
    
    // Crear un nuevo buffer para el archivo con la línea insertada
    uint8_t new_buffer[SECTOR_SIZE];
    memset(new_buffer, 0, SECTOR_SIZE);
    
    int current_line = 1;
    uint32_t src_pos = 0;
    uint32_t dst_pos = 0;
    int line_inserted = 0;
    
    // Procesar el archivo línea por línea
    while (src_pos < file_size && dst_pos < SECTOR_SIZE - 2) {
        if (current_line == line_num && !line_inserted) {
            // Insertar línea en blanco
            new_buffer[dst_pos++] = '\n';
            line_inserted = 1;
            current_line++;
            // No incrementar src_pos, continuar con la línea actual
        } else {
            // Copiar línea existente
            if (buffer[src_pos] == '\n') {
                current_line++;
            }
            new_buffer[dst_pos++] = buffer[src_pos++];
        }
    }
    
    // Si la línea está más allá del final del archivo, añadir líneas vacías
    if (!line_inserted) {
        while (current_line < line_num && dst_pos < SECTOR_SIZE - 2) {
            new_buffer[dst_pos++] = '\n';
            current_line++;
        }
        // Insertar la línea en blanco final
        new_buffer[dst_pos++] = '\n';
    }
    
    // Escribir el archivo modificado
    fs_write(name, new_buffer, dst_pos);
    printf("Linea en blanco insertada en posicion %d del archivo: %s\n", line_num, name);
}

// Mostrar contenido de archivo en hexadecimal
static void fs_hexdump(const char *name) {
    uint8_t buffer[SECTOR_SIZE];
    uint32_t file_size = 0;
    
    if (!fs_read(name, buffer, &file_size)) {
        printf("Archivo no encontrado: %s\n", name);
        return;
    }
    
    printf("=== HEXDUMP de %s (%u bytes) ===\n", name, file_size);
    for (uint32_t i = 0; i < file_size; i += 16) {
        printf("%04x: ", i);
        // Mostrar hex
        for (int j = 0; j < 16; j++) {
            if (i + j < file_size) {
                printf("%02x ", buffer[i + j]);
            } else {
                printf("   ");
            }
        }
        printf(" ");
        // Mostrar ASCII
        for (int j = 0; j < 16 && i + j < file_size; j++) {
            char c = buffer[i + j];
            printf("%c", (c >= 32 && c < 127) ? c : '.');
        }
        printf("\n");
    }
}

// Contar líneas, palabras y caracteres
static void fs_wc(const char *name) {
    uint8_t buffer[SECTOR_SIZE];
    uint32_t file_size = 0;
    
    if (!fs_read(name, buffer, &file_size)) {
        printf("Archivo no encontrado: %s\n", name);
        return;
    }
    
    int lines = 0, words = 0, chars = file_size;
    int in_word = 0;
    
    for (uint32_t i = 0; i < file_size; i++) {
        char c = buffer[i];
        if (c == '\n') lines++;
        if (c == ' ' || c == '\t' || c == '\n') {
            if (in_word) {
                words++;
                in_word = 0;
            }
        } else {
            in_word = 1;
        }
    }
    if (in_word) words++; // Última palabra sin \n
    
    printf("%d %d %d %s\n", lines, words, chars, name);
}


// Buscar texto en archivo
static void fs_grep(const char *pattern, const char *name) {
    uint8_t buffer[SECTOR_SIZE];
    uint32_t file_size = 0;
    
    if (!fs_read(name, buffer, &file_size)) {
        printf("Archivo no encontrado: %s\n", name);
        return;
    }
    
    int line_num = 1;
    int line_start = 0;
    int matches = 0;
    
    for (uint32_t i = 0; i < file_size; i++) {
        if (buffer[i] == '\n') {
            // Procesar línea
            buffer[i] = '\0';  // Terminar temporalmente
            char *line = (char *)&buffer[line_start];
            
            // Buscar patrón simple (sin regex)
            if (strstr(line, pattern)) {
                printf("%d: %s\n", line_num, line);
                matches++;
            }
            
            buffer[i] = '\n';  // Restaurar
            line_start = i + 1;
            line_num++;
        }
    }
    
    if (matches == 0) {
        printf("Patron '%s' no encontrado en %s\n", pattern, name);
    }
}

// Mostrar primeras líneas de archivo
static void fs_head(const char *name, int lines) {
    uint8_t buffer[SECTOR_SIZE];
    uint32_t file_size = 0;
    
    if (!fs_read(name, buffer, &file_size)) {
        printf("Archivo no encontrado: %s\n", name);
        return;
    }
    
    int current_line = 1;
    for (uint32_t i = 0; i < file_size && current_line <= lines; i++) {
        putchar(buffer[i]);
        if (buffer[i] == '\n') current_line++;
    }
}

// Mostrar últimas líneas de archivo
static void fs_tail(const char *name, int lines) {
    uint8_t buffer[SECTOR_SIZE];
    uint32_t file_size = 0;
    
    if (!fs_read(name, buffer, &file_size)) {
        printf("Archivo no encontrado: %s\n", name);
        return;
    }
    
    // Contar líneas totales
    int total_lines = 0;
    for (uint32_t i = 0; i < file_size; i++) {
        if (buffer[i] == '\n') total_lines++;
    }
    
    // Mostrar desde la línea (total_lines - lines + 1)
    int start_line = (total_lines > lines) ? total_lines - lines + 1 : 1;
    int current_line = 1;
    
    for (uint32_t i = 0; i < file_size; i++) {
        if (current_line >= start_line) {
            putchar(buffer[i]);
        }
        if (buffer[i] == '\n') current_line++;
    }
}

// Crear directorio simulado (como archivo especial)
static void fs_mkdir(const char *name) {
    char dirname[16];
    // Agregar prefijo para directorios
    dirname[0] = '[';
    for (int i = 0; i < 10 && name[i]; i++) {
        dirname[i + 1] = name[i];
    }
    dirname[strlen(name) + 1] = ']';
    dirname[strlen(name) + 2] = '\0';
    
    fs_touch(dirname);
    printf("Directorio simulado creado: %s\n", dirname);
}

// =============================================================================
// FUNCIONES DE PANTALLA
// =============================================================================
// Llena toda la pantalla VGA con espacios y resetea el cursor
static void clear_screen(void) {
    for (int y = 0; y < VGA_HEIGHT; y++)
        for (int x = 0; x < VGA_WIDTH; x++)
            VGA_BUFFER[y * VGA_WIDTH + x] = (uint16_t)' ' | 0x0700;
    cursor_x = cursor_y = 0; update_cursor();
}

// Función para mostrar ayuda con pausas
static void show_help_with_pause(void) {
    prints("=== MINI-KERNEL EDUCATIVO - COMANDOS DISPONIBLES ===\n");
    prints("=== ARCHIVOS Y DIRECTORIOS ===\n");
    prints("ls              - Listar archivos del directorio raiz\n");
    prints("cat <file>      - Mostrar contenido de un archivo\n");
    prints("touch <file>    - Crear archivo vacio\n");
    prints("cp <src> <dst>  - Copiar archivo\n");
    prints("mv <old> <new>  - Renombrar archivo\n");
    prints("delete <file>   - Eliminar archivo\n");
    prints("mkdir <dir>     - Crear directorio simulado\n");
    prints("pwd             - Mostrar directorio actual\n");
    
    prints("\n--- Presiona cualquier tecla para continuar ---");
    getchar_stub();
    
    prints("\n=== EDICION DE TEXTO ===\n");
    prints("copycon <file>  - Crear archivo desde teclado\n");
    prints("edln <file> <num> <texto> - Editar linea especifica\n");
    prints("delln <file> <num>        - Eliminar linea especifica\n");
    prints("insln <file> <num>        - Insertar linea en blanco\n");
    prints("tee <file>      - Escribir a archivo y pantalla\n");
    
    prints("\n--- Presiona cualquier tecla para continuar ---");
    getchar_stub();
    
    prints("\n=== ANALISIS DE ARCHIVOS ===\n");
    prints("wc <file>       - Contar lineas, palabras, caracteres\n");
    prints("head <file> [n] - Mostrar primeras n lineas (def: 10)\n");
    prints("tail <file> [n] - Mostrar ultimas n lineas (def: 10)\n");
    prints("grep <txt> <file> - Buscar texto en archivo\n");
    prints("hexdump <file>  - Mostrar archivo en hexadecimal\n");
    prints("file <file>     - Determinar tipo de archivo\n");
    prints("stat <file>     - Estadisticas de archivo\n");
    prints("du <file>       - Uso de disco del archivo\n");
    
    prints("\n--- Presiona cualquier tecla para continuar ---");
    getchar_stub();
    
    prints("\n=== UTILIDADES DE TEXTO ===\n");
    prints("echo <text>     - Imprimir texto\n");
    prints("rev <text>      - Invertir texto\n");
    prints("yes <text>      - Repetir texto (limitado)\n");
    prints("sort <file>     - Ordenar contenido (simulado)\n");
    prints("which <cmd>     - Encontrar ubicacion de comando\n");
    
    prints("\n--- Presiona cualquier tecla para continuar ---");
    getchar_stub();
    
    prints("\n=== SISTEMA ===\n");
    prints("whoami          - Mostrar usuario actual\n");
    prints("uname           - Informacion del sistema\n");
    prints("date            - Fecha actual\n");
    prints("cal             - Calendario\n");
    prints("uptime          - Tiempo funcionamiento\n");
    prints("free            - Uso de memoria\n");
    prints("history         - Historial de comandos\n");
    prints("man <cmd>       - Manual de comando especifico\n");
    prints("cls/clear       - Limpiar pantalla\n");
    prints("help/?          - Mostrar esta ayuda\n");
    
    prints("\n--- Presiona cualquier tecla para finalizar ---");
    getchar_stub();
    
    prints("\nTeclado: soporta letras, numeros, simbolos (.,;'[]/-=|\\)\n");
    prints("Usa backspace para borrar y Ctrl+combinaciones para funciones especiales.\n");
    prints("=========================================================\n");
}

// Muestra la ayuda del sistema (versión simple para arranque)
static void show_help(void) {
    prints("=== MINI-KERNEL EDUCATIVO ===\n");
    prints("Escribe 'help' para ver todos los comandos disponibles.\n");
    prints("Comandos basicos: ls, cat, touch, cp, mv, delete, echo, clear\n");
    prints("Edicion: copycon, edln, delln, insln, tee\n");
    prints("Sistema: whoami, date, free, history, man\n");
    prints("=========================================================\n");
}

// =============================================================================
// INTÉRPRETE DE COMANDOS (SHELL)
// =============================================================================
// El shell es la interfaz entre el usuario y el sistema operativo.
// Lee comandos del teclado, los interpreta y ejecuta las funciones correspondientes.
#define CMD_BUFSIZE 128
#define HISTORY_SIZE 10
static char cmdbuf[CMD_BUFSIZE];
static char history[HISTORY_SIZE][CMD_BUFSIZE];
static int history_count = 0;

// Agregar comando al historial
static void add_to_history(const char *cmd) {
    if (strlen(cmd) == 0) return;  // No agregar comandos vacíos
    
    // Copiar comando al historial
    int idx = history_count % HISTORY_SIZE;
    memcpy(history[idx], cmd, strlen(cmd) + 1);
    history_count++;
}

// Función para redibujar la línea de comando
static void redraw_line(const char *line, int cursor_pos) {
    // Ir al inicio de la línea
    putchar('\r');
    printf("shell> %s", line);
    
    // Limpiar caracteres extra agregando espacios
    for (int i = 0; i < 20; i++) putchar(' ');
    
    // Volver al inicio y posicionar cursor
    putchar('\r');
    printf("shell> ");
    for (int i = 0; i < cursor_pos; i++) {
        putchar(line[i]);
    }
}

// =============================================================================
// PIPES SIMPLES
// =============================================================================
// Implementación básica de pipes para comandos educativos
// Captura la salida del primer comando y la pasa como entrada al segundo
static void execute_pipe(char *cmd1, char *cmd2) {
    // Buffer temporal para capturar la salida del primer comando
    static char pipe_buffer[4096];
    static char temp_output[4096];
    
    // Limpiar buffer
    memset(pipe_buffer, 0, sizeof(pipe_buffer));
    memset(temp_output, 0, sizeof(temp_output));
    
    printf("Ejecutando pipe: '%s' | '%s'\n", cmd1, cmd2);
    
    // Para esta implementación educativa, solo soportamos algunos comandos básicos
    // cmd1 debe ser un comando que produzca salida (cat, ls, echo)
    // cmd2 debe ser un comando que procese entrada (grep, wc, head, tail)
    
    // Ejecutar primer comando y capturar salida
    if (strcmp(cmd1, "ls") == 0) {
        // Capturar salida de ls
        int count = 0;
        for (int i = 0; i < 32; i++) {
            fat16_dir_entry entry;
            read_root_entry(i, &entry);
            if (entry.name[0] != 0x00 && entry.name[0] != 0xE5) {
                // Formatear nombre de archivo FAT16 a string normal
                char filename[13];
                int k = 0;
                // Copiar nombre (8 caracteres)
                for (int j = 0; j < 8 && entry.name[j] != ' '; j++) {
                    filename[k++] = entry.name[j];
                }
                // Agregar punto si hay extensión
                if (entry.name[8] != ' ') {
                    filename[k++] = '.';
                    for (int j = 8; j < 11 && entry.name[j] != ' '; j++) {
                        filename[k++] = entry.name[j];
                    }
                }
                filename[k] = '\0';
                
                // Agregar al buffer
                int len = strlen(filename);
                if (count + len + 1 < sizeof(pipe_buffer)) {
                    if (count > 0) {
                        pipe_buffer[count++] = '\n';
                    }
                    memcpy(pipe_buffer + count, filename, len);
                    count += len;
                }
            }
        }
    } else if (strncmp(cmd1, "cat ", 4) == 0) {
        // Capturar salida de cat
        char *filename = cmd1 + 4;
        while (*filename == ' ') filename++; // Limpiar espacios
        
        uint32_t size = 0;
        if (fs_read(filename, pipe_buffer, &size)) {
            // fs_read ya copió el contenido a pipe_buffer
        }
    } else if (strncmp(cmd1, "echo ", 5) == 0) {
        // Capturar salida de echo
        char *text = cmd1 + 5;
        while (*text == ' ') text++; // Limpiar espacios
        
        int len = strlen(text);
        if (len < sizeof(pipe_buffer)) {
            memcpy(pipe_buffer, text, len);
        }
    } else if (strncmp(cmd1, "rev ", 4) == 0) {
        // Capturar salida de rev (texto invertido)
        char *text = cmd1 + 4;
        while (*text == ' ') text++; // Limpiar espacios
        
        int len = strlen(text);
        if (len < sizeof(pipe_buffer)) {
            // Invertir el texto y guardarlo en pipe_buffer
            for (int i = len - 1; i >= 0; i--) {
                pipe_buffer[len - 1 - i] = text[i];
            }
            pipe_buffer[len] = '\0'; // Null terminator
        }
    } else if (strcmp(cmd1, "date") == 0) {
        // Capturar salida de date
        const char *date_str = "Lun Ene 01 12:00:00 UTC 2024";
        int len = strlen(date_str);
        if (len < sizeof(pipe_buffer)) {
            memcpy(pipe_buffer, date_str, len);
            pipe_buffer[len] = '\0';
        }
    } else if (strcmp(cmd1, "whoami") == 0) {
        // Capturar salida de whoami
        const char *user = "root";
        int len = strlen(user);
        if (len < sizeof(pipe_buffer)) {
            memcpy(pipe_buffer, user, len);
            pipe_buffer[len] = '\0';
        }
    } else if (strcmp(cmd1, "uname") == 0) {
        // Capturar salida de uname
        const char *uname_str = "MiniKernel x86 1.0";
        int len = strlen(uname_str);
        if (len < sizeof(pipe_buffer)) {
            memcpy(pipe_buffer, uname_str, len);
            pipe_buffer[len] = '\0';
        }
    } else {
        printf("Error: Comando '%s' no soportado en pipes\n", cmd1);
        printf("Comandos soportados como entrada: ls, cat <file>, echo <text>, rev <text>, date, whoami, uname\n");
        return;
    }
    
    // Procesar segundo comando con la salida del primero
    if (strncmp(cmd2, "grep ", 5) == 0) {
        // grep palabra
        char *search_term = cmd2 + 5;
        while (*search_term == ' ') search_term++; // Limpiar espacios
        
        printf("Buscando '%s' en la salida:\n", search_term);
        
        // Buscar línea por línea
        char *line_start = pipe_buffer;
        char *line_end;
        
        while (*line_start) {
            line_end = line_start;
            while (*line_end && *line_end != '\n') line_end++;
            
            // Crear línea temporal
            char line[256];
            int line_len = line_end - line_start;
            if (line_len < sizeof(line)) {
                memcpy(line, line_start, line_len);
                line[line_len] = '\0';
                
                // Buscar término en la línea
                if (strstr(line, search_term)) {
                    printf("%s\n", line);
                }
            }
            
            // Avanzar a la siguiente línea
            line_start = line_end;
            if (*line_start == '\n') line_start++;
        }
    } else if (strcmp(cmd2, "wc") == 0 || strcmp(cmd2, "wc -l") == 0) {
        // Contar líneas, palabras, caracteres
        int lines = 0, words = 0, chars = 0;
        int in_word = 0;
        
        for (int i = 0; pipe_buffer[i]; i++) {
            chars++;
            if (pipe_buffer[i] == '\n') lines++;
            if (pipe_buffer[i] == ' ' || pipe_buffer[i] == '\n' || pipe_buffer[i] == '\t') {
                in_word = 0;
            } else if (!in_word) {
                in_word = 1;
                words++;
            }
        }
        
        printf("  %d  %d  %d\n", lines, words, chars);
    } else if (strncmp(cmd2, "head", 4) == 0) {
        // Mostrar primeras líneas
        int max_lines = 10; // Por defecto
        
        // Verificar si hay número especificado
        if (strlen(cmd2) > 5) {
            char *num_str = cmd2 + 5;
            while (*num_str == ' ') num_str++;
            if (*num_str >= '0' && *num_str <= '9') {
                max_lines = atoi(num_str);
            }
        }
        
        // Mostrar líneas
        char *line_start = pipe_buffer;
        char *line_end;
        int line_count = 0;
        
        while (*line_start && line_count < max_lines) {
            line_end = line_start;
            while (*line_end && *line_end != '\n') line_end++;
            
            // Imprimir línea
            for (char *p = line_start; p < line_end; p++) {
                printf("%c", *p);
            }
            printf("\n");
            
            line_count++;
            line_start = line_end;
            if (*line_start == '\n') line_start++;
        }
    } else if (strncmp(cmd2, "tail", 4) == 0) {
        // Mostrar últimas líneas (implementación simplificada)
        int max_lines = 10; // Por defecto
        
        printf("Últimas líneas de la salida:\n");
        printf("%s\n", pipe_buffer); // Simplificado para esta versión educativa
    } else if (strcmp(cmd2, "rev") == 0) {
        // Invertir toda la salida del comando anterior
        int len = strlen(pipe_buffer);
        printf("Texto invertido:\n");
        for (int i = len - 1; i >= 0; i--) {
            printf("%c", pipe_buffer[i]);
        }
        printf("\n");
    } else if (strncmp(cmd2, "sort", 4) == 0) {
        // Ordenar líneas (implementación básica)
        printf("Contenido ordenado (simulado):\n");
        printf("%s\n", pipe_buffer);
    } else if (strcmp(cmd2, "uniq") == 0) {
        // Mostrar líneas únicas (implementación básica)
        printf("Líneas únicas:\n");
        printf("%s\n", pipe_buffer);
    } else if (strncmp(cmd2, "cut ", 4) == 0) {
        // Extraer campos (implementación básica)
        printf("Campos extraídos:\n");
        printf("%s\n", pipe_buffer);
    } else {
        printf("Error: Comando '%s' no soportado como destino de pipe\n", cmd2);
        printf("Comandos soportados como salida: grep <patrón>, wc, head [n], tail [n], rev, sort, uniq, cut\n");
        printf("Salida del primer comando:\n%s\n", pipe_buffer);
    }
}

// Bucle principal del shell: lee y ejecuta comandos
// Esta función implementa la lógica básica de cualquier intérprete de comandos
static void shell_loop(void) {
    printf("shell> "); 
    int p = 0;           // Posición actual en el buffer
    int cursor_pos = 0;  // Posición del cursor
    int hist_pos = history_count;  // Posición en el historial
    
    memset(cmdbuf, 0, CMD_BUFSIZE);
    
    // Leer comando del usuario carácter por carácter
    while (1) {
        unsigned char c = getchar_stub();
        
        if (c == '\n') break;  // Fin de línea
        
        // Manejar teclas especiales
        if (c == KEY_UP) {  // Historial hacia atrás
            if (hist_pos > 0 && hist_pos > history_count - HISTORY_SIZE) {
                hist_pos--;
                int idx = hist_pos % HISTORY_SIZE;
                memcpy(cmdbuf, history[idx], CMD_BUFSIZE);
                p = strlen(cmdbuf);
                cursor_pos = p;
                redraw_line(cmdbuf, cursor_pos);
            }
            continue;
        }
        
        if (c == KEY_DOWN) {  // Historial hacia adelante
            if (hist_pos < history_count) {
                hist_pos++;
                if (hist_pos == history_count) {
                    // Volver a línea vacía
                    memset(cmdbuf, 0, CMD_BUFSIZE);
                    p = 0;
                } else {
                    int idx = hist_pos % HISTORY_SIZE;
                    memcpy(cmdbuf, history[idx], CMD_BUFSIZE);
                    p = strlen(cmdbuf);
                }
                cursor_pos = p;
                redraw_line(cmdbuf, cursor_pos);
            }
            continue;
        }
        
        if (c == KEY_LEFT) {  // Mover cursor a la izquierda
            if (cursor_pos > 0) {
                cursor_pos--;
                putchar('\b');
            }
            continue;
        }
        
        if (c == KEY_RIGHT) {  // Mover cursor a la derecha
            if (cursor_pos < p) {
                putchar(cmdbuf[cursor_pos]);
                cursor_pos++;
            }
            continue;
        }
        
        // Manejar backspace y delete
        if (c == 0x08 || c == '\b' || c == KEY_DELETE) {  // Backspace o Delete
            if (c == KEY_DELETE) {
                // Delete: borrar carácter a la derecha del cursor
                if (cursor_pos < p) {
                    for (int i = cursor_pos; i < p - 1; i++) {
                        cmdbuf[i] = cmdbuf[i + 1];
                    }
                    p--;
                    cmdbuf[p] = '\0';
                    redraw_line(cmdbuf, cursor_pos);
                }
            } else {
                // Backspace: borrar carácter a la izquierda del cursor
                if (cursor_pos > 0) {
                    for (int i = cursor_pos - 1; i < p - 1; i++) {
                        cmdbuf[i] = cmdbuf[i + 1];
                    }
                    p--;
                    cursor_pos--;
                    cmdbuf[p] = '\0';
                    redraw_line(cmdbuf, cursor_pos);
                }
            }
            continue;
        }
        
        // Carácter normal - insertar en la posición del cursor
        if (p < CMD_BUFSIZE - 1 && c >= 32 && c < 127) {  // Solo caracteres imprimibles
            // Mover caracteres a la derecha para hacer espacio
            for (int i = p; i > cursor_pos; i--) {
                cmdbuf[i] = cmdbuf[i - 1];
            }
            cmdbuf[cursor_pos] = c;
            p++;
            cursor_pos++;
            cmdbuf[p] = '\0';
            
            // Redibujar desde la posición del cursor
            for (int i = cursor_pos - 1; i < p; i++) {
                putchar(cmdbuf[i]);
            }
            // Volver a la posición correcta del cursor
            for (int i = cursor_pos; i < p; i++) {
                putchar('\b');
            }
        }
    }
    
    cmdbuf[p] = '\0';  // Terminar la cadena
    putchar('\n');
    
    // Agregar al historial si no está vacío
    if (p > 0) {
        add_to_history(cmdbuf);
    }
    
    // Verificar si hay pipe en el comando
    char *pipe_pos = strchr(cmdbuf, '|');
    if (pipe_pos) {
        // Separar comandos por pipe
        *pipe_pos = '\0';
        char *cmd1 = cmdbuf;
        char *cmd2 = pipe_pos + 1;
        
        // Limpiar espacios al inicio
        while (*cmd1 == ' ') cmd1++;
        while (*cmd2 == ' ') cmd2++;
        
        // Limpiar espacios al final de cmd1
        char *end1 = cmd1 + strlen(cmd1) - 1;
        while (end1 >= cmd1 && *end1 == ' ') {
            *end1 = '\0';
            end1--;
        }
        
        // Limpiar espacios al final de cmd2
        char *end2 = cmd2 + strlen(cmd2) - 1;
        while (end2 >= cmd2 && *end2 == ' ') {
            *end2 = '\0';
            end2--;
        }
        
        // Validar que ambos comandos no estén vacíos
        if (strlen(cmd1) == 0) {
            printf("Error: Comando vacío antes del pipe\n");
            return;
        }
        if (strlen(cmd2) == 0) {
            printf("Error: Comando vacío después del pipe\n");
            return;
        }
        
        // Ejecutar pipe simple
        execute_pipe(cmd1, cmd2);
        return;
    }
    
    // Separar comando de argumentos (por el primer espacio)
    char *cmd = cmdbuf, *arg = strchr(cmd, ' ');
    if (arg) { 
        *arg = '\0';  // Terminar el comando
        arg++;        // Apuntar al primer argumento
    }
    
    // Interpretar y ejecutar los comandos
    if (!strcmp(cmd,"ls")) fs_ls();
    else if (!strcmp(cmd,"cat") && arg) {
        uint8_t buf[SECTOR_SIZE]; uint32_t sz;
        if (fs_read(arg, buf, &sz)) for (uint32_t i=0;i<sz;i++) putchar(buf[i]);
    } else if (!strcmp(cmd,"echo") && arg) { prints(arg); putchar('\n'); }
    else if (!strcmp(cmd,"touch") && arg) fs_touch(arg);
    else if (!strcmp(cmd,"cp") && arg) {
        // Comando cp: copiar archivo de origen a destino
        char *dst = strchr(arg,' '); 
        if (dst) { 
            *dst=0; dst++; 
            uint8_t buf2[SECTOR_SIZE]; uint32_t sz2; 
            if(fs_read(arg,buf2,&sz2)) {
                fs_write(dst,buf2,sz2);  // fs_write creará el archivo si no existe
                printf("Archivo copiado: %s -> %s\n", arg, dst);
            } else {
                printf("Archivo no encontrado: %s\n", arg);
            }
        }
    } else if (!strcmp(cmd,"copy") && arg) {
        // Alias del comando cp para compatibilidad con MS-DOS
        char *dst = strchr(arg,' '); 
        if (dst) { 
            *dst=0; dst++; 
            uint8_t buf2[SECTOR_SIZE]; uint32_t sz2; 
            if(fs_read(arg,buf2,&sz2)) {
                fs_write(dst,buf2,sz2);  // fs_write creará el archivo si no existe
                printf("Archivo copiado: %s -> %s\n", arg, dst);
            } else {
                printf("Archivo no encontrado: %s\n", arg);
            }
        }
    } else if (!strcmp(cmd,"mv") && arg) {
        // Comando mv: renombrar/mover archivo
        char *dst = strchr(arg,' '); 
        if (dst) { 
            *dst=0; dst++; 
            fat16_dir_entry e; int idx;
            if (fs_find(arg, &e, &idx)) {
                fs_mv(arg,dst);
                printf("Archivo renombrado: %s -> %s\n", arg, dst);
            } else {
                printf("Archivo no encontrado: %s\n", arg);
            }
        }
    } else if (!strcmp(cmd,"delete") && arg) {
        // Comando delete: eliminar archivo
        fs_delete(arg);
    } else if (!strcmp(cmd,"copycon") && arg) {
        // Comando copycon: crear archivo desde entrada de teclado
        // El usuario escribe contenido hasta presionar Ctrl+Z (ASCII 26) o Ctrl+D (ASCII 4)
        printf("Escriba el contenido del archivo (Ctrl+Z o Ctrl+D para terminar):\n");
        uint8_t buf3[SECTOR_SIZE]; int i=0; 
        while (1) { 
            unsigned char c=getchar_stub(); 
            
            if(c==0x1A || c==0x04) { // Ctrl+Z (0x1A) o Ctrl+D (0x04) para terminar
                printf("^%c\n", c==0x1A ? 'Z' : 'D'); 
                break;
            }
            
            if(i>=SECTOR_SIZE-1) break; // Evitar desbordamiento
            
            // Manejar backspace y delete
            if (c == 0x08 || c == '\b' || c == KEY_DELETE) {
                if (i > 0) {
                    i--;  // Retroceder en el buffer
                    putchar('\b');  // Mover cursor atrás
                    putchar(' ');   // Borrar carácter
                    putchar('\b');  // Volver a posición original
                }
                continue;
            }
            
            buf3[i++]=c;
            putchar(c); // Echo del carácter
        } 
        fs_write(arg, buf3, i);
        printf("Archivo creado: %s (%d bytes)\n", arg, i);
    } else if (!strcmp(cmd,"clear") || !strcmp(cmd,"cls")) clear_screen();
    else if (!strcmp(cmd,"free")) {
        uint32_t used = (TOTAL_SECTORS*SECTOR_SIZE)/1024;
        printf("RAM libre: %u KB\n", used);
    } else if (!strcmp(cmd,"help")||!strcmp(cmd,"?")) {
        show_help_with_pause();
    } else if (!strcmp(cmd, "edln") && arg) {
        // Comando edln: editar línea específica de un archivo
        // Formato: edln <archivo> <numero_linea> <texto>
        
        // Parsear manualmente los argumentos desde arg
        char *filename = arg;
        char *space1 = strchr(filename, ' ');
        if (!space1) {
            printf("Uso: edln <archivo> <numero_linea> <texto>\n");
            return;
        }
        *space1 = '\0';  // Terminar filename
        
        char *line_num_str = space1 + 1;
        char *space2 = strchr(line_num_str, ' ');
        if (!space2) {
            printf("Uso: edln <archivo> <numero_linea> <texto>\n");
            return;
        }
        *space2 = '\0';  // Terminar line_num_str
        
        char *new_text = space2 + 1;
        if (strlen(new_text) == 0) {
            printf("Uso: edln <archivo> <numero_linea> <texto>\n");
            return;
        }
        
        int line_num = atoi(line_num_str);
        if (line_num < 1) {
            printf("Error: '%s' no es un numero valido (resultado: %d)\n", line_num_str, line_num);
            printf("Uso: edln <archivo> <numero_linea> <texto>\n");
            return;
        }
        
        printf("Editando archivo: %s, linea: %d, texto: %s\n", filename, line_num, new_text);
        fs_edit_line(filename, line_num, new_text);
    } else if (!strcmp(cmd, "delln") && arg) {
        // Comando delln: eliminar línea específica de un archivo
        // Formato: delln <archivo> <numero_linea>
        
        char *filename = arg;
        char *space1 = strchr(filename, ' ');
        if (!space1) {
            printf("Uso: delln <archivo> <numero_linea>\n");
            return;
        }
        *space1 = '\0';  // Terminar filename
        
        char *line_num_str = space1 + 1;
        if (strlen(line_num_str) == 0) {
            printf("Uso: delln <archivo> <numero_linea>\n");
            return;
        }
        
        int line_num = atoi(line_num_str);
        if (line_num < 1) {
            printf("Error: '%s' no es un numero valido (resultado: %d)\n", line_num_str, line_num);
            printf("Uso: delln <archivo> <numero_linea>\n");
            return;
        }
        
        fs_delete_line(filename, line_num);
    } else if (!strcmp(cmd, "insln") && arg) {
        // Comando insln: insertar línea en blanco en posición específica
        // Formato: insln <archivo> <numero_linea>
        
        char *filename = arg;
        char *space1 = strchr(filename, ' ');
        if (!space1) {
            printf("Uso: insln <archivo> <numero_linea>\n");
            return;
        }
        *space1 = '\0';  // Terminar filename
        
        char *line_num_str = space1 + 1;
        if (strlen(line_num_str) == 0) {
            printf("Uso: insln <archivo> <numero_linea>\n");
            return;
        }
        
        int line_num = atoi(line_num_str);
        if (line_num < 1) {
            printf("Error: '%s' no es un numero valido (resultado: %d)\n", line_num_str, line_num);
            printf("Uso: insln <archivo> <numero_linea>\n");
            return;
        }
        
        fs_insert_line(filename, line_num);
    } else if (!strcmp(cmd, "hexdump") && arg) {
        // Comando hexdump: mostrar archivo en hexadecimal
        fs_hexdump(arg);
    } else if (!strcmp(cmd, "wc") && arg) {
        // Comando wc: contar líneas, palabras y caracteres
        fs_wc(arg);
    } else if (!strcmp(cmd, "grep") && arg) {
        // Comando grep: buscar patrón en archivo
        char *pattern = arg;
        char *space = strchr(pattern, ' ');
        if (!space) {
            printf("Uso: grep <patron> <archivo>\n");
            return;
        }
        *space = '\0';
        char *filename = space + 1;
        fs_grep(pattern, filename);
    } else if (!strcmp(cmd, "head") && arg) {
        // Comando head: mostrar primeras líneas
        char *filename = arg;
        char *space = strchr(filename, ' ');
        int lines = 10;  // Por defecto 10 líneas
        if (space) {
            *space = '\0';
            lines = atoi(space + 1);
            if (lines < 1) lines = 10;
        }
        fs_head(filename, lines);
    } else if (!strcmp(cmd, "tail") && arg) {
        // Comando tail: mostrar últimas líneas
        char *filename = arg;
        char *space = strchr(filename, ' ');
        int lines = 10;  // Por defecto 10 líneas
        if (space) {
            *space = '\0';
            lines = atoi(space + 1);
            if (lines < 1) lines = 10;
        }
        fs_tail(filename, lines);
    } else if (!strcmp(cmd, "mkdir") && arg) {
        // Comando mkdir: crear directorio simulado
        fs_mkdir(arg);
    } else if (!strcmp(cmd, "pwd")) {
        // Comando pwd: mostrar directorio actual
        printf("/root\n");
    } else if (!strcmp(cmd, "whoami")) {
        // Comando whoami: mostrar usuario actual
        printf("root\n");
    } else if (!strcmp(cmd, "uname")) {
        // Comando uname: información del sistema
        printf("r2os 1.0 i686 mini-kernel educativo\n");
    } else if (!strcmp(cmd, "uptime")) {
        // Comando uptime: tiempo de funcionamiento (simulado)
        printf("Sistema funcionando correctamente\n");
    } else if (!strcmp(cmd, "date")) {
        // Comando date: fecha actual (simulada)
        printf("Lun Dic  1 12:00:00 UTC 2024\n");
    } else if (!strcmp(cmd, "cal")) {
        // Comando cal: calendario simple
        printf("   Diciembre 2024\n");
        printf("Do Lu Ma Mi Ju Vi Sa\n");
        printf(" 1  2  3  4  5  6  7\n");
        printf(" 8  9 10 11 12 13 14\n");
        printf("15 16 17 18 19 20 21\n");
        printf("22 23 24 25 26 27 28\n");
        printf("29 30 31\n");
    } else if (!strcmp(cmd, "yes") && arg) {
        // Comando yes: repetir texto (limitado)
        for (int i = 0; i < 10; i++) {
            printf("%s\n", arg);
        }
        printf("(limitado a 10 repeticiones)\n");
    } else if (!strcmp(cmd, "rev") && arg) {
        // Comando rev: invertir texto
        int len = strlen(arg);
        for (int i = len - 1; i >= 0; i--) {
            putchar(arg[i]);
        }
        printf("\n");
    } else if (!strcmp(cmd, "sort") && arg) {
        // Comando sort: mostrar contenido ordenado (simple)
        uint8_t buffer[SECTOR_SIZE];
        uint32_t file_size = 0;
        if (fs_read(arg, buffer, &file_size)) {
            printf("Contenido de %s (simulacion de sort):\n", arg);
            for (uint32_t i = 0; i < file_size; i++) {
                putchar(buffer[i]);
            }
        } else {
            printf("Archivo no encontrado: %s\n", arg);
        }
    } else if (!strcmp(cmd, "uniq") && arg) {
        // Comando uniq: líneas únicas (simple)
        printf("Simulacion de uniq para: %s\n", arg);
        fs_tail(arg, 5);  // Mostrar últimas 5 líneas como demo
    } else if (!strcmp(cmd, "which") && arg) {
        // Comando which: encontrar comando
        if (!strcmp(arg, "ls") || !strcmp(arg, "cat") || !strcmp(arg, "cp") || 
            !strcmp(arg, "echo") || !strcmp(arg, "grep") || !strcmp(arg, "head")) {
            printf("/bin/%s\n", arg);
        } else {
            printf("%s: comando no encontrado\n", arg);
        }
    } else if (!strcmp(cmd, "file") && arg) {
        // Comando file: tipo de archivo
        uint8_t buffer[16];
        uint32_t file_size = 0;
        if (fs_read(arg, buffer, &file_size)) {
            if (file_size == 0) {
                printf("%s: archivo vacio\n", arg);
            } else if (buffer[0] == '[' && buffer[file_size-1] == ']') {
                printf("%s: directorio simulado\n", arg);
            } else {
                printf("%s: archivo de texto ASCII\n", arg);
            }
        } else {
            printf("%s: archivo no encontrado\n", arg);
        }
    } else if (!strcmp(cmd, "du") && arg) {
        // Comando du: uso de disco
        uint8_t buffer[SECTOR_SIZE];
        uint32_t file_size = 0;
        if (fs_read(arg, buffer, &file_size)) {
            printf("%u\t%s\n", (file_size + 511) / 512, arg);  // En sectores
        } else {
            printf("0\t%s (no encontrado)\n", arg);
        }
    } else if (!strcmp(cmd, "stat") && arg) {
        // Comando stat: estadísticas de archivo
        fat16_dir_entry e; int idx;
        if (fs_find(arg, &e, &idx)) {
            printf("Archivo: %s\n", arg);
            printf("Tamaño: %u bytes\n", e.size);
            printf("Cluster: %u\n", e.first_cluster);
            printf("Tipo: archivo regular\n");
        } else {
            printf("stat: %s: archivo no encontrado\n", arg);
        }
    } else if (!strcmp(cmd, "basename") && arg) {
        // Comando basename: nombre base de archivo
        printf("%s\n", arg);  // Simplificado, no hay rutas
    } else if (!strcmp(cmd, "dirname") && arg) {
        // Comando dirname: directorio de archivo
        printf(".\n");  // Simplificado, todo en directorio actual
    } else if (!strcmp(cmd, "tee") && arg) {
        // Comando tee: escribir a archivo y pantalla
        printf("Escriba texto (Ctrl+D para terminar):\n");
        uint8_t buffer[256]; int i = 0;
        while (i < 255) {
            unsigned char c = getchar_stub();
            if (c == 0x04) break;  // Ctrl+D
            buffer[i++] = c;
            putchar(c);
        }
        fs_write(arg, buffer, i);
        printf("\nTexto guardado en: %s\n", arg);
    } else if (!strcmp(cmd, "man") && arg) {
        // Comando man: manual de comandos
        if (!strcmp(arg, "ls")) {
            printf("MANUAL: ls\n");
            printf("Uso: ls\n");
            printf("Descripcion: Lista todos los archivos del directorio raiz\n");
            printf("Muestra: nombre del archivo y tamaño en bytes\n");
        } else if (!strcmp(arg, "cat")) {
            printf("MANUAL: cat\n");
            printf("Uso: cat <archivo>\n");
            printf("Descripcion: Muestra el contenido completo de un archivo\n");
            printf("Ejemplo: cat hola.txt\n");
        } else if (!strcmp(arg, "copycon")) {
            printf("MANUAL: copycon\n");
            printf("Uso: copycon <archivo>\n");
            printf("Descripcion: Crea un archivo escribiendo desde el teclado\n");
            printf("Terminar: Ctrl+Z o Ctrl+D\n");
            printf("Ejemplo: copycon test.txt\n");
        } else if (!strcmp(arg, "edln")) {
            printf("MANUAL: edln\n");
            printf("Uso: edln <archivo> <numero_linea> <texto>\n");
            printf("Descripcion: Edita o crea una linea especifica en un archivo\n");
            printf("Ejemplo: edln test.txt 2 nueva linea\n");
        } else if (!strcmp(arg, "grep")) {
            printf("MANUAL: grep\n");
            printf("Uso: grep <patron> <archivo>\n");
            printf("Descripcion: Busca un patron de texto en un archivo\n");
            printf("Ejemplo: grep hola test.txt\n");
        } else if (!strcmp(arg, "head")) {
            printf("MANUAL: head\n");
            printf("Uso: head <archivo> [numero_lineas]\n");
            printf("Descripcion: Muestra las primeras lineas de un archivo\n");
            printf("Por defecto: 10 lineas\n");
            printf("Ejemplo: head test.txt 5\n");
        } else if (!strcmp(arg, "tail")) {
            printf("MANUAL: tail\n");
            printf("Uso: tail <archivo> [numero_lineas]\n");
            printf("Descripcion: Muestra las ultimas lineas de un archivo\n");
            printf("Por defecto: 10 lineas\n");
            printf("Ejemplo: tail test.txt 3\n");
        } else if (!strcmp(arg, "wc")) {
            printf("MANUAL: wc\n");
            printf("Uso: wc <archivo>\n");
            printf("Descripcion: Cuenta lineas, palabras y caracteres\n");
            printf("Formato salida: lineas palabras caracteres archivo\n");
        } else if (!strcmp(arg, "hexdump")) {
            printf("MANUAL: hexdump\n");
            printf("Uso: hexdump <archivo>\n");
            printf("Descripcion: Muestra el contenido en formato hexadecimal\n");
            printf("Incluye: direccion, hex y representacion ASCII\n");
        } else if (!strcmp(arg, "history")) {
            printf("MANUAL: history\n");
            printf("Uso: history\n");
            printf("Descripcion: Muestra los ultimos comandos ejecutados\n");
            printf("Navegacion: Usar flechas arriba/abajo en shell\n");
        } else {
            printf("man: No hay manual para '%s'\n", arg);
            printf("Comandos con manual: ls, cat, copycon, edln, grep, head, tail, wc, hexdump, history\n");
        }
    } else if (!strcmp(cmd, "history")) {
        // Comando history: mostrar historial de comandos
        printf("=== HISTORIAL DE COMANDOS ===\n");
        if (history_count == 0) {
            printf("No hay comandos en el historial.\n");
        } else {
            printf("Total de comandos: %d\n", history_count);
            int start = (history_count > HISTORY_SIZE) ? history_count - HISTORY_SIZE : 0;
            int end = history_count;
            
            for (int i = start; i < end; i++) {
                int idx = i % HISTORY_SIZE;
                if (history[idx][0] != '\0') {  // Verificar que no esté vacío
                    printf("%2d: %s\n", i + 1, history[idx]);
                }
            }
        }
    } else if (!strcmp(cmd, "testpipe")) {
        printf("=== TEST DEL CARACTER PIPE ===\n");
        printf("Presiona diferentes teclas para encontrar el pipe '|':\n");
        printf("- Intenta: Shift + \\ (backslash)\n");
        printf("- En Mac: podría ser Alt + L o Shift + Alt + L\n");
        printf("- También puedes probar otras combinaciones\n");
        printf("Escribe 'quit' para salir del test\n\n");
        
        while (1) {
            printf("test> ");
            char test_input[32];
            int pos = 0;
            
            while (1) {
                char c = getchar_stub();
                if (c == '\n') {
                    test_input[pos] = '\0';
                    break;
                }
                if (c == '\b' && pos > 0) {
                    pos--;
                    printf("\b \b");
                } else if (c >= 32 && c <= 126 && pos < 31) {
                    test_input[pos++] = c;
                    printf("0x%02x('%c') ", (unsigned char)c, c);
                }
            }
            printf("\n");
            
            if (strcmp(test_input, "quit") == 0) break;
            
            // Verificar si contiene pipe
            if (strchr(test_input, '|')) {
                printf("¡EXCELENTE! Encontraste el caracter pipe: %s\n", test_input);
                printf("Ahora puedes usar pipes como:\n");
                printf("  ls | wc                  - Contar archivos\n");
                printf("  echo 'hello world' | rev - Invertir texto\n");
                printf("  cat archivo.txt | grep palabra - Buscar en archivo\n");
                printf("  date | rev               - Fecha invertida\n");
                printf("  whoami | grep root       - Buscar usuario\n");
                break;
            } else {
                printf("No se detectó el caracter pipe en: %s\n", test_input);
            }
        }
    } else if (strlen(cmd) > 0) {
        printf("comando no encontrado: %s\n",cmd);
    }
}

// =============================================================================
// FUNCIÓN PRINCIPAL DEL KERNEL
// =============================================================================
// Esta es la función que se ejecuta cuando arranca el sistema operativo.
// Inicializa la pantalla y entra en el bucle principal del shell.
void kernel_main(void) {
    // Limpiar pantalla al inicio
    clear_screen();
    
    // Mensaje de bienvenida
    printf("Bienvenido al mini-kernel educativo!\n");
    printf("Inicializando sistema...\n\n");
    
    // Inicializar el sistema de archivos
    fs_init();
    
    // Mostrar ayuda automáticamente al arrancar
    show_help();
    
    // Entrar en el bucle principal del shell
    while (1) shell_loop();
}
