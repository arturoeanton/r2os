# Capítulo 7: Redes y Protocolos

## 📚 Introducción

Este capítulo explora la implementación de capacidades de red en sistemas operativos, desde los conceptos fundamentales de networking hasta la implementación práctica de protocolos de comunicación. Aprenderemos cómo los sistemas operativos gestionan la comunicación entre dispositivos a través de redes locales e internet.

## 🎯 Objetivos de Aprendizaje

Al completar este capítulo, el estudiante será capaz de:

1. **Comprender** la arquitectura de red en capas (OSI/TCP-IP)
2. **Implementar** drivers básicos de red (Ethernet)
3. **Desarrollar** protocolos de comunicación simples
4. **Crear** sockets y APIs de red
5. **Optimizar** el rendimiento de comunicaciones de red

---

## 🏛️ Parte Teórica

### 1.1 Fundamentos de Redes

#### Modelo OSI y TCP/IP

```
┌─────────────────────────────────────────────────────────────┐
│                    MODELO OSI vs TCP/IP                     │
│                                                             │
│    OSI (7 capas)              TCP/IP (4 capas)             │
│  ┌─────────────────┐         ┌─────────────────┐            │
│  │   Aplicación    │         │   Aplicación    │            │
│  ├─────────────────┤         │                 │            │
│  │  Presentación   │         │  (HTTP, FTP,    │            │
│  ├─────────────────┤         │   DNS, SMTP)    │            │
│  │     Sesión      │         │                 │            │
│  ├─────────────────┤         ├─────────────────┤            │
│  │   Transporte    │         │   Transporte    │            │
│  │  (TCP/UDP)      │         │  (TCP/UDP)      │            │
│  ├─────────────────┤         ├─────────────────┤            │
│  │      Red        │         │    Internet     │            │
│  │    (IP)         │         │    (IP)         │            │
│  ├─────────────────┤         ├─────────────────┤            │
│  │  Enlace Datos   │         │   Acceso Red    │            │
│  ├─────────────────┤         │                 │            │
│  │     Física      │         │  (Ethernet,     │            │
│  │                 │         │   WiFi)         │            │
│  └─────────────────┘         └─────────────────┘            │
└─────────────────────────────────────────────────────────────┘
```

#### Protocolo Ethernet

```
┌─────────────────────────────────────────────────────────────┐
│                    FRAME ETHERNET                           │
│                                                             │
│  ┌─────────┬─────────┬─────────┬─────────┬─────────┬─────┐  │
│  │Preámbulo│ MAC     │ MAC     │ Tipo/   │ Datos   │ FCS │  │
│  │ (8)     │ Destino │ Origen  │ Long.   │(46-1500)│ (4) │  │
│  │         │ (6)     │ (6)     │ (2)     │         │     │  │
│  └─────────┴─────────┴─────────┴─────────┴─────────┴─────┘  │
│                                                             │
│  Campos (bytes):                                            │
│  - Preámbulo: Sincronización                               │
│  - MAC Destino: Dirección Ethernet destino                 │
│  - MAC Origen: Dirección Ethernet origen                   │
│  - Tipo/Longitud: Protocolo superior o tamaño             │
│  - Datos: Payload del protocolo superior                   │
│  - FCS: Frame Check Sequence (CRC32)                       │
└─────────────────────────────────────────────────────────────┘
```

#### Protocolo IP (Internet Protocol)

```
┌─────────────────────────────────────────────────────────────┐
│                    HEADER IP (IPv4)                         │
│                                                             │
│  0                   1                   2                   3
│  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
│ ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
│ │Ver│IHL│   TOS   │         Total Length          │
│ ├─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┤
│ │         Identification          │Flags│  Fragment Offset  │
│ ├─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┤
│ │   TTL   │  Protocol │         Header Checksum   │
│ ├─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┤
│ │                    Source Address                         │
│ ├─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┤
│ │                  Destination Address                      │
│ ├─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┤
│ │                    Options                    │  Padding  │
│ └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
│                                                             │
│  Campos importantes:                                        │
│  - Version: 4 (IPv4)                                       │
│  - IHL: Internet Header Length                             │
│  - Protocol: 1=ICMP, 6=TCP, 17=UDP                        │
│  - TTL: Time To Live                                       │
│  - Source/Dest Address: Direcciones IP                     │
└─────────────────────────────────────────────────────────────┘
```

### 1.2 Arquitectura de Red en el Kernel

#### Stack de Protocolos

```
┌─────────────────────────────────────────────────────────────┐
│                ARQUITECTURA DE RED EN KERNEL                │
│                                                             │
│  ┌─────────────────────────────────────────────────────────┐ │
│  │            APLICACIONES DE USUARIO                      │ │
│  │    (Web Browser, Email, Games, etc.)                   │ │
│  └─────────────────────────────────────────────────────────┘ │
│                              │                               │
│                              ▼                               │
│  ┌─────────────────────────────────────────────────────────┐ │
│  │                    SOCKET API                           │ │
│  │  socket(), bind(), listen(), accept(), send(), recv()  │ │
│  └─────────────────────────────────────────────────────────┘ │
│                              │                               │
│                              ▼                               │
│  ┌─────────────────────────────────────────────────────────┐ │
│  │               CAPA DE TRANSPORTE                        │ │
│  │            TCP                    UDP                   │ │
│  │  ┌─────────────────┐    ┌─────────────────┐            │ │
│  │  │ Control Flujo   │    │   Datagrama     │            │ │
│  │  │ Confiabilidad   │    │   Simple        │            │ │
│  │  │ Orden           │    │   Rápido        │            │ │
│  │  └─────────────────┘    └─────────────────┘            │ │
│  └─────────────────────────────────────────────────────────┘ │
│                              │                               │
│                              ▼                               │
│  ┌─────────────────────────────────────────────────────────┐ │
│  │                  CAPA DE RED (IP)                       │ │
│  │  • Routing                                              │ │
│  │  • Fragmentación                                        │ │
│  │  • Forwarding                                           │ │
│  └─────────────────────────────────────────────────────────┘ │
│                              │                               │
│                              ▼                               │
│  ┌─────────────────────────────────────────────────────────┐ │
│  │                 CAPA DE ENLACE                          │ │
│  │            Ethernet Driver                              │ │
│  │  ┌─────────────────┐    ┌─────────────────┐            │ │
│  │  │  Send Queue     │    │  Recv Queue     │            │ │
│  │  │                 │    │                 │            │ │
│  │  └─────────────────┘    └─────────────────┘            │ │
│  └─────────────────────────────────────────────────────────┘ │
│                              │                               │
│                              ▼                               │
│  ┌─────────────────────────────────────────────────────────┐ │
│  │                HARDWARE DE RED                          │ │
│  │          Network Interface Card (NIC)                  │ │
│  └─────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────┘
```

### 1.3 Protocolos de Transporte

#### TCP (Transmission Control Protocol)

```c
// Estructura del header TCP
typedef struct {
    uint16_t source_port;      // Puerto origen
    uint16_t dest_port;        // Puerto destino
    uint32_t seq_number;       // Número de secuencia
    uint32_t ack_number;       // Número de acknowledgment
    uint8_t  header_length;    // Longitud del header (4 bits)
    uint8_t  flags;            // Flags TCP
    uint16_t window_size;      // Tamaño de ventana
    uint16_t checksum;         // Checksum
    uint16_t urgent_pointer;   // Puntero urgente
    uint8_t  options[];        // Opciones TCP
} __attribute__((packed)) tcp_header_t;

// Flags TCP
#define TCP_FIN  0x01    // Finalizar conexión
#define TCP_SYN  0x02    // Sincronizar números de secuencia
#define TCP_RST  0x04    // Reset conexión
#define TCP_PSH  0x08    // Push datos
#define TCP_ACK  0x10    // Acknowledgment válido
#define TCP_URG  0x20    // Datos urgentes
```

#### UDP (User Datagram Protocol)

```c
// Estructura del header UDP (más simple)
typedef struct {
    uint16_t source_port;      // Puerto origen
    uint16_t dest_port;        // Puerto destino
    uint16_t length;           // Longitud del datagrama
    uint16_t checksum;         // Checksum
} __attribute__((packed)) udp_header_t;
```

---

## 🔧 Parte Práctica

### 2.1 Driver de Red Básico

#### Estructura del Driver de Red

```c
// network_driver.h
#ifndef NETWORK_DRIVER_H
#define NETWORK_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

// Estructura de un packet de red
typedef struct {
    uint8_t *data;             // Datos del packet
    uint32_t length;           // Tamaño del packet
    uint32_t timestamp;        // Timestamp de recepción
    struct packet *next;       // Siguiente packet en cola
} network_packet_t;

// Estructura de la interfaz de red
typedef struct {
    char name[16];             // Nombre de la interfaz (eth0, wlan0)
    uint8_t mac_address[6];    // Dirección MAC
    uint32_t ip_address;       // Dirección IP
    uint32_t netmask;          // Máscara de subred
    uint32_t gateway;          // Gateway por defecto
    
    // Estadísticas
    uint32_t packets_sent;
    uint32_t packets_received;
    uint32_t bytes_sent;
    uint32_t bytes_received;
    uint32_t errors;
    
    // Colas de packets
    network_packet_t *send_queue;
    network_packet_t *recv_queue;
    
    // Funciones del driver
    int (*send)(struct network_interface *iface, network_packet_t *packet);
    int (*receive)(struct network_interface *iface);
    int (*configure)(struct network_interface *iface);
} network_interface_t;

// Funciones del driver
int network_driver_init(void);
int network_interface_register(network_interface_t *iface);
int network_send_packet(network_interface_t *iface, network_packet_t *packet);
network_packet_t *network_receive_packet(network_interface_t *iface);

#endif
```

#### Implementación del Driver

```c
// network_driver.c
#include "network_driver.h"
#include "memory.h"
#include "interrupts.h"
#include "kernel.h"

static network_interface_t *network_interfaces[MAX_INTERFACES];
static int num_interfaces = 0;

int network_driver_init(void) {
    printf("Inicializando driver de red...\n");
    
    // Limpiar array de interfaces
    for (int i = 0; i < MAX_INTERFACES; i++) {
        network_interfaces[i] = NULL;
    }
    
    // Inicializar subsistema de red
    protocol_stack_init();
    
    printf("Driver de red inicializado\n");
    return 0;
}

int network_interface_register(network_interface_t *iface) {
    if (num_interfaces >= MAX_INTERFACES) {
        printf("Error: Máximo número de interfaces alcanzado\n");
        return -1;
    }
    
    network_interfaces[num_interfaces] = iface;
    num_interfaces++;
    
    printf("Interfaz de red registrada: %s\n", iface->name);
    printf("  MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
           iface->mac_address[0], iface->mac_address[1],
           iface->mac_address[2], iface->mac_address[3],
           iface->mac_address[4], iface->mac_address[5]);
    
    return 0;
}

int network_send_packet(network_interface_t *iface, network_packet_t *packet) {
    if (!iface || !packet) {
        return -1;
    }
    
    // Agregar packet a cola de envío
    add_to_send_queue(iface, packet);
    
    // Actualizar estadísticas
    iface->packets_sent++;
    iface->bytes_sent += packet->length;
    
    // Llamar función de envío del driver específico
    return iface->send(iface, packet);
}

network_packet_t *network_receive_packet(network_interface_t *iface) {
    if (!iface) {
        return NULL;
    }
    
    network_packet_t *packet = remove_from_recv_queue(iface);
    
    if (packet) {
        // Actualizar estadísticas
        iface->packets_received++;
        iface->bytes_received += packet->length;
        
        // Procesar packet en el stack de protocolos
        protocol_stack_process(packet);
    }
    
    return packet;
}
```

### 2.2 Implementación de Protocolos

#### Protocolo Ethernet

```c
// ethernet.c
#include "ethernet.h"
#include "network_driver.h"

#define ETHERTYPE_IP    0x0800
#define ETHERTYPE_ARP   0x0806

typedef struct {
    uint8_t dest_mac[6];       // MAC destino
    uint8_t src_mac[6];        // MAC origen
    uint16_t ethertype;        // Tipo de protocolo
    uint8_t payload[];         // Datos
} __attribute__((packed)) ethernet_header_t;

int ethernet_send(network_interface_t *iface, uint8_t *dest_mac, 
                  uint16_t ethertype, uint8_t *data, uint32_t length) {
    
    // Crear packet Ethernet
    uint32_t packet_size = sizeof(ethernet_header_t) + length;
    network_packet_t *packet = kmalloc(sizeof(network_packet_t));
    packet->data = kmalloc(packet_size);
    packet->length = packet_size;
    
    ethernet_header_t *eth_header = (ethernet_header_t *)packet->data;
    
    // Llenar header Ethernet
    memcpy(eth_header->dest_mac, dest_mac, 6);
    memcpy(eth_header->src_mac, iface->mac_address, 6);
    eth_header->ethertype = htons(ethertype);
    
    // Copiar datos
    memcpy(eth_header->payload, data, length);
    
    // Enviar packet
    return network_send_packet(iface, packet);
}

int ethernet_receive(network_packet_t *packet) {
    if (packet->length < sizeof(ethernet_header_t)) {
        printf("Error: Packet Ethernet demasiado pequeño\n");
        return -1;
    }
    
    ethernet_header_t *eth_header = (ethernet_header_t *)packet->data;
    uint16_t ethertype = ntohs(eth_header->ethertype);
    
    printf("Ethernet recibido: tipo=0x%04x, tamaño=%d\n", 
           ethertype, packet->length);
    
    // Procesar según el tipo
    switch (ethertype) {
    case ETHERTYPE_IP:
        return ip_receive(eth_header->payload, 
                         packet->length - sizeof(ethernet_header_t));
    
    case ETHERTYPE_ARP:
        return arp_receive(eth_header->payload,
                          packet->length - sizeof(ethernet_header_t));
    
    default:
        printf("Tipo Ethernet desconocido: 0x%04x\n", ethertype);
        return -1;
    }
}
```

#### Protocolo IP

```c
// ip.c
#include "ip.h"
#include "ethernet.h"

#define IP_VERSION_4    4
#define IP_HEADER_LEN   5

typedef struct {
    uint8_t version_ihl;       // Version (4 bits) + IHL (4 bits)
    uint8_t tos;               // Type of Service
    uint16_t total_length;     // Longitud total
    uint16_t identification;   // Identificación
    uint16_t flags_fragment;   // Flags (3 bits) + Fragment Offset (13 bits)
    uint8_t ttl;               // Time To Live
    uint8_t protocol;          // Protocolo siguiente
    uint16_t checksum;         // Checksum del header
    uint32_t src_ip;           // IP origen
    uint32_t dest_ip;          // IP destino
    uint8_t options[];         // Opciones IP
} __attribute__((packed)) ip_header_t;

static uint16_t ip_id_counter = 1;

int ip_send(network_interface_t *iface, uint32_t dest_ip, uint8_t protocol,
            uint8_t *data, uint32_t length) {
    
    // Crear header IP
    ip_header_t ip_header;
    memset(&ip_header, 0, sizeof(ip_header_t));
    
    ip_header.version_ihl = (IP_VERSION_4 << 4) | IP_HEADER_LEN;
    ip_header.tos = 0;
    ip_header.total_length = htons(sizeof(ip_header_t) + length);
    ip_header.identification = htons(ip_id_counter++);
    ip_header.flags_fragment = htons(0x4000);  // Don't fragment
    ip_header.ttl = 64;
    ip_header.protocol = protocol;
    ip_header.checksum = 0;  // Calcular después
    ip_header.src_ip = htonl(iface->ip_address);
    ip_header.dest_ip = htonl(dest_ip);
    
    // Calcular checksum
    ip_header.checksum = ip_checksum(&ip_header, sizeof(ip_header_t));
    
    // Crear packet completo
    uint32_t packet_size = sizeof(ip_header_t) + length;
    uint8_t *packet_data = kmalloc(packet_size);
    
    memcpy(packet_data, &ip_header, sizeof(ip_header_t));
    memcpy(packet_data + sizeof(ip_header_t), data, length);
    
    // Resolver MAC destino (ARP)
    uint8_t dest_mac[6];
    if (arp_resolve(dest_ip, dest_mac) != 0) {
        printf("Error: No se pudo resolver MAC para IP %d.%d.%d.%d\n",
               (dest_ip >> 24) & 0xFF, (dest_ip >> 16) & 0xFF,
               (dest_ip >> 8) & 0xFF, dest_ip & 0xFF);
        kfree(packet_data);
        return -1;
    }
    
    // Enviar vía Ethernet
    int result = ethernet_send(iface, dest_mac, ETHERTYPE_IP, 
                              packet_data, packet_size);
    
    kfree(packet_data);
    return result;
}

int ip_receive(uint8_t *data, uint32_t length) {
    if (length < sizeof(ip_header_t)) {
        printf("Error: Packet IP demasiado pequeño\n");
        return -1;
    }
    
    ip_header_t *ip_header = (ip_header_t *)data;
    
    // Verificar versión
    uint8_t version = (ip_header->version_ihl >> 4) & 0xF;
    if (version != IP_VERSION_4) {
        printf("Error: Versión IP no soportada: %d\n", version);
        return -1;
    }
    
    // Verificar checksum
    uint16_t received_checksum = ip_header->checksum;
    ip_header->checksum = 0;
    uint16_t calculated_checksum = ip_checksum(ip_header, sizeof(ip_header_t));
    
    if (received_checksum != calculated_checksum) {
        printf("Error: Checksum IP inválido\n");
        return -1;
    }
    
    // Procesar según protocolo
    uint8_t protocol = ip_header->protocol;
    uint32_t payload_length = ntohs(ip_header->total_length) - sizeof(ip_header_t);
    uint8_t *payload = data + sizeof(ip_header_t);
    
    printf("IP recibido: protocolo=%d, tamaño=%d\n", protocol, payload_length);
    
    switch (protocol) {
    case 1:  // ICMP
        return icmp_receive(payload, payload_length);
    
    case 6:  // TCP
        return tcp_receive(payload, payload_length);
    
    case 17: // UDP
        return udp_receive(payload, payload_length);
    
    default:
        printf("Protocolo IP desconocido: %d\n", protocol);
        return -1;
    }
}

uint16_t ip_checksum(void *data, uint32_t length) {
    uint16_t *buf = (uint16_t *)data;
    uint32_t sum = 0;
    
    // Sumar todas las palabras de 16 bits
    while (length > 1) {
        sum += *buf++;
        length -= 2;
    }
    
    // Agregar byte final si existe
    if (length == 1) {
        sum += *(uint8_t *)buf;
    }
    
    // Doblar carry
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }
    
    // Complemento a uno
    return ~sum;
}
```

### 2.3 Protocolo UDP

```c
// udp.c
#include "udp.h"
#include "ip.h"

typedef struct {
    uint16_t src_port;         // Puerto origen
    uint16_t dest_port;        // Puerto destino
    uint16_t length;           // Longitud del datagrama
    uint16_t checksum;         // Checksum
} __attribute__((packed)) udp_header_t;

// Tabla de puertos UDP
typedef struct {
    uint16_t port;
    void (*handler)(uint8_t *data, uint32_t length, uint32_t src_ip, uint16_t src_port);
} udp_port_handler_t;

static udp_port_handler_t udp_handlers[MAX_UDP_HANDLERS];
static int num_udp_handlers = 0;

int udp_send(network_interface_t *iface, uint32_t dest_ip, uint16_t src_port,
             uint16_t dest_port, uint8_t *data, uint32_t length) {
    
    // Crear header UDP
    udp_header_t udp_header;
    udp_header.src_port = htons(src_port);
    udp_header.dest_port = htons(dest_port);
    udp_header.length = htons(sizeof(udp_header_t) + length);
    udp_header.checksum = 0;  // Opcional en IPv4
    
    // Crear datagrama completo
    uint32_t packet_size = sizeof(udp_header_t) + length;
    uint8_t *packet_data = kmalloc(packet_size);
    
    memcpy(packet_data, &udp_header, sizeof(udp_header_t));
    memcpy(packet_data + sizeof(udp_header_t), data, length);
    
    // Enviar vía IP
    int result = ip_send(iface, dest_ip, 17, packet_data, packet_size);
    
    kfree(packet_data);
    return result;
}

int udp_receive(uint8_t *data, uint32_t length) {
    if (length < sizeof(udp_header_t)) {
        printf("Error: Datagrama UDP demasiado pequeño\n");
        return -1;
    }
    
    udp_header_t *udp_header = (udp_header_t *)data;
    
    uint16_t src_port = ntohs(udp_header->src_port);
    uint16_t dest_port = ntohs(udp_header->dest_port);
    uint16_t udp_length = ntohs(udp_header->length);
    
    if (udp_length != length) {
        printf("Error: Longitud UDP incorrecta\n");
        return -1;
    }
    
    printf("UDP recibido: puerto %d -> %d, tamaño=%d\n", 
           src_port, dest_port, udp_length);
    
    // Buscar handler para el puerto destino
    for (int i = 0; i < num_udp_handlers; i++) {
        if (udp_handlers[i].port == dest_port) {
            uint8_t *payload = data + sizeof(udp_header_t);
            uint32_t payload_length = udp_length - sizeof(udp_header_t);
            
            udp_handlers[i].handler(payload, payload_length, 0, src_port);
            return 0;
        }
    }
    
    printf("No hay handler para puerto UDP %d\n", dest_port);
    return -1;
}

int udp_bind(uint16_t port, void (*handler)(uint8_t *, uint32_t, uint32_t, uint16_t)) {
    if (num_udp_handlers >= MAX_UDP_HANDLERS) {
        printf("Error: Máximo número de handlers UDP alcanzado\n");
        return -1;
    }
    
    udp_handlers[num_udp_handlers].port = port;
    udp_handlers[num_udp_handlers].handler = handler;
    num_udp_handlers++;
    
    printf("UDP handler registrado para puerto %d\n", port);
    return 0;
}
```

### 2.4 Protocolo ICMP (Ping)

```c
// icmp.c
#include "icmp.h"
#include "ip.h"

#define ICMP_ECHO_REQUEST   8
#define ICMP_ECHO_REPLY     0

typedef struct {
    uint8_t type;              // Tipo de mensaje ICMP
    uint8_t code;              // Código
    uint16_t checksum;         // Checksum
    uint16_t identifier;       // Identificador
    uint16_t sequence;         // Número de secuencia
    uint8_t data[];            // Datos
} __attribute__((packed)) icmp_header_t;

int icmp_send_ping(network_interface_t *iface, uint32_t dest_ip) {
    // Crear packet ICMP Echo Request
    icmp_header_t icmp_header;
    icmp_header.type = ICMP_ECHO_REQUEST;
    icmp_header.code = 0;
    icmp_header.checksum = 0;
    icmp_header.identifier = htons(getpid());
    icmp_header.sequence = htons(1);
    
    // Datos de prueba
    char test_data[] = "Hello, ICMP!";
    
    // Crear packet completo
    uint32_t packet_size = sizeof(icmp_header_t) + strlen(test_data);
    uint8_t *packet_data = kmalloc(packet_size);
    
    memcpy(packet_data, &icmp_header, sizeof(icmp_header_t));
    memcpy(packet_data + sizeof(icmp_header_t), test_data, strlen(test_data));
    
    // Calcular checksum
    icmp_header_t *header = (icmp_header_t *)packet_data;
    header->checksum = ip_checksum(packet_data, packet_size);
    
    printf("Enviando ping a %d.%d.%d.%d\n",
           (dest_ip >> 24) & 0xFF, (dest_ip >> 16) & 0xFF,
           (dest_ip >> 8) & 0xFF, dest_ip & 0xFF);
    
    // Enviar vía IP
    int result = ip_send(iface, dest_ip, 1, packet_data, packet_size);
    
    kfree(packet_data);
    return result;
}

int icmp_receive(uint8_t *data, uint32_t length) {
    if (length < sizeof(icmp_header_t)) {
        printf("Error: Packet ICMP demasiado pequeño\n");
        return -1;
    }
    
    icmp_header_t *icmp_header = (icmp_header_t *)data;
    
    uint8_t type = icmp_header->type;
    uint8_t code = icmp_header->code;
    
    printf("ICMP recibido: tipo=%d, código=%d\n", type, code);
    
    switch (type) {
    case ICMP_ECHO_REQUEST:
        // Responder con Echo Reply
        return icmp_send_reply(data, length);
    
    case ICMP_ECHO_REPLY:
        printf("Ping reply recibido!\n");
        return 0;
    
    default:
        printf("Tipo ICMP no manejado: %d\n", type);
        return -1;
    }
}

int icmp_send_reply(uint8_t *request_data, uint32_t length) {
    // Crear reply cambiando tipo
    uint8_t *reply_data = kmalloc(length);
    memcpy(reply_data, request_data, length);
    
    icmp_header_t *header = (icmp_header_t *)reply_data;
    header->type = ICMP_ECHO_REPLY;
    header->checksum = 0;
    
    // Recalcular checksum
    header->checksum = ip_checksum(reply_data, length);
    
    // Enviar reply (necesitaríamos IP origen del request)
    // Implementación simplificada
    printf("Enviando ICMP Echo Reply\n");
    
    kfree(reply_data);
    return 0;
}
```

---

## 🧪 Ejercicios Prácticos

### Ejercicio 1: Sniffer de Red (Nivel: Intermedio)

**Objetivo**: Implementar un sniffer que capture y analice paquetes de red.

**Instrucciones**:
1. Interceptar todos los paquetes de red
2. Analizar headers Ethernet, IP, TCP/UDP
3. Mostrar información relevante
4. Implementar filtros por protocolo

**Código Base**:
```c
// Completar la implementación
void network_sniffer_demo(void) {
    printf("=== DEMO: Network Sniffer ===\n");
    
    // TODO: Configurar modo promiscuo
    // TODO: Interceptar paquetes
    // TODO: Analizar headers
    // TODO: Mostrar estadísticas
    
    printf("=== FIN DEMO ===\n");
}

// Función para analizar paquetes
void analyze_packet(network_packet_t *packet) {
    // TODO: Implementar análisis
    // - Ethernet header
    // - IP header
    // - TCP/UDP header
    // - Datos
}
```

### Ejercicio 2: Servidor Echo UDP (Nivel: Avanzado)

**Objetivo**: Implementar un servidor echo que responda a paquetes UDP.

**Instrucciones**:
1. Bind a puerto UDP específico
2. Recibir paquetes UDP
3. Responder con los mismos datos
4. Manejar múltiples clientes

**Código Base**:
```c
// Servidor Echo UDP
void udp_echo_server(uint8_t *data, uint32_t length, 
                     uint32_t src_ip, uint16_t src_port) {
    printf("Echo server recibió %d bytes desde puerto %d\n", length, src_port);
    
    // TODO: Enviar respuesta con los mismos datos
    // TODO: Manejar errores
}

void start_udp_echo_server(void) {
    printf("Iniciando servidor Echo UDP en puerto 7\n");
    
    // TODO: Bind handler al puerto 7
    // TODO: Configurar interfaz de red
}
```

### Ejercicio 3: Cliente Ping (Nivel: Avanzado)

**Objetivo**: Implementar un cliente ping completo con estadísticas.

**Instrucciones**:
1. Enviar paquetes ICMP Echo Request
2. Medir tiempo de respuesta
3. Calcular estadísticas (min, max, avg, pérdida)
4. Implementar timeout

**Código Base**:
```c
typedef struct {
    uint32_t packets_sent;
    uint32_t packets_received;
    uint32_t min_time;
    uint32_t max_time;
    uint32_t total_time;
} ping_stats_t;

// TODO: Implementar cliente ping completo
int ping_host(uint32_t dest_ip, int count) {
    ping_stats_t stats = {0};
    
    for (int i = 0; i < count; i++) {
        // TODO: Enviar ping
        // TODO: Esperar respuesta con timeout
        // TODO: Actualizar estadísticas
    }
    
    // TODO: Mostrar estadísticas finales
    return 0;
}
```

### Ejercicio 4: Stack TCP Básico (Nivel: Extremo)

**Objetivo**: Implementar un stack TCP básico con handshake y transferencia.

**Instrucciones**:
1. Implementar 3-way handshake
2. Gestionar números de secuencia
3. Implementar ventana deslizante básica
4. Manejar retransmisiones

**Código Base**:
```c
// Estados de conexión TCP
typedef enum {
    TCP_CLOSED,
    TCP_LISTEN,
    TCP_SYN_SENT,
    TCP_SYN_RECEIVED,
    TCP_ESTABLISHED,
    TCP_FIN_WAIT_1,
    TCP_FIN_WAIT_2,
    TCP_CLOSE_WAIT,
    TCP_CLOSING,
    TCP_LAST_ACK,
    TCP_TIME_WAIT
} tcp_state_t;

// Control Block de conexión TCP
typedef struct {
    tcp_state_t state;
    uint32_t local_ip;
    uint16_t local_port;
    uint32_t remote_ip;
    uint16_t remote_port;
    
    uint32_t send_seq;         // Próximo byte a enviar
    uint32_t send_ack;         // Próximo byte esperado
    uint32_t recv_seq;         // Próximo byte esperado
    uint32_t recv_ack;         // Último byte recibido
    
    uint16_t window_size;      // Tamaño de ventana
    uint8_t *send_buffer;      // Buffer de envío
    uint8_t *recv_buffer;      // Buffer de recepción
} tcp_connection_t;

// TODO: Implementar funciones TCP
int tcp_connect(uint32_t dest_ip, uint16_t dest_port);
int tcp_listen(uint16_t port);
int tcp_send(tcp_connection_t *conn, uint8_t *data, uint32_t length);
int tcp_receive(tcp_connection_t *conn, uint8_t *buffer, uint32_t max_length);
int tcp_close(tcp_connection_t *conn);
```

---

## 🔧 Integración con el Shell

### Comandos de Red

```c
// Comandos de red para el shell
void cmd_ping(char *args) {
    // Parsear IP destino
    uint32_t dest_ip = parse_ip_address(args);
    if (dest_ip == 0) {
        printf("Error: Dirección IP inválida\n");
        return;
    }
    
    // Enviar ping
    network_interface_t *iface = get_default_interface();
    if (iface) {
        icmp_send_ping(iface, dest_ip);
    } else {
        printf("Error: No hay interfaz de red disponible\n");
    }
}

void cmd_ifconfig(char *args) {
    printf("=== CONFIGURACIÓN DE RED ===\n");
    
    for (int i = 0; i < num_interfaces; i++) {
        network_interface_t *iface = network_interfaces[i];
        
        printf("Interfaz: %s\n", iface->name);
        printf("  MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
               iface->mac_address[0], iface->mac_address[1],
               iface->mac_address[2], iface->mac_address[3],
               iface->mac_address[4], iface->mac_address[5]);
        
        printf("  IP: %d.%d.%d.%d\n",
               (iface->ip_address >> 24) & 0xFF,
               (iface->ip_address >> 16) & 0xFF,
               (iface->ip_address >> 8) & 0xFF,
               iface->ip_address & 0xFF);
        
        printf("  Estadísticas:\n");
        printf("    Paquetes enviados: %d\n", iface->packets_sent);
        printf("    Paquetes recibidos: %d\n", iface->packets_received);
        printf("    Bytes enviados: %d\n", iface->bytes_sent);
        printf("    Bytes recibidos: %d\n", iface->bytes_received);
        printf("    Errores: %d\n", iface->errors);
        printf("\n");
    }
}

void cmd_netstat(char *args) {
    printf("=== ESTADÍSTICAS DE RED ===\n");
    
    // Mostrar conexiones activas
    printf("Conexiones activas:\n");
    // TODO: Implementar lista de conexiones
    
    printf("\nProtocolos:\n");
    printf("  IP: %d paquetes\n", ip_stats.packets_processed);
    printf("  TCP: %d conexiones\n", tcp_stats.active_connections);
    printf("  UDP: %d datagramas\n", udp_stats.datagrams_sent);
    printf("  ICMP: %d mensajes\n", icmp_stats.messages_sent);
}

void cmd_arp(char *args) {
    printf("=== TABLA ARP ===\n");
    
    // Mostrar tabla ARP
    arp_table_print();
    
    printf("\nUso: arp -a (mostrar tabla)\n");
    printf("     arp -d <ip> (eliminar entrada)\n");
}
```

---

## 📊 Análisis de Rendimiento

### Métricas de Red

```c
// Estructura de estadísticas de red
typedef struct {
    // Estadísticas generales
    uint32_t total_packets_sent;
    uint32_t total_packets_received;
    uint32_t total_bytes_sent;
    uint32_t total_bytes_received;
    
    // Estadísticas por protocolo
    uint32_t ethernet_frames;
    uint32_t ip_packets;
    uint32_t tcp_segments;
    uint32_t udp_datagrams;
    uint32_t icmp_messages;
    
    // Errores
    uint32_t checksum_errors;
    uint32_t protocol_errors;
    uint32_t buffer_overflows;
    uint32_t timeout_errors;
    
    // Rendimiento
    uint32_t average_latency;
    uint32_t peak_throughput;
    uint32_t current_throughput;
} network_stats_t;

void print_network_performance(void) {
    network_stats_t *stats = get_network_stats();
    
    printf("\n=== RENDIMIENTO DE RED ===\n");
    printf("Throughput actual: %d KB/s\n", stats->current_throughput / 1024);
    printf("Throughput pico: %d KB/s\n", stats->peak_throughput / 1024);
    printf("Latencia promedio: %d ms\n", stats->average_latency);
    printf("Tasa de errores: %.2f%%\n", 
           (float)stats->checksum_errors / stats->total_packets_received * 100);
    printf("===========================\n");
}
```

---

## 🎯 Proyecto Final: Servidor Web Básico

### Especificaciones

Implementar un servidor web HTTP básico con:

1. **Servidor HTTP**:
   - Manejo de requests GET
   - Respuestas HTTP válidas
   - Servir archivos estáticos
   - Logging de accesos

2. **Stack de Red Completo**:
   - Ethernet + IP + TCP
   - Múltiples conexiones concurrentes
   - Gestión de buffers
   - Timeouts y retransmisiones

3. **Interfaz de Usuario**:
   - Configuración vía shell
   - Monitoreo de conexiones
   - Estadísticas de acceso

### Ejemplo de Implementación

```c
// Servidor HTTP básico
void http_server_handler(tcp_connection_t *conn, uint8_t *data, uint32_t length) {
    // Parsear request HTTP
    http_request_t request;
    if (parse_http_request(data, length, &request) != 0) {
        send_http_error(conn, 400, "Bad Request");
        return;
    }
    
    // Servir archivo
    if (strcmp(request.method, "GET") == 0) {
        serve_file(conn, request.path);
    } else {
        send_http_error(conn, 405, "Method Not Allowed");
    }
}

void serve_file(tcp_connection_t *conn, const char *path) {
    // Leer archivo del filesystem
    file_t *file = filesystem_open(path);
    if (file == NULL) {
        send_http_error(conn, 404, "Not Found");
        return;
    }
    
    // Enviar response HTTP
    char response_header[512];
    snprintf(response_header, sizeof(response_header),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: text/html\r\n"
             "Content-Length: %d\r\n"
             "\r\n", file->size);
    
    tcp_send(conn, response_header, strlen(response_header));
    tcp_send(conn, file->data, file->size);
    
    filesystem_close(file);
}
```

---

## 🏆 Resumen del Capítulo

### Conceptos Clave Aprendidos

1. **Arquitectura de Red**: Modelo OSI/TCP-IP y stack de protocolos
2. **Drivers de Red**: Interfaz entre hardware y software
3. **Protocolos**: Ethernet, IP, TCP, UDP, ICMP
4. **Sockets**: API para comunicación de red
5. **Rendimiento**: Optimización y métricas de red

### Habilidades Desarrolladas

- ✅ Implementación de drivers de red
- ✅ Programación de protocolos de comunicación
- ✅ Debugging de comunicaciones de red
- ✅ Optimización de rendimiento de red
- ✅ Integración con el sistema operativo

### Próximos Pasos

En el siguiente capítulo exploraremos **Seguridad y Virtualización**, donde aprenderemos sobre mecanismos de protección, aislamiento y virtualización de recursos.

---

*"En redes, la confiabilidad no es opcional, es fundamental."* - Principio de Comunicaciones

**¡Felicitaciones!** 🎉 Has completado el capítulo de Redes y Protocolos. Ahora tienes los conocimientos para implementar comunicación de red en sistemas operativos.