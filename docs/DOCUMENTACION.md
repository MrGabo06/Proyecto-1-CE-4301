## Mapeo entre estados y registros

En esta implementación, el estado de ChaCha20 de 16 palabras de 32 bits **no se mantiene de forma permanente en registros**. En su lugar, se usa un esquema mixto donde el **estado completo se almacena en memoria (stack)** y únicamente las **4 palabras activas** de cada llamada a `quarter_round` se cargan temporalmente en registros. Esta decisión reduce la presión sobre el banco de registros y simplifica la reutilización de la rutina `quarter_round`.

### Organización del estado en memoria

Dentro de `block`, el stack guarda dos copias del estado:

| Rango en stack | Contenido |
|---|---|
| `0(sp)` a `60(sp)` | estado original `x0` a `x15` |
| `64(sp)` a `124(sp)` | estado de trabajo `x0` a `x15` |

La copia original se conserva para realizar la **suma final** del algoritmo, mientras que la copia de trabajo es la que se modifica durante las 20 rondas.

### Mapeo de palabras en registros durante `quarter_round`

La función `quarter_round` recibe 4 palabras mediante los registros de argumentos:

| Registro | Significado |
|---|---|
| `a0` | palabra `a` |
| `a1` | palabra `b` |
| `a2` | palabra `c` |
| `a3` | palabra `d` |

Al inicio de la rutina, estas palabras se copian a registros temporales para operar con ellas:

| Registro temporal | Palabra almacenada |
|---|---|
| `t0` | `a` |
| `t1` | `b` |
| `t2` | `c` |
| `t3` | `d` |
| `t4`, `t5` | auxiliares para rotaciones |

Al finalizar, los resultados vuelven a `a0`–`a3`, lo que permite retornar las 4 palabras actualizadas al código llamador.

### Mapeo en rondas de columna

Durante las rondas de columna, las palabras del estado de trabajo se cargan así:

| Quarter round | `a0` | `a1` | `a2` | `a3` |
|---|---|---|---|---|
| 1 | `x0` | `x4` | `x8`  | `x12` |
| 2 | `x1` | `x5` | `x9`  | `x13` |
| 3 | `x2` | `x6` | `x10` | `x14` |
| 4 | `x3` | `x7` | `x11` | `x15` |

### Mapeo en rondas diagonales

Durante las rondas diagonales, el mapeo cambia para seguir la especificación de ChaCha20:

| Quarter round | `a0` | `a1` | `a2` | `a3` |
|---|---|---|---|---|
| 1 | `x0` | `x5` | `x10` | `x15` |
| 2 | `x1` | `x6` | `x11` | `x12` |
| 3 | `x2` | `x7` | `x8`  | `x13` |
| 4 | `x3` | `x4` | `x9`  | `x14` |

### Justificación del diseño

Este mapeo fue elegido porque permite:

- conservar el estado completo sin consumir demasiados registros,
- reutilizar una sola rutina `quarter_round` para columnas y diagonales,
- mantener un flujo claro entre **carga desde memoria → procesamiento en registros → escritura de regreso**.

En consecuencia, el diseño no asigna una palabra fija del estado a un registro fijo durante toda la ejecución, sino que utiliza un **mapeo temporal y controlado**, adecuado para una implementación modular en RISC-V.


## Bitácora de bug
Al realizar varios casos de prueba para la función ***chacha20_encrypt*** se encontró un error que hacía que al ingresar un **plaintext** con un tamaño inferior a los 64 bytes, el programa dejaba de funcionar. Sin embargo, al ingresar **plaintext** de tamaño igual o superior a 64 bytes, el programa funcionaba con normalidad. Al realizar el debugeo se encontró que esto ocurría porque al recibir un bloque menor a 64 bytes, en el registro que almacena la cantidad de bloques de 64 bytes enteros que existen, se guarda un *0* (*s2*). 

![Texto alternativo](./bug_1.png)

 Lo que ocacionaba esto era que al ingresar a ***complete_block_loop*** al finalizar el primer loop, restara 1 y en el registro *s2* se almacenara un -1. 

 ![Texto alternativo](./bug_2.png)
 
 Como en el programa de ensamblador el condicional que frena el loop es un ***bnez*** esto ocacionaba que entrara en un ciclo infinito, pues nunca iba a ser 0 ya que en cada iteración se resta 1 por la naturaleza del loop. 

  ![Texto alternativo](./bug_3.png)

  Para solucionar esto, se decidió incluir una instrucción la cual verifica si al realizar el calculo de los bloques completos, el registro **s2** tiene un 0 y en caso de ser así, se desplaza a la vferificación de bloques incompletos. Con esto se soluciona el problema de que con bloques de tamaño menor a 64 bytes el programa falle, pues al leer **0** en **s2** se trata como un bloque incompleto.