# Capítulo 4: Shell y Comunicación Inter-Proceso

## 📚 Introducción

Este capítulo final explora la implementación de shells avanzados y sistemas de comunicación inter-proceso (IPC). Utilizaremos el sistema de pipes implementado en el mini-kernel educativo como base para comprender cómo los procesos pueden comunicarse de manera eficiente y cómo construir interfaces de usuario robustas en sistemas operativos.

## 🎯 Objetivos de Aprendizaje

Al completar este capítulo, el estudiante será capaz de:

1. **Implementar** parsers de comandos avanzados con soporte para pipes
2. **Diseñar** sistemas de comunicación inter-proceso eficientes
3. **Desarrollar** shells con funcionalidades avanzadas
4. **Analizar** patrones de concurrencia en sistemas de pipes
5. **Optimizar** la comunicación entre procesos para máximo rendimiento

---

## 🏛️ Parte Teórica

### 1.1 Fundamentos de Shells

#### ¿Qué es un Shell?

Un shell es la interfaz entre el usuario y el sistema operativo. Actúa como intérprete de comandos, proporcionando una capa de abstracción que permite a los usuarios interactuar con el kernel de manera intuitiva.

```
┌─────────────────────────────────────────────────┐
│                   USUARIO                       │
├─────────────────────────────────────────────────┤
│                    SHELL                        │
│  Parser → Executor → Pipe Handler → Output     │
├─────────────────────────────────────────────────┤
│              SISTEMA OPERATIVO                  │
│    Procesos → IPC → Sistema de Archivos        │
├─────────────────────────────────────────────────┤
│                   HARDWARE                      │
└─────────────────────────────────────────────────┘
```

#### Funciones Principales del Shell

**1. Interpretación de Comandos:**
- Parsing de sintaxis compleja
- Resolución de nombres de comandos
- Expansión de variables y wildcards
- Validación de sintaxis

**2. Gestión de Procesos:**
- Creación de procesos hijos
- Control de jobs (foreground/background)
- Manejo de señales
- Recolección de estados de terminación

**3. Redirección y Pipes:**
- Redirección de entrada/salida
- Construcción de pipelines
- Manejo de descriptores de archivo
- Buffering y sincronización

**4. Gestión del Entorno:**
- Variables de entorno
- Directorio de trabajo actual
- PATH y resolución de comandos
- Configuración persistente

### 1.2 Arquitectura de Parsing

#### Análisis Léxico (Tokenización)

```c
// Estados del parser
typedef enum {
    STATE_NORMAL,
    STATE_IN_QUOTES,
    STATE_ESCAPE,
    STATE_PIPE,
    STATE_REDIRECT
} parser_state_t;

// Token types
typedef enum {
    TOKEN_WORD,
    TOKEN_PIPE,
    TOKEN_REDIRECT_IN,
    TOKEN_REDIRECT_OUT,
    TOKEN_BACKGROUND,
    TOKEN_EOF
} token_type_t;

typedef struct {
    token_type_t type;
    char *value;
    int position;
} token_t;
```

**Ejemplo de tokenización:**
```bash
Input:  echo "hello world" | grep hello > output.txt &
Tokens: [WORD:"echo"] [WORD:"hello world"] [PIPE] [WORD:"grep"] 
        [WORD:"hello"] [REDIRECT_OUT] [WORD:"output.txt"] [BACKGROUND]
```

#### Análisis Sintáctico

**Grammar BNF para nuestro shell:**
```bnf
<command_line> ::= <pipeline> [<background>]
<pipeline>     ::= <command> [<pipe> <pipeline>]
<command>      ::= <word> [<args>] [<redirections>]
<args>         ::= <word> [<args>]
<redirections> ::= <redirect> [<redirections>]
<redirect>     ::= ">" <word> | "<" <word> | ">>" <word>
<pipe>         ::= "|"
<background>   ::= "&"
<word>         ::= [a-zA-Z0-9_./]+
```

#### Abstract Syntax Tree (AST)

```c
typedef struct command_node {
    char **argv;              // Argumentos del comando
    char *input_file;         // Redirección de entrada
    char *output_file;        // Redirección de salida
    int append_output;        // Modo append (>>)
    struct command_node *next; // Siguiente comando en pipe
} command_node_t;

typedef struct {
    command_node_t *first_command;
    int background;           // Ejecutar en background (&)
    int command_count;        // Número de comandos en pipeline
} pipeline_t;
```

### 1.3 Comunicación Inter-Proceso (IPC)

#### Taxonomía de Mecanismos IPC

**1. Pipes (Tuberías)**
```
Proceso A ─────────► [PIPE] ─────────► Proceso B
         stdout              stdin
```

**Características:**
- Comunicación unidireccional
- FIFO (First In, First Out)
- Sincronización automática
- Buffer limitado del kernel

**2. Named Pipes (FIFOs)**
```bash
# Crear FIFO
mkfifo /tmp/mypipe

# Proceso escritor
echo "data" > /tmp/mypipe &

# Proceso lector
cat < /tmp/mypipe
```

**3. Message Queues**
```c
struct message {
    long msg_type;
    char msg_text[256];
};

// Enviar mensaje
msgsnd(msgid, &message, sizeof(message.msg_text), 0);

// Recibir mensaje
msgrcv(msgid, &message, sizeof(message.msg_text), 1, 0);
```

**4. Shared Memory**
```c
// Crear segmento de memoria compartida
int shmid = shmget(IPC_PRIVATE, 4096, IPC_CREAT | 0666);

// Attachar al espacio de direcciones
char *shm_ptr = shmat(shmid, NULL, 0);

// Usar memoria compartida
strcpy(shm_ptr, "shared data");

// Detach
shmdt(shm_ptr);
```

**5. Signals**
```c
// Handler de señal
void signal_handler(int sig) {
    printf("Received signal %d\n", sig);
}

// Registrar handler
signal(SIGUSR1, signal_handler);

// Enviar señal
kill(pid, SIGUSR1);
```

#### Patterns de Comunicación

**1. Producer-Consumer**
```c
// Buffer circular para pipes
struct pipe_buffer {
    char data[PIPE_BUF_SIZE];
    int read_pos;
    int write_pos;
    int count;
    mutex_t mutex;
    condition_t not_full;
    condition_t not_empty;
};

void pipe_write(struct pipe_buffer *buf, char *data, int len) {
    mutex_lock(&buf->mutex);
    
    while (buf->count + len > PIPE_BUF_SIZE) {
        condition_wait(&buf->not_full, &buf->mutex);
    }
    
    memcpy(buf->data + buf->write_pos, data, len);
    buf->write_pos = (buf->write_pos + len) % PIPE_BUF_SIZE;
    buf->count += len;
    
    condition_signal(&buf->not_empty);
    mutex_unlock(&buf->mutex);
}
```

**2. Request-Response**
```c
typedef struct {
    int request_id;
    int client_pid;
    int operation;
    char data[256];
} request_t;

typedef struct {
    int request_id;
    int status;
    char result[256];
} response_t;
```

**3. Publisher-Subscriber**
```c
struct topic {
    char name[64];
    struct subscriber *subscribers;
    int subscriber_count;
};

void publish_message(struct topic *topic, char *message) {
    for (int i = 0; i < topic->subscriber_count; i++) {
        send_message(topic->subscribers[i].pid, message);
    }
}
```

### 1.4 Sincronización en Pipes

#### Problemas de Concurrencia

**1. Buffer Overflow:**
```c
// Problema: escritor más rápido que lector
while (data_available()) {
    if (pipe_write(pipe, data, len) == -1) {
        // Pipe lleno - escritor debe esperar
        sleep_until_space_available();
    }
}
```

**2. Deadlock en Pipes Bidireccionales:**
```c
// Proceso A
write(pipe_a_to_b, large_data, LARGE_SIZE);  // Puede bloquear
read(pipe_b_to_a, response, RESPONSE_SIZE);  // Deadlock

// Proceso B
write(pipe_b_to_a, large_data, LARGE_SIZE);  // Puede bloquear
read(pipe_a_to_b, response, RESPONSE_SIZE);  // Deadlock
```

**Solución - Non-blocking I/O:**
```c
int flags = fcntl(pipe_fd, F_GETFL);
fcntl(pipe_fd, F_SETFL, flags | O_NONBLOCK);

int bytes_written = write(pipe_fd, data, len);
if (bytes_written == -1 && errno == EAGAIN) {
    // Pipe lleno, intentar más tarde
    add_to_write_queue(data, len);
}
```

#### Modelos de Concurrencia

**1. Asynchronous I/O:**
```c
struct aiocb aio_request;
aio_request.aio_fildes = pipe_fd;
aio_request.aio_buf = buffer;
aio_request.aio_nbytes = buffer_size;

// Operación asíncrona
aio_read(&aio_request);

// Verificar completion
while (aio_error(&aio_request) == EINPROGRESS) {
    // Hacer otro trabajo
    do_other_work();
}
```

**2. Event-Driven Programming:**
```c
struct epoll_event events[MAX_EVENTS];
int epoll_fd = epoll_create1(0);

// Agregar pipe al monitor
struct epoll_event ev;
ev.events = EPOLLIN | EPOLLOUT;
ev.data.fd = pipe_fd;
epoll_ctl(epoll_fd, EPOLL_CTL_ADD, pipe_fd, &ev);

// Event loop
while (1) {
    int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
    for (int i = 0; i < nfds; i++) {
        if (events[i].events & EPOLLIN) {
            handle_read(events[i].data.fd);
        }
        if (events[i].events & EPOLLOUT) {
            handle_write(events[i].data.fd);
        }
    }
}
```

---

## 🔧 Parte Práctica

### 2.1 Análisis del Sistema de Pipes del Mini-Kernel

#### Estructura del Parser de Comandos

```c
// kernel.c: líneas 1800-1900 - Función principal del shell
static void shell(void) {
    char input[256];
    char *cmd_history[HISTORY_SIZE];
    int history_count = 0;
    int history_pos = -1;
    
    printf("Sistema listo. Usa 'help' para ver comandos disponibles.\n\n");
    
    while (1) {
        printf("shell> ");
        
        // Lectura con historial y navegación
        read_line_with_history(input, sizeof(input), cmd_history, 
                              &history_count, &history_pos);
        
        // Verificar si contiene pipe
        if (strchr(input, '|')) {
            execute_pipe(input);
        } else {
            // Comando simple
            execute_single_command(input);
        }
    }
}
```

#### Implementación del Parser de Pipes

```c
// kernel.c: líneas 1750-1800 - Función execute_pipe
static void execute_pipe(const char *input) {
    char cmd_line[256];
    strcpy(cmd_line, input);
    
    // Buscar el carácter pipe
    char *pipe_pos = strchr(cmd_line, '|');
    if (!pipe_pos) return;
    
    // Dividir comandos
    *pipe_pos = '\0';
    char *cmd1 = cmd_line;
    char *cmd2 = pipe_pos + 1;
    
    // Limpiar espacios
    trim_spaces(cmd1);
    trim_spaces(cmd2);
    
    // Buffer para comunicación
    char pipe_buffer[PIPE_BUFFER_SIZE];
    memset(pipe_buffer, 0, sizeof(pipe_buffer));
    
    // Ejecutar primer comando (productor)
    int output_size = execute_command_with_output(cmd1, pipe_buffer, 
                                                 sizeof(pipe_buffer));
    
    if (output_size > 0) {
        // Ejecutar segundo comando (consumidor)
        execute_command_with_input(cmd2, pipe_buffer, output_size);
    }
}
```

#### Comandos de Entrada (Productores)

```c
// Comandos que generan salida para pipes
static int execute_command_with_output(const char *cmd, char *output, int max_size) {
    char cmd_name[64], arg[192];
    parse_command(cmd, cmd_name, arg);
    
    int output_pos = 0;
    
    if (!strcmp(cmd_name, "ls")) {
        // Generar listado de archivos
        fat16_dir_entry *root_dir = get_root_directory();
        for (int i = 0; i < 32; i++) {
            if (root_dir[i].name[0] != 0x00 && root_dir[i].name[0] != 0xE5) {
                char filename[12];
                format_filename(root_dir[i].name, filename);
                output_pos += snprintf(output + output_pos, 
                                     max_size - output_pos, "%s\n", filename);
            }
        }
    }
    else if (!strcmp(cmd_name, "cat") && strlen(arg) > 0) {
        // Leer contenido de archivo
        uint8_t file_buffer[SECTOR_SIZE];
        uint32_t file_size;
        if (fs_read(arg, file_buffer, &file_size)) {
            int copy_size = (file_size < max_size - 1) ? file_size : max_size - 1;
            memcpy(output, file_buffer, copy_size);
            output[copy_size] = '\0';
            output_pos = copy_size;
        }
    }
    else if (!strcmp(cmd_name, "echo")) {
        // Copiar texto de entrada
        output_pos = snprintf(output, max_size, "%s\n", arg);
    }
    else if (!strcmp(cmd_name, "date")) {
        output_pos = snprintf(output, max_size, "Fecha: Sistema iniciado\n");
    }
    else if (!strcmp(cmd_name, "whoami")) {
        output_pos = snprintf(output, max_size, "root\n");
    }
    
    return output_pos;
}
```

#### Comandos de Salida (Consumidores)

```c
// Comandos que procesan entrada de pipes
static void execute_command_with_input(const char *cmd, const char *input, int input_size) {
    char cmd_name[64], arg[192];
    parse_command(cmd, cmd_name, arg);
    
    if (!strcmp(cmd_name, "grep")) {
        // Filtrar líneas que contengan el patrón
        pipe_grep(input, arg);
    }
    else if (!strcmp(cmd_name, "wc")) {
        // Contar líneas, palabras, caracteres
        pipe_wc(input);
    }
    else if (!strcmp(cmd_name, "head")) {
        // Mostrar primeras n líneas
        int n = (strlen(arg) > 0) ? atoi(arg) : 10;
        pipe_head(input, n);
    }
    else if (!strcmp(cmd_name, "tail")) {
        // Mostrar últimas n líneas
        int n = (strlen(arg) > 0) ? atoi(arg) : 10;
        pipe_tail(input, n);
    }
    else if (!strcmp(cmd_name, "rev")) {
        // Invertir cada línea
        pipe_rev(input);
    }
    else if (!strcmp(cmd_name, "sort")) {
        // Ordenar líneas (implementación simple)
        pipe_sort(input);
    }
    else {
        printf("Error: Comando '%s' no soporta pipes\n", cmd_name);
    }
}
```

### 2.2 Funciones Especializadas de Pipes

#### Implementación de grep

```c
// Buscar patrón en texto de entrada
static void pipe_grep(const char *input, const char *pattern) {
    if (strlen(pattern) == 0) {
        printf("Error: grep requiere un patrón\n");
        return;
    }
    
    char line[256];
    int input_pos = 0;
    int line_pos = 0;
    
    // Procesar línea por línea
    while (input_pos < strlen(input)) {
        if (input[input_pos] == '\n' || line_pos >= 255) {
            line[line_pos] = '\0';
            
            // Buscar patrón en la línea actual
            if (strstr(line, pattern)) {
                printf("%s\n", line);
            }
            
            line_pos = 0;
        } else {
            line[line_pos++] = input[input_pos];
        }
        input_pos++;
    }
    
    // Procesar última línea si no termina en \n
    if (line_pos > 0) {
        line[line_pos] = '\0';
        if (strstr(line, pattern)) {
            printf("%s\n", line);
        }
    }
}
```

#### Implementación de wc (word count)

```c
// Contar líneas, palabras y caracteres
static void pipe_wc(const char *input) {
    int lines = 0;
    int words = 0;
    int chars = 0;
    int in_word = 0;
    
    for (int i = 0; input[i]; i++) {
        chars++;
        
        if (input[i] == '\n') {
            lines++;
        }
        
        if (input[i] == ' ' || input[i] == '\t' || input[i] == '\n') {
            in_word = 0;
        } else {
            if (!in_word) {
                words++;
                in_word = 1;
            }
        }
    }
    
    printf("  %d  %d  %d\n", lines, words, chars);
}
```

#### Implementación de head/tail

```c
// Mostrar primeras n líneas
static void pipe_head(const char *input, int n) {
    char line[256];
    int input_pos = 0;
    int line_pos = 0;
    int line_count = 0;
    
    while (input_pos < strlen(input) && line_count < n) {
        if (input[input_pos] == '\n' || line_pos >= 255) {
            line[line_pos] = '\0';
            printf("%s\n", line);
            line_count++;
            line_pos = 0;
        } else {
            line[line_pos++] = input[input_pos];
        }
        input_pos++;
    }
    
    // Última línea sin \n
    if (line_pos > 0 && line_count < n) {
        line[line_pos] = '\0';
        printf("%s\n", line);
    }
}

// Mostrar últimas n líneas (implementación simplificada)
static void pipe_tail(const char *input, int n) {
    // Contar total de líneas
    int total_lines = 0;
    for (int i = 0; input[i]; i++) {
        if (input[i] == '\n') total_lines++;
    }
    
    // Si texto no termina en \n, hay una línea más
    if (strlen(input) > 0 && input[strlen(input) - 1] != '\n') {
        total_lines++;
    }
    
    int start_line = (total_lines > n) ? total_lines - n : 0;
    
    // Mostrar desde start_line
    char line[256];
    int input_pos = 0;
    int line_pos = 0;
    int current_line = 0;
    
    while (input_pos < strlen(input)) {
        if (input[input_pos] == '\n' || line_pos >= 255) {
            line[line_pos] = '\0';
            if (current_line >= start_line) {
                printf("%s\n", line);
            }
            current_line++;
            line_pos = 0;
        } else {
            line[line_pos++] = input[input_pos];
        }
        input_pos++;
    }
    
    // Última línea
    if (line_pos > 0 && current_line >= start_line) {
        line[line_pos] = '\0';
        printf("%s\n", line);
    }
}
```

### 2.3 Manejo Avanzado de Entrada

#### Lectura de Línea con Historial

```c
// Lectura de línea con soporte para navegación y historial
static void read_line_with_history(char *buffer, int max_size, 
                                  char **history, int *history_count, 
                                  int *history_pos) {
    int pos = 0;
    int cursor_col = 9; // Después de "shell> "
    char temp_buffer[256];
    
    while (1) {
        unsigned char c = keyboard_getchar();
        
        switch (c) {
            case KEY_UP:
                // Navegar hacia atrás en historial
                if (*history_pos < *history_count - 1) {
                    (*history_pos)++;
                    int hist_idx = *history_count - 1 - *history_pos;
                    
                    // Limpiar línea actual
                    clear_current_line(cursor_col);
                    
                    // Mostrar comando del historial
                    strcpy(buffer, history[hist_idx]);
                    printf("%s", buffer);
                    pos = strlen(buffer);
                    update_cursor(cursor_col + pos, terminal_row);
                }
                break;
                
            case KEY_DOWN:
                // Navegar hacia adelante en historial
                if (*history_pos > 0) {
                    (*history_pos)--;
                    int hist_idx = *history_count - 1 - *history_pos;
                    
                    clear_current_line(cursor_col);
                    strcpy(buffer, history[hist_idx]);
                    printf("%s", buffer);
                    pos = strlen(buffer);
                    update_cursor(cursor_col + pos, terminal_row);
                } else if (*history_pos == 0) {
                    // Volver a línea vacía
                    *history_pos = -1;
                    clear_current_line(cursor_col);
                    buffer[0] = '\0';
                    pos = 0;
                    update_cursor(cursor_col, terminal_row);
                }
                break;
                
            case KEY_LEFT:
                // Mover cursor a la izquierda
                if (pos > 0) {
                    pos--;
                    update_cursor(cursor_col + pos, terminal_row);
                }
                break;
                
            case KEY_RIGHT:
                // Mover cursor a la derecha
                if (pos < strlen(buffer)) {
                    pos++;
                    update_cursor(cursor_col + pos, terminal_row);
                }
                break;
                
            case '\b': // Backspace
                if (pos > 0) {
                    // Eliminar carácter a la izquierda
                    memmove(buffer + pos - 1, buffer + pos, 
                           strlen(buffer) - pos + 1);
                    pos--;
                    
                    // Redibujar línea
                    update_cursor(cursor_col, terminal_row);
                    clear_to_end_of_line();
                    printf("%s", buffer);
                    update_cursor(cursor_col + pos, terminal_row);
                }
                break;
                
            case KEY_DELETE:
                if (pos < strlen(buffer)) {
                    // Eliminar carácter a la derecha
                    memmove(buffer + pos, buffer + pos + 1, 
                           strlen(buffer) - pos);
                    
                    // Redibujar línea
                    update_cursor(cursor_col, terminal_row);
                    clear_to_end_of_line();
                    printf("%s", buffer);
                    update_cursor(cursor_col + pos, terminal_row);
                }
                break;
                
            case '\n':
                // Enter - procesar comando
                buffer[strlen(buffer)] = '\0';
                printf("\n");
                
                // Agregar al historial si no está vacío
                if (strlen(buffer) > 0) {
                    add_to_history(buffer, history, history_count);
                }
                
                *history_pos = -1;
                return;
                
            default:
                // Carácter normal
                if (c >= 32 && c < 127 && pos < max_size - 1) {
                    // Insertar carácter en posición actual
                    memmove(buffer + pos + 1, buffer + pos, 
                           strlen(buffer) - pos + 1);
                    buffer[pos] = c;
                    pos++;
                    
                    // Redibujar línea
                    update_cursor(cursor_col, terminal_row);
                    clear_to_end_of_line();
                    printf("%s", buffer);
                    update_cursor(cursor_col + pos, terminal_row);
                }
                break;
        }
    }
}
```

### 2.4 Funcionalidades Avanzadas del Shell

#### Autocompletado de Comandos

```c
// Lista de comandos disponibles para autocompletado
static const char *available_commands[] = {
    "ls", "cat", "touch", "cp", "mv", "delete", "mkdir", "pwd",
    "copycon", "edln", "delln", "insln", "tee", "stat", "du",
    "wc", "head", "tail", "grep", "hexdump", "file", "sort", "rev",
    "echo", "which", "whoami", "uname", "date", "uptime", "free",
    "history", "cls", "clear", "help", "man", "testpipe",
    NULL
};

// Función de autocompletado
static void autocomplete_command(char *buffer, int *pos) {
    char partial[64];
    int partial_len = 0;
    
    // Extraer palabra parcial
    int start = *pos;
    while (start > 0 && buffer[start - 1] != ' ') start--;
    
    for (int i = start; i < *pos; i++) {
        partial[partial_len++] = buffer[i];
    }
    partial[partial_len] = '\0';
    
    // Buscar coincidencias
    const char *matches[32];
    int match_count = 0;
    
    for (int i = 0; available_commands[i]; i++) {
        if (strncmp(available_commands[i], partial, partial_len) == 0) {
            matches[match_count++] = available_commands[i];
            if (match_count >= 32) break;
        }
    }
    
    if (match_count == 1) {
        // Una sola coincidencia - completar automáticamente
        const char *completion = matches[0] + partial_len;
        int completion_len = strlen(completion);
        
        // Insertar completion en buffer
        memmove(buffer + *pos + completion_len, buffer + *pos,
               strlen(buffer) - *pos + 1);
        memcpy(buffer + *pos, completion, completion_len);
        *pos += completion_len;
        
        // Redibujar línea
        printf("\r\033[K"); // Limpiar línea
        printf("shell> %s", buffer);
    } else if (match_count > 1) {
        // Múltiples coincidencias - mostrar opciones
        printf("\n");
        for (int i = 0; i < match_count; i++) {
            printf("%s  ", matches[i]);
            if ((i + 1) % 8 == 0) printf("\n");
        }
        if (match_count % 8 != 0) printf("\n");
        printf("shell> %s", buffer);
    }
}
```

---

## 📝 Ejercicios Prácticos

### Ejercicio 1: Parser de Comandos Avanzado
**Dificultad**: Intermedia

**Objetivo**: Extender el parser para soportar múltiples pipes y redirección.

**Instrucciones**:
1. Implementa soporte para comandos con múltiples pipes:
```c
// Estructura para pipeline de múltiples comandos
typedef struct pipeline_command {
    char command[128];
    struct pipeline_command *next;
} pipeline_command_t;

void execute_multi_pipe(const char *input) {
    // TODO: Parsear múltiples comandos separados por |
    // TODO: Crear pipeline_command_t para cada comando
    // TODO: Ejecutar pipeline completo con buffers intermedios
}
```

2. Agrega soporte para redirección de salida:
```c
void parse_redirections(const char *cmd, char *clean_cmd, 
                       char *output_file, int *append_mode) {
    // TODO: Detectar > y >> en el comando
    // TODO: Extraer nombre de archivo
    // TODO: Limpiar comando de operadores de redirección
}
```

**Ejemplos a implementar**:
```bash
ls | grep .txt | head 5           # Triple pipe
echo "test" > output.txt          # Redirección simple
cat file.txt >> log.txt          # Redirección append
ls | grep error > errors.txt     # Pipe + redirección
```

---

### Ejercicio 2: Variables de Entorno
**Dificultad**: Avanzada

**Objetivo**: Implementar sistema básico de variables de entorno.

**Instrucciones**:
1. Define estructura para variables:
```c
#define MAX_VARIABLES 32
#define MAX_VAR_NAME 32
#define MAX_VAR_VALUE 128

typedef struct {
    char name[MAX_VAR_NAME];
    char value[MAX_VAR_VALUE];
} env_variable_t;

static env_variable_t env_vars[MAX_VARIABLES];
static int env_count = 0;
```

2. Implementa comandos de gestión:
```c
void cmd_set(const char *args) {
    // TODO: Parsear "VAR=valor"
    // TODO: Agregar o actualizar variable
}

void cmd_unset(const char *args) {
    // TODO: Eliminar variable por nombre
}

void cmd_env(void) {
    // TODO: Mostrar todas las variables
}
```

3. Expande variables en comandos:
```c
void expand_variables(const char *input, char *output, int max_size) {
    // TODO: Buscar $VAR o ${VAR} en input
    // TODO: Reemplazar con valor de variable
    // TODO: Escribir resultado en output
}
```

**Ejemplos de uso**:
```bash
set PATH=/bin:/usr/bin
set HOME=/root
echo $PATH                    # Muestra: /bin:/usr/bin
echo "User: $USER"           # Expansión en strings
ls $HOME                     # Expansión en argumentos
```

---

### Ejercicio 3: Job Control Básico
**Dificultad**: Muy Avanzada

**Objetivo**: Implementar control de jobs con procesos en background.

**Instrucciones**:
1. Define estructura de jobs:
```c
typedef struct job {
    int job_id;
    int pid;
    char command[256];
    enum { JOB_RUNNING, JOB_STOPPED, JOB_DONE } status;
    int background;
    struct job *next;
} job_t;

static job_t *job_list = NULL;
static int next_job_id = 1;
```

2. Implementa gestión de jobs:
```c
int start_background_job(const char *command) {
    // TODO: Crear nuevo proceso (fork simulado)
    // TODO: Agregar job a la lista
    // TODO: Retornar job ID
}

void cmd_jobs(void) {
    // TODO: Mostrar lista de jobs activos
    // [1]+ Running    long_command &
    // [2]- Stopped   editor file.txt
}

void cmd_fg(const char *args) {
    // TODO: Traer job a foreground
    // TODO: Esperar a que termine
}

void cmd_bg(const char *args) {
    // TODO: Continuar job en background
}
```

3. Manejo de señales básico:
```c
void handle_sigchld(int sig) {
    // TODO: Recolectar estado de procesos terminados
    // TODO: Actualizar status de jobs
}

void handle_sigtstp(int sig) {
    // TODO: Suspender proceso en foreground
    // TODO: Marcar job como STOPPED
}
```

**Funcionalidad esperada**:
```bash
long_command &               # Ejecutar en background
# [1] 1234
jobs                        # Listar jobs
# [1]+ Running    long_command &
fg 1                        # Traer a foreground
# Ctrl+Z suspende
bg 1                        # Continuar en background
```

---

### Ejercicio 4: Sistema de Aliases
**Dificultad**: Extrema

**Objetivo**: Implementar sistema de aliases para comandos.

**Instrucciones**:
1. Estructura de aliases:
```c
#define MAX_ALIASES 64

typedef struct alias {
    char name[32];
    char expansion[256];
    struct alias *next;
} alias_t;

static alias_t *alias_list = NULL;
```

2. Comandos de gestión:
```c
void cmd_alias(const char *args) {
    if (strlen(args) == 0) {
        // TODO: Mostrar todos los aliases
        show_all_aliases();
    } else {
        // TODO: Parsear "alias_name='expansion'"
        // TODO: Agregar nuevo alias
        add_alias(args);
    }
}

void cmd_unalias(const char *args) {
    // TODO: Eliminar alias por nombre
}
```

3. Expansión de aliases:
```c
int expand_alias(const char *command, char *expanded, int max_size) {
    // TODO: Verificar si primer word es un alias
    // TODO: Expandir alias preservando argumentos
    // TODO: Manejar aliases recursivos (con límite)
    // TODO: Retornar 1 si se expandió, 0 si no
}
```

4. Funcionalidades avanzadas:
```c
// Alias con parámetros
void expand_parameterized_alias(const char *alias_def, 
                               const char *args, 
                               char *result) {
    // TODO: Soportar $1, $2, $@ en definición de alias
    // TODO: Reemplazar parámetros con argumentos
}

// Detección de recursión
int check_alias_recursion(const char *alias_name, 
                         char *expansion_stack[], 
                         int stack_depth) {
    // TODO: Verificar si alias_name ya está en stack
    // TODO: Prevenir loops infinitos
}
```

**Ejemplos avanzados**:
```bash
alias ll='ls -l'
alias la='ls -la'
alias grep='grep --color'
alias ..='cd ..'
alias ...='cd ../..'

# Alias con parámetros (avanzado)
alias mkcd='mkdir $1 && cd $1'  # mkcd newdir

# Alias anidados
alias lll='ll | head'           # Usa alias 'll'
```

---

## 📋 Resumen del Capítulo

### Conceptos Clave Aprendidos

1. **Arquitectura de Shells**:
   - Parsing de comandos complejos con pipes y redirección
   - Construcción de Abstract Syntax Trees (AST)
   - Manejo de historial y navegación interactiva

2. **Comunicación Inter-Proceso**:
   - Implementación de pipes como mecanismo IPC básico
   - Patrones Producer-Consumer en sistemas de pipes
   - Sincronización y manejo de buffers

3. **Procesamiento de Texto**:
   - Implementación de herramientas tipo UNIX (grep, wc, head, tail)
   - Algoritmos eficientes para filtrado y transformación
   - Manejo de streams de datos variables

4. **Funcionalidades Avanzadas de Shell**:
   - Variables de entorno y expansión
   - Job control y procesos en background
   - Sistema de aliases y autocompletado

### Habilidades Técnicas Desarrolladas

- ✅ **Diseño** de parsers para lenguajes de comando
- ✅ **Implementación** de sistemas IPC eficientes
- ✅ **Algoritmos** de procesamiento de texto en tiempo real
- ✅ **Concurrencia** en sistemas de comunicación
- ✅ **Interfaz de usuario** avanzada para sistemas de bajo nivel

### Conexión con el Código del Mini-Kernel

En nuestro mini-kernel educativo, estos conceptos se implementan directamente:

- **Líneas 1750-1900**: Sistema completo de pipes con múltiples comandos
- **Líneas 1400-1600**: Funciones especializadas para procesamiento de texto
- **Líneas 1200-1400**: Manejo avanzado de entrada con historial y navegación
- **Shell principal**: Integración de todos los componentes IPC

### Impacto en el Desarrollo de Sistemas

La comprensión profunda de shells e IPC es fundamental para:
- **Desarrollo de herramientas**: Crear aplicaciones que se integren bien con sistemas UNIX
- **Automatización**: Diseñar scripts y pipelines eficientes
- **Sistemas distribuidos**: Entender patrones de comunicación escalables
- **Administración de sistemas**: Optimizar workflows y procesos

### Preparación para Proyectos Avanzados

Los ejercicios de este capítulo preparan para:
- Desarrollo de shells completos tipo Bash/Zsh
- Implementación de sistemas de comunicación distribuida
- Diseño de APIs y protocolos de comunicación
- Optimización de rendimiento en sistemas concurrentes

### Extensiones Futuras

Con estos fundamentos, los estudiantes pueden explorar:
- **Shells modernos**: Fish, Zsh con características avanzadas
- **Sistemas de orquestación**: Docker, Kubernetes
- **Message queues**: RabbitMQ, Apache Kafka
- **Microservicios**: Patrones de comunicación distribuida

---

**📚 Lecturas Complementarias:**
- "Advanced Programming in the UNIX Environment" por W. Richard Stevens
- "The Art of UNIX Programming" por Eric S. Raymond
- "Unix Shell Programming" por Kochan y Wood
- "Modern Operating Systems" - Capítulo de IPC

**🎯 ¡Has completado el recorrido completo por los fundamentos de sistemas operativos!**

Este capítulo cierra una formación integral que abarca desde el arranque de hardware hasta la comunicación inter-proceso avanzada. Los estudiantes ahora poseen las herramientas conceptuales y prácticas para abordar cualquier sistema operativo moderno con confianza y comprensión profunda de sus mecanismos internos.