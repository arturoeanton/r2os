# Dockerfile para Mini-Kernel Educativo
FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive
ENV TARGET=i686-elf
ENV PREFIX=/usr/local/cross
ENV PATH="$PREFIX/bin:$PATH"

# Instalar dependencias base
RUN apt-get update && apt-get install -y \
    build-essential \
    gcc \
    g++ \
    make \
    libc6-dev \
    bison \
    flex \
    libgmp3-dev \
    libmpc-dev \
    libmpfr-dev \
    texinfo \
    qemu-system-x86 \
    qemu-utils \
    gdb \
    git \
    wget \
    curl \
    nano \
    vim \
    sudo \
    ca-certificates \
    xauth \
    x11-apps && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# Crear usuario developer
RUN useradd -m -s /bin/bash developer && \
    usermod -aG sudo developer && \
    echo "developer ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers

# Crear directorio para cross-compiler
RUN mkdir -p $PREFIX

# Compilar Binutils
WORKDIR /tmp
RUN wget -q https://ftp.gnu.org/gnu/binutils/binutils-2.39.tar.gz && \
    tar -xf binutils-2.39.tar.gz && \
    mkdir build-binutils && \
    cd build-binutils && \
    ../binutils-2.39/configure \
        --target=$TARGET \
        --prefix=$PREFIX \
        --with-sysroot \
        --disable-nls \
        --disable-werror && \
    make -j4 && \
    make install && \
    cd /tmp && \
    rm -rf binutils-2.39* build-binutils

# Compilar GCC
RUN wget -q https://ftp.gnu.org/gnu/gcc/gcc-12.2.0/gcc-12.2.0.tar.gz && \
    tar -xf gcc-12.2.0.tar.gz && \
    mkdir build-gcc && \
    cd build-gcc && \
    ../gcc-12.2.0/configure \
        --target=$TARGET \
        --prefix=$PREFIX \
        --disable-nls \
        --enable-languages=c,c++ \
        --without-headers && \
    make -j4 all-gcc && \
    make -j4 all-target-libgcc && \
    make install-gcc && \
    make install-target-libgcc && \
    cd /tmp && \
    rm -rf gcc-12.2.0* build-gcc

# Verificar instalación
RUN which i686-elf-gcc && i686-elf-gcc --version

# Scripts útiles
RUN echo '#!/bin/bash' > /usr/local/bin/check-environment && \
    echo 'echo "=== Verificación del Entorno ==="' >> /usr/local/bin/check-environment && \
    echo 'i686-elf-gcc --version | head -1' >> /usr/local/bin/check-environment && \
    echo 'qemu-system-i386 --version | head -1' >> /usr/local/bin/check-environment && \
    echo 'echo "✅ Entorno listo!"' >> /usr/local/bin/check-environment && \
    chmod +x /usr/local/bin/check-environment

# Configurar entorno
ENV CC=i686-elf-gcc
ENV AS=i686-elf-as
ENV LD=i686-elf-ld

# Cambiar a usuario developer
USER developer
WORKDIR /workspace

# Configurar bash
RUN echo 'export PATH="$PREFIX/bin:$PATH"' >> ~/.bashrc && \
    echo 'echo "🎓 Mini-Kernel Educativo"' >> ~/.bashrc

CMD ["/bin/bash"]