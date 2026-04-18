# UNO en C 

Proyecto de **UNO en C** para terminal. El repositorio incluye:

- motor base del juego.
- representación de cartas, manos y estado de partida.
- reglas principales para una partida local.
- un **multiplayer** en desarrollo usando **sockets TCP** con arquitectura **host/cliente**.

> Estado actual: el proyecto ya puede generar una partida, administrar mazo y descarte, validar jugadas y establecer una primera comunicación host-cliente por red. La capa multiplayer está en fase de desarrollo: el **host mantiene el estado real del juego** y el cliente remoto recibe el estado visible y envía comandos simples.

---

## Características actuales

### Juego
- Mazo de **109 cartas**.
- 4 colores: **Red, Yellow, Green, Blue**.
- Cartas numéricas.
- Cartas de acción:
  - `SKIP`
  - `REVERSE`
  - `DRAW_TWO`
- Comodines:
  - `WILD`
  - `WILD_DRAW_FOUR`
  - `WILD_TOTAL` (carta especial propia del proyecto, aún sin implementar)
- Mezcla del mazo con **Fisher-Yates**.
- Reparto inicial de cartas.
- Validación de jugadas según color, número, tipo y comodines.
- Conteo de cartas válidas por jugador.
- Verificación de victoria.
- Sistema de descarte (`bin`) para **reciclar el mazo** cuando se vacía.

### Interfaz de terminal
- Salida coloreada con secuencias ANSI.
- Visualización de la carta en mesa.
- Visualización de la mano actual.
- Entrada por teclado con validación básica.

### Multiplayer base
- Comunicación por **TCP**.
- Modelo **cliente-servidor**.
- El **host es autoritativo**: mantiene `GameState` y aplica la lógica del juego.
- El cliente recibe el **estado visible** y envía comandos de texto.
- Protocolo textual orientado por líneas, con mensajes como:
  - `STATE`
  - `TOP ...`
  - `COLOR ...`
  - `HAND_COUNT ...`
  - `CARD ...`
  - `YOUR_TURN`
  - `WAIT`
  - `PLAY n`
  - `DRAW`

---

## Estructura del proyecto

```text
.
├── incs/
│   ├── cards.h
│   ├── colors.h
│   ├── game.h
│   ├── net.h
│   ├── protocol.h
│   ├── client_ui.h
│   └── session.h
├── src/
│   ├── cards.c
│   ├── game.c
│   ├── main.c
│   ├── net.c
│   ├── protocol.c
│   ├── client_ui.c
│   └── session.c
├── build/
├── Makefile
└── README.md
```

### Descripción de módulos

#### `cards.*`
Contiene la representación de las cartas y utilidades asociadas:
- enums de color y tipo de carta.
- estructura `Card`.
- estructura `HandSlot`.
- estructura `Deck`.
- generación de mazo.
- mezcla (`shuffle_deck`).
- funciones de impresión.
- ordenamiento de mano para compactar slots válidos.

#### `game.*`
Contiene el núcleo del juego:
- estructura `GameState`.
- inicialización de partida (`init_game`).
- reparto de cartas.
- robo desde el mazo.
- descarte y reciclaje de mazo (`bin`).
- validación de movimientos.
- aplicación de efectos de cartas.
- verificación de victoria.

#### `net.*`
Capa de red de bajo nivel:
- creación de socket servidor;
- aceptación de clientes;
- conexión desde cliente;
- envío completo de buffers (`send_all`);
- envío y recepción de líneas (`send_line`, `recv_line`);
- cierre de sockets.

#### `protocol.*`
Capa de protocolo de aplicación:
- serializa parte del estado del juego en mensajes de texto;
- envía el estado visible al cliente;
- encapsula mensajes como `YOUR_TURN`, `WAIT` y recepción de comandos.

#### `session.*`
Controla la sesión host/cliente:
- `run_host()` inicia el servidor, acepta conexión e interactúa con el cliente;
- `run_client()` se conecta al host, recibe estado y envía acciones.

#### `main.c`
Punto de entrada del programa. Solo decide si ejecutar en modo:
- `host`
- `client <ip>`

---

## Modelo de datos principal

### `Card`
Representa una carta individual.

```c
typedef struct {
    CardType type;
    Color color;
    int number;
} Card;
```

### `HandSlot`
Representa una posición en la mano del jugador.
Permite conservar un arreglo fijo y marcar si el slot está ocupado.

```c
typedef struct {
    Card card;
    bool valid;
} HandSlot;
```

### `Deck`
Representa un mazo o pila de cartas.

```c
typedef struct {
    Card cards[DECK_SIZE];
    int size;
} Deck;
```

### `GameState`
Representa el estado completo del juego.

```c
typedef struct {
    Deck deck;
    Deck bin;
    HandSlot player1[MAX_HAND_SIZE];
    HandSlot player2[MAX_HAND_SIZE];
    Card topCard;
    Color currentColor;
    int currentPlayer;
} GameState;
```

---

## Reglas y decisiones de implementación

### Mano
Las manos se modelan con `HandSlot[]` en vez de un arreglo simple de `Card[]`. Esto permite:
- marcar cartas jugadas como inválidas;
- mantener tamaño fijo;
- compactar la mano sin memoria dinámica.

### Descarte
La carta superior en mesa se mantiene en `topCard`.
Las cartas ya jugadas, excepto la superior, se guardan en `bin`.
Cuando `deck` se vacía:
1. se mezcla `bin`;
2. `bin` pasa a ser el nuevo `deck`;
3. `bin` queda vacío.

### Multiplayer
El cliente **no mantiene el estado real del juego**.
Solo muestra lo que el host le envía y responde con comandos.

---

## Compilación

Desde la raíz del proyecto:

```bash
make
```

El ejecutable se genera en:

```text
build/uno
```

---

## Ejecución

### Modo host

```bash
make unoh
```

O directamente:

```bash
./build/uno host
```

### Modo cliente

```bash
make unoc IP=127.0.0.1
```

O directamente:

```bash
./build/uno client 127.0.0.1
```

> Al nivel que se encuentra el proyecto, se recomienda usar `127.0.0.1` en dos terminales sobre la misma máquina.

---

## Limitaciones actuales

El proyecto está en una etapa funcional, pero todavía base. Algunas limitaciones actuales son:

- la parte multiplayer aún está en evolución;
- no hay una partida remota completa con dos clientes simultáneos;
- el protocolo actual es textual y simple, pensado para depuración y desarrollo;
- varias reglas avanzadas y flujo completo de turnos en red aún pueden requerir refactor;
- algunas decisiones actuales priorizan claridad y avance del proyecto por sobre una arquitectura final definitiva.

---

## Posibles mejoras futuras

- alternancia completa de turnos entre host y cliente;
- soporte para más de 2 jugadores;
- interfaz más amigable en terminal;

---

## Ideas clave del proyecto

Este repositorio no solo busca implementar UNO, sino también practicar:

- modelado de datos en C.
- separación modular del código.
- sockets TCP en sistemas UNIX.
- diseño de un protocolo simple de aplicación.
- arquitectura cliente-servidor.

---

## Autor
Iván Ignacio Sebastián Gallardo Barría.

Proyecto personal desarrollado en C.

