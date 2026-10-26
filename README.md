# UART ↔ RS-232 con computadora — RP2040 y RP2350

Actividad integradora: periféricos de la Raspberry Pi Pico (RP2040) y Pico 2 W (RP2350).
Equipo: Ana Nickole Cisneros Herrera (203477) · Leslie Yoselin Sanchez Flores (201831)

## Objetivo
Configurar la UART (ARM PL011) mediante **(a) punteros a registros** y **(b) Pico SDK**, comunicarla con una
computadora a través de una interfaz **RS-232 (MAX3232)** y controlarla con el comando de REPL `rs232`.
**El USB del Pico no se usa como puerto serial**: los datos viajan por UART1 → MAX3232 → RS-232 → PC.

## Descripción del hardware
| Señal | Pico | Destino |
|---|---|---|
| UART1 TX | GP4 (pin 6) | MAX3232 T1IN |
| UART1 RX | GP5 (pin 7) | MAX3232 R1OUT |
| 3V3(OUT) | pin 36 | MAX3232 VCC (3.3 V) |
| GND | pin 38 | MAX3232 GND |
| Consola del REPL del curso | USB (stdio del curso) | Solo consola; los datos RS-232 NO pasan por el USB |

El lado RS-232 del MAX3232 va al DB9 y de ahí a la PC (adaptador USB-RS232 o puerto COM). Si el módulo es DTE,
usar cable **null-modem**. Nunca conectar RS-232 directo a los GPIO.

## Componentes
Pico (RP2040), Pico 2 W (RP2350), módulo MAX3232 con DB9, cable serial DB9, adaptador USB-RS232 para la PC,
protoboard y jumpers.

## Software y herramientas
Pico SDK ≥ 2.0, CMake ≥ 3.13, arm-none-eabi-gcc, terminal serial (PuTTY / Tera Term / minicom).
MicroPython: firmware oficial de Pico y Pico 2 W, Thonny.

## Compilación
```bash
export PICO_SDK_PATH=/ruta/al/pico-sdk
cp $PICO_SDK_PATH/external/pico_sdk_import.cmake .
./build_both.sh            # genera build_pico/ (RP2040) y build_pico2_w/ (RP2350)
```

## Carga del programa
Mantener BOOTSEL al conectar, copiar `build_pico/uart_rs232.uf2` (RP2040) o `build_pico2_w/uart_rs232.uf2` (RP2350)
a la unidad `RPI-RP2` / `RP2350`.

## Uso del REPL (el del curso, con el comando rs232 agregado)
```
rs232 <texto...>        envía el texto por RS-232       (rs232 hola)
rs232 enviar <texto>    igual (para palabras reservadas)
rs232 baud <n>          300..230400
rs232 fmt <8N1|7E2|..>  bits / paridad N,E,O / parada
rs232 modo <reg|sdk>    punteros a registros o Pico SDK
rs232 rx [ms]           muestra lo que escribas en la PC
rs232 lazo              autoprueba con loopback interno (sin cables)
rs232 estado            volcado de registros
```
Cadena demostrable: **comando REPL → C (argc/argv) → UART → MAX3232 → PC → texto en la terminal**.

## Implementación mediante registros (`src/uart_regs.c`, `src/rp_map.h`)
Dirección base → offset → dirección del registro → puntero `volatile uint32_t*` → valor → periférico.
Pasos: sacar UART del reset (RESETS) → pad + FUNCSEL=2 (PADS/IO_BANK0) → IBRD/FBRD → LCR_H → CR.
Mapa completo de registros: `proyecto_uart_rs232.json` (sección `mapa_registros`) y `docs/mapa_registros.md`.

## Implementación mediante SDK (`src/uart_sdk.c`)
`uart_init`, `gpio_set_function`, `uart_set_format`, `uart_putc_raw`, `uart_is_readable_within_us`.
El SDK abstrae reset, divisores, pads (incl. ISO en RP2350) y orden de escritura. No ofrece API de loopback.

## Diferencias RP2040 / RP2350
| | RP2040 | RP2350 |
|---|---|---|
| UART0 / UART1 base | 0x40034000 / 0x40038000 | 0x40070000 / 0x40078000 |
| RESETS / IO_BANK0 / PADS_BANK0 | 0x4000C000 / 0x40014000 / 0x4001C000 | 0x40020000 / 0x40028000 / 0x40038000 |
| Bit de reset UART1 | 23 | 27 |
| Pad | sin ISO | ISO (bit 8) a 0 |
| clk_sys por defecto | 125 MHz (IBRD 67, FBRD 52) | 150 MHz (IBRD 81, FBRD 24) |
| Offsets/campos PL011 | iguales | iguales |

## MicroPython (`micropython/rs232.py`)
Copiar a la placa (Thonny) y ejecutar `import rs232; rs232.rs232("hola")` o `rs232.repl()`.
`RegUART` usa `machine.mem32` (equivale a punteros) y `SdkUART` usa `machine.UART`.

## Procedimiento para reproducir la demostración
1. Conectar según la tabla; abrir la terminal de la PC (puerto del adaptador RS-232, 115200 8N1).
2. En la consola del REPL: `rs232 estado`, `rs232 lazo` (debe dar OK).
3. `rs232 hola equipo` → aparece en la PC. `rs232 modo sdk` y repetir.
4. `rs232 rx 10000` y escribir en la PC → aparece en la consola.
5. Repetir con la otra placa y comparar `rs232 estado`.

## Evidencias
`docs/evidencias/`: compilación, REPL, configuración, conexión, demostración, RP2040 y RP2350.
