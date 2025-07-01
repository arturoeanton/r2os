; bootsect.asm — 512 bytes, firma 0x55AA
[BITS 16]
[ORG 0x7C00]

cli
xor ax, ax
mov ds, ax
mov es, ax

; Debug: imprimir “A” con INT 10h, función 0Eh
mov ah, 0x0E
mov al, 'A'
int 0x10

; Leer 11 sectores (tu kernel de 4439 B)
mov bx, 0x8000         ; destino en 0x0000:0x8000
mov cx, 11             ; número de sectores a leer
read_loop:
    cmp cx, 0
    je  loaded

    mov ah, 0x02       ; INT 13h, función 02h: leer sector
    mov al, 1          ; leer 1 sector
    xor ch, ch         ; cilindro = 0
    mov cl, 2          ; sector = 2 (sector 1 = este bootsector)
    xor dh, dh         ; cabeza = 0
    xor dl, dl         ; unidad = floppy A:
    int 0x13
    jc  disk_error     ; si CF=1, error

    add bx, 512        ; avanza puntero destino
    dec cx
    jmp read_loop

loaded:
    sti                ; rehabilitar interrupciones
    jmp 0x0000:0x8000  ; salto al kernel cargado

disk_error:
    ; imprimir “E” en caso de error
    mov ah, 0x0E
    mov al, 'E'
    int 0x10
    hlt
    jmp $

; Relleno hasta 510 bytes
times 510-($-$$) db 0

; Firma BIOS al final del sector
db 0x55, 0xAA