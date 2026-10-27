# Mapa de registros UART0

Fuentes: datasheets oficiales RP2040/RP2350 y headers de Pico SDK 2.2.0 (ver REFERENCIAS.md). Dirección = base + offset. Todos los accesos del programa son de 32 bits, aunque algunos campos tengan menos bits. Los offsets se expresan en hexadecimal.

| Registro | Offset | Dirección RP2040 | Dirección RP2350 | Acceso | Campos usados | Valor |
|---|---|---|---|---|---|---|
| DR | 0x000 | 0x40034000 | 0x40070000 | R/W | 7:0 datos; 11:8 OE/BE/PE/FE al leer | Byte TX o RX |
| RSR/ECR | 0x004 | 0x40034004 | 0x40070004 | R/W | RSR al leer; cualquier escritura limpia errores | 0 para limpiar |
| FR | 0x018 | 0x40034018 | 0x40070018 | R | BUSY bit3, RXFE bit4, TXFF bit5 | Son dinámicos |
| IBRD | 0x024 | 0x40034024 | 0x40070024 | R/W | 15:0 divisor entero | Calculado |
| FBRD | 0x028 | 0x40034028 | 0x40070028 | R/W | 5:0 divisor fraccionario | Calculado |
| LCR_H | 0x02C | 0x4003402C | 0x4007002C | R/W | WLEN[6:5]=3, FEN[4]=1, STP2[3]=0, PEN[1]=0 | 0x70 |
| CR | 0x030 | 0x40034030 | 0x40070030 | R/W | RXE[9], TXE[8], UARTEN[0] | 0x301 |
| IMSC | 0x038 | 0x40034038 | 0x40070038 | R/W | Máscaras IRQ UART | 0 |
| ICR | 0x044 | 0x40034044 | 0x40070044 | W1C | Limpia fuentes de interrupción | 0x7FF |
| DMACR | 0x048 | 0x40034048 | 0x40070048 | R/W | DMA RX bit0, TX bit1 | 0 |

FR se lee sin retirarse bytes de la FIFO; DR sí consume un byte RX al leer. Leer DR para inspeccionarlo puede alterar la aplicación. ICR es write-one-to-clear: escribir uno limpia el evento, no lo habilita. ECR comparte dirección con RSR y tiene semántica distinta en escritura.

## Reset utilizado por punteros

| Operación | Offset desde RESETS | RP2040 | RP2350 |
|---|---|---|---|
| RESET normal | 0x000 | 0x4000C000 | 0x40020000 |
| RESET alias SET | 0x2000 | 0x4000E000 | 0x40022000 |
| RESET alias CLR | 0x3000 | 0x4000F000 | 0x40023000 |
| RESET_DONE lectura | 0x008 | 0x4000C008 | 0x40020008 |
| Máscara UART0 | bit22 / bit26 | 0x00400000 | 0x04000000 |

Los alias SET/CLR evitan una operación read-modify-write sobre el registro RESET compartido. Se escribe solo la máscara UART0. Se espera RESET_DONE antes de configurar UART.

## Secuencia de configuración

1. Esperar BUSY=0 si ya se transmitía.
2. Configurar GP0 y GP1 con `gpio_set_function(..., GPIO_FUNC_UART)`. La función selecciona UART en IO_BANK0 y prepara los pads. Esa parte utiliza SDK en ambos modos.
3. Reset UART0 y espera de RESET_DONE.
4. CR=0, IMSC=0, ICR=0x7FF, ECR=0 y DMACR=0.
5. IBRD y FBRD calculados a partir de `clock_get_hz(clk_peri)`.
6. LCR_H=0x70, después CR=0x301.

La escritura de LCR_H aplica los divisores. No cambiar formato con una transmisión activa. El programa no toca relojes globales: usa los establecidos por el arranque SDK.

## Divisor

D = f_peri / (16 × baud). IBRD = parte entera; FBRD = redondeo de 64 × fracción, con acarreo si redondea a64. El código calcula `d64 = (4*f_peri + baud/2)/baud` en 64 bits y lo divide en parte entera y fraccionaria. Evita asumir 125 MHz, 150 MHz o 48 MHz. Para **48 MHz como ejemplo**, 9600 baud requiere IBRD=312 y FBRD=32. La frecuencia real se imprime con `uart info` y puede ser otra.

## Punteros

```c
volatile uint32_t *cr = (volatile uint32_t *)(UART0_BASE + 0x030u);
*cr = (1u << 9) | (1u << 8) | 1u;
```

`UART0_BASE + 0x030` es una dirección, `cr` guarda esa dirección y `*cr` accede al registro. `uint32_t` fija 32 bits; `volatile` obliga a realizar los accesos observables y evita tratar el registro como una variable constante en un bucle. No vuelve atómicas todas las operaciones ni sustituye exclusión mutua. `1u << 9` crea la máscara RXE. La asignación completa es apropiada después del reset cuando conocemos los demás bits.

## Arquitectura y opciones

UART0/1 son periféricos PL011 con registros APB, generador de baud, registro de desplazamiento y FIFO TX/RX de 32 entradas. En TX la CPU llena DR y el hardware transmite bits por el multiplexor de GPIO. En RX el hardware detecta inicio, muestrea y coloca datos/errores en FIFO. Interrupciones y DMA permiten atender tráfico sin sondeo continuo; aquí se usa polling para hacer visible la relación entre FR y DR.

Opciones principales: baud, 5–8 bits de datos, paridad ninguna/par/impar, 1–2 stop, FIFO habilitada/deshabilitada, control RTS/CTS, IRQ RX/TX/errores y DMA. Ambos extremos deben coincidir en formato y velocidad. Se elige 9600 8N1 sin flujo por simplicidad y margen para módulos de fabricante desconocido. Cambiar baud en este proyecto requiere modificar BAUD en uart_link.c, recompilar y ajustar la terminal. Las opciones se explican, pero el REPL no permite cambiar arbitrariamente el formato de su propio canal.

## Registros heredados

Los comandos SIO usan `sio_hw` del SDK para GPIO_IN, GPIO_OUT, GPIO_OE y alias SET/CLR/XOR. El temporizador usa `timer_hw` y su alarma0. Esas estructuras también representan registros mapeados, pero el ejemplo explícito de punteros solicitado por UART está en uart_link.c. La ISR TIMER solo limpia, conmuta, cuenta y rearma; no imprime. Se reclama alarma0 para evitar uso accidental simultáneo.
