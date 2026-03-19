# Implementación de ChaCha20 en RISC-V con QEMU y GDB

Este proyecto implementa el algoritmo **ChaCha20** en ensamblador **RISC-V de 32 bits**, integrado con un programa principal en **C** para ejecutar pruebas en un entorno **bare-metal**, usando **QEMU**, **GDB** y **Docker**.

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

`main.c` implementa la salida por UART, define los vectores de prueba y llama a las funciones `quarter_round`, `block` y `chacha20_encrypt`.

`math_asm.s` contiene la implementación en ensamblador de `quarter_round`, `block` y `chacha20_encrypt`.

`startup.s` inicializa el entorno bare-metal antes de ejecutar `main`.

`linker.ld` define el mapa de memoria del programa.

`Dockerfile` define la imagen con QEMU y el toolchain RISC-V.

`run.sh` automatiza la construcción de la imagen y la ejecución del contenedor.

`docs/` contiene la documentación y capturas de depuración.

## 2. Requisitos previos

Antes de ejecutar el proyecto, se requiere contar con:

- **Docker** instalado y funcionando correctamente.
- Permisos para ejecutar contenedores Docker.
- `make` instalado para usar las reglas del `Makefile`.
- Un sistema Linux o un entorno compatible con Docker.
- Conexión a internet la primera vez, para construir o descargar la imagen base del contenedor.

El **toolchain de RISC-V** y **QEMU** no necesitan instalarse manualmente en la máquina host, ya que ambos se configuran dentro del contenedor definido en el `Dockerfile`.

## 3. Descripción general

El proyecto implementa las operaciones principales del algoritmo **ChaCha20**:

- `quarter_round`
- `block`
- `chacha20_encrypt`

El programa principal ejecuta vectores de prueba oficiales del **RFC 8439** para validar cada nivel del algoritmo:

- pruebas de `quarter_round`,
- pruebas de `block`,
- y pruebas de `chacha20_encrypt`.

En cada caso, el programa:

- muestra los datos de entrada,
- ejecuta la función correspondiente,
- imprime el resultado obtenido,
- lo compara con el resultado esperado,
- y reporta si la prueba fue exitosa.

En las pruebas de `chacha20_encrypt`, además, se vuelve a aplicar la función al ciphertext para verificar que se recupere correctamente el texto original.

## 4. Compilación y ejecución

```bash
# en una terminal
make start

#compilar
make build

# ejecutar con qemu
make qmu

```

También puede ejecutarse compilación y ejecución en un solo paso:

```bash
# compilar y ejecutar
make bqmu
```

## 5. Depuración y entorno

### Depuración con GDB

La depuración se realiza conectando `gdb-multiarch` al servidor remoto que QEMU expone en el puerto `1234`. Esto permite inspeccionar la ejecución de las funciones `quarter_round`, `block` y `chacha20_encrypt`, así como revisar registros y memoria en tiempo real.

Flujo típico de depuración:

```bash
# en otra terminal, luego de inicializar QEMU
make run

# iniciar debug en GDB
make dg
```

Comandos útiles de GDB:

```gdb
break _start
break main
break quarter_round
break block
break chacha20_encrypt
continue
step
info registers
x/16wx $sp
continue
```

### Convenciones de llamada RISC-V

El proyecto sigue la convención estándar de llamadas de RISC-V:

- `a0-a7`: parámetros de entrada y valores de retorno
- `t0-t6`: registros temporales
- `s0-s11`: registros preservados
- `ra`: dirección de retorno
- `sp`: puntero de pila

Las rutinas en ensamblador respetan estas convenciones guardando en la pila los registros preservados que modifican y restaurándolos antes de retornar. Además, `block` y `chacha20_encrypt` reciben punteros a buffers de salida para escribir sus resultados directamente en memoria, lo que facilita la integración con C.