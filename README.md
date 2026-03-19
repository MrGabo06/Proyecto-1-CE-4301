# Implementación de ChaCha20 en RISC-V con QEMU y GDB

Este proyecto implementa el algoritmo **ChaCha20** en ensamblador **RISC-V de 32 bits**, integrado con un programa principal en **C** para realizar pruebas interactivas en un entorno **bare-metal**, usando **QEMU**, **GDB** y **Docker**.

---

## 1. Estructura del proyecto

```text
.
├── Dockerfile
├── run.sh
├── Makefile
├── src/
│   ├── main.c
│   ├── linker.ld
│   ├── build.sh
│   ├── run-qemu.sh
│   ├── math_asm.s
│   └── startup.s
├── docs/
│   ├── bug_1.png
│   ├── bug_2.png
│   ├── bug_3.png
│   └── DOCUMENTACION.md
└── README.md
```

`src/` contiene el código fuente del proyecto.

`main.c` implementa la interacción por UART y llama a la función `chacha20_encrypt`.

`math_asm.s` contiene la implementación en ensamblador de `quarter_round`, `block` y `chacha20_encrypt`.

`startup.s` inicializa el entorno bare-metal antes de ejecutar `main`.

`linker.ld` define el mapa de memoria del programa.

`Dockerfile` define la imagen con QEMU y el toolchain RISC-V.

`run.sh` automatiza la construcción de la imagen y la ejecución del contenedor.

`docs/` contiene la documentación y capturas de depuración.

## 2. Descripción general

El proyecto implementa las operaciones principales del algoritmo **ChaCha20**:

- `quarter_round`
- `block`
- `chacha20_encrypt`

El programa principal permite ingresar:

- una clave de 256 bits,
- un nonce de 96 bits,
- un contador inicial,
- y un plaintext.

Luego el programa:

- cifra el mensaje,
- muestra el ciphertext en hexadecimal,
- vuelve a aplicar `chacha20_encrypt` al ciphertext,
- y verifica que se recupere el texto original.

Esto permite validar manualmente el funcionamiento básico del cifrado y descifrado.

## Resumen de uso

- `make start`: construye e inicia el entorno Docker.
- `make run`: abre una terminal dentro del contenedor.
- `make build`: compila el proyecto.
- `make qmu`: ejecuta el programa en QEMU.
- `make bqmu`: compila y ejecuta en un solo paso.
- `make dg`: abre una sesión de depuración con GDB.

## 6. Compilación y entorno

El proyecto está diseñado para ejecutarse en un entorno bare-metal RISC-V.

La compilación se realiza mediante `build.sh`, utilizando una configuración adecuada para este contexto, normalmente con opciones como:

- `-march=rv32im`
- `-mabi=ilp32`
- `-nostdlib`
- `-ffreestanding`
- `-g`

Estas opciones permiten generar un ejecutable compatible con QEMU y con soporte para depuración en GDB.