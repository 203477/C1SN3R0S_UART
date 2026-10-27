# UART y RS-232 con Raspberry Pi Pico

**Ana Nickole Cisneros Herrera — 203477**  
**Leslie Yoselin Sánchez Flores — 201831**

Proyecto de la actividad integradora de periféricos. Adaptación del monitor C proporcionado, con atribución y copia intacta en `original/main_base.c`. El mismo código se compila para Pico RP2040 y Pico 2 W RP2350. Ambas implementaciones UART son seleccionables desde el REPL.

## Primero revisa el adaptador

**Un USB–VGA de 15 contactos en tres filas sirve para video y no permite este proyecto.** Se necesita un adaptador **USB–RS-232**, normalmente DE-9 (llamado DB9) de nueve contactos en dos filas, o un puerto RS-232 nativo de la computadora. Un adaptador USB–TTL tampoco sustituye el trayecto RS-232 solicitado. La forma del conector no basta: verifica modelo y especificación.

El USB de la **computadora** puede conectarse al USB–RS-232 externo. El USB de la **Pico** se utiliza solamente para cargar UF2 y alimentar, nunca como serial USB CDC. CMake deshabilita los dos drivers stdio automáticos; toda entrada y salida pasa por UART0.

## Objetivo y arquitectura

Enviar comandos desde una terminal de PC, recibirlos por UART0, ejecutarlos en C y devolver resultados a través del MAX3232 y RS-232. Comparar el acceso con punteros a registros frente a las funciones del SDK. El comando `rs232 hola` produce una respuesta `[RS232] hola` en la computadora. El eco de tecleo y la respuesta son eventos distintos.

El REPL y la aplicación comparten el mismo enlace físico, por lo que basta un MAX3232 y un adaptador USB–RS-232. La CPU accede a los registros por el bus APB. UART serializa bytes en TX y reconstruye bytes en RX; el MAX3232 adapta los niveles eléctricos.

## Componentes

- Pico 2 W RP2350 disponible. Pico RP2040 adicional o prestada para cumplir la demostración en ambas plataformas.
- Módulo MAX3232 con capacitores de bomba de carga. Si solo tienes el integrado, consulta `docs/CONEXIONES.md`.
- Adaptador USB–RS-232 auténtico, computadora, terminal serial.
- Protoboard, cables, cable USB para alimentar y cargar.
- LED externo, resistencia de 330 Ω, pulsador opcional para los comandos heredados.

## Conexiones

Con alimentación desconectada, identifica señales por dirección y por esquema del módulo. Las etiquetas TX/RX de módulos comerciales pueden referirse a puntos de vista distintos.

| Origen | Destino | Función |
|---|---|---|
| Pico GP0, pin físico 1 | MAX3232 T1IN / DIN1 | TX lógico a 3.3 V |
| MAX3232 R1OUT / ROUT1 | Pico GP1, pin físico 2 | RX lógico a 3.3 V |
| Pico 3V3 OUT, pin 36 | MAX3232 VCC | Alimentación 3.3 V |
| Pico GND, pin 3 | MAX3232 GND y masa RS-232 | Referencia común |
| MAX3232 T1OUT / DOUT1 | RXD PC, DE-9 pin 2 si DTE estándar | Datos hacia computadora |
| TXD PC, DE-9 pin 3 si DTE estándar | MAX3232 R1IN | Datos desde computadora |
| DE-9 pin 5 | GND común | Masa de señal |
| Pico GP15, pin 20 | Resistencia 330 Ω, ánodo LED; cátodo a GND | Salida de demostración |
| Pico GP13, pin 17 | Pulsador a GND | Entrada con pull-up, presionado = 0 |

No conectar tensiones RS-232 directamente a GP0/GP1. Alimenta el MAX3232 a 3.3 V para mantener compatible su salida lógica. No usar VBUS 5 V en este montaje. Consulta `docs/conexiones.svg` y `docs/CONEXIONES.md` antes de conectar un módulo con DE-9 integrado.

## Herramientas y compilación

Pico SDK **2.2.0**, CMake, Make o Ninja, compilador `arm-none-eabi-gcc` con newlib y bibliotecas C++, Git. En Omarchy/Arch, instala si faltan: `sudo pacman -S --needed git cmake make gcc arm-none-eabi-gcc arm-none-eabi-newlib`. En Windows puede usarse la extensión oficial Raspberry Pi Pico de VS Code o un entorno con Pico SDK y toolchain configurados. La Pico usa C compilado, no MicroPython.

```bash
git clone --depth 1 --branch 2.2.0 https://github.com/raspberrypi/pico-sdk.git "$HOME/pico-sdk"
export PICO_SDK_PATH="$HOME/pico-sdk"
# Desde la carpeta Proyecto_UART:
cmake -S . -B build_pico2w -DPICO_BOARD=pico2_w
cmake --build build_pico2w -j
cmake -S . -B build_pico -DPICO_BOARD=pico
cmake --build build_pico -j
```

La aplicación no utiliza Wi-Fi ni TinyUSB y no necesita esos submódulos. El SDK descarga y compila picotool para generar el UF2 si no está instalado; la primera configuración necesita Internet y un compilador C/C++ para la computadora anfitriona. No usar `PICO_NO_PICOTOOL=1`: deshabilita la salida UF2. Usa carpetas separadas porque cambiar `PICO_BOARD` sobre una configuración vieja puede conservar valores incompatibles.

## Carga y terminal

1. Mantén BOOTSEL y conecta el USB de la Pico. Suelta BOOTSEL.
2. Copia `build_pico2w/uart_rs232.uf2` a la unidad **RP2350**, o `build_pico/uart_rs232.uf2` a **RPI-RP2** para Pico RP2040. No intercambiar binarios.
3. La unidad se desmonta automáticamente al cargar. Eso es normal. El USB de la Pico no mostrará un puerto serial.
4. Abre el puerto del **adaptador externo**: COMx en Windows o `/dev/ttyUSBx` normalmente en Linux. Identifícalo conectando/desconectando solo ese adaptador; el nombre no está garantizado.
5. Configura **9600 baud, 8 bits, sin paridad, 1 stop, sin RTS/CTS ni XON/XOFF**, eco local apagado. Enter puede ser CR, LF o CRLF.
6. En PuTTY: Connection type Serial, puerto y velocidad; en Connection > Serial ajustar 8/N/1/None. En Linux una alternativa es `python -m serial.tools.miniterm /dev/ttyUSB0 9600 --eol CR` después de instalar pyserial en un entorno virtual.
7. Presiona Enter si no viste el mensaje de arranque. Debe aparecer `UART>`.

Si falta permiso para el puerto, revisa su grupo con `ls -l /dev/ttyUSB0` y agrega tu usuario a ese grupo según tu distribución. Cierra otras terminales que usen el mismo puerto. No se requiere ejecutar toda la aplicación como root.

## Relación con el proyecto del LDR

Se revisó como referencia `Proyecto_ADC_Equipo1_LDR.zip`. Para repetir la dinámica de `luz regs` / `luz sdk`, también se puede seleccionar la implementación en el propio comando:

```text
rs232 regs hola desde registros
rs232 sdk hola desde SDK
```

La selección queda activa para el resto de la consola. Las palabras `regs` y `sdk` al inicio del mensaje se reservan como selectores y requieren texto después. `rs232 hola` conserva el envío con el modo actual. En este último formato una palabra desconocida se interpreta como texto, no como un backend inválido; `uart otro` sí se rechaza como modo inválido.

Consulta `docs/DEL_LDR_A_UART.md` para relacionar periférico, registros, SDK, hardware y argumentos con el trabajo anterior.

## Uso del REPL

| Comando | Resultado |
|---|---|
| `help` | Lista los comandos |
| `uart info` | Modo, reloj, base, divisores y configuración |
| `uart regs` | Reinicializa UART0 por punteros y usa E/S por registros |
| `uart sdk` | Reinicializa mediante SDK y usa E/S SDK |
| `rs232 hola desde la Pico` | Responde `[RS232] hola desde la Pico` |
| `on`, `off`, `toggle` | Controlan LED externo GP15 |
| `read`, `status` | Botón GP13 y estado de GPIO |
| `sio`, `mask` | Registros y máscara heredados |
| `timer_on 1`, `timer_off` | Temporizador heredado; cuenta disparos al detener |

Arranca en `regs`. Espera al prompt antes de otro comando, especialmente tras cambiar de backend: se reinicia el receptor y podrían perderse bytes enviados durante el cambio. Línea máxima 63 caracteres ASCII y 16 argumentos contando el comando. Si se excede la línea, se descarta completa. No hay comillas, escapes, historial ni soporte de acentos en los comandos. `rs232` reconstruye el mensaje separando palabras con un espacio. Se rechazan modo desconocido, argumentos faltantes o adicionales en `uart`, exceso de argumentos y temporizadores no enteros. Los comandos heredados de GPIO conservan su comportamiento simple.

## Registros y SDK

En `src/uart_link.c`, `REG(base,off)` convierte una dirección en `volatile uint32_t *` y desreferencia el puntero. UART0 se resetea por alias SET/CLR, se esperan sus bits RESET_DONE y se escriben divisores, formato y control. TX espera TXFF=0; RX espera RXFE=0. Los errores de DR se descartan y cuentan en modo registros.

El modo SDK usa `uart_init`, `uart_set_format`, `uart_set_hw_flow`, `uart_set_fifo_enabled`, `uart_putc_raw` y `uart_getc`. `uart info` lee registros en ambos modos para comparar. No se utiliza IRQ UART ni DMA. En SDK se deshabilita explícitamente DMACR por registro porque `uart_init` habilita solicitudes DREQ por defecto. El conteo de errores es exclusivo del modo registros, no una comparación de tasas de error.

**Alcance honesto del acceso directo:** los registros de UART y su reset se manipulan por punteros en `regs`; el arranque, reloj, multiplexor GPIO y temporizador siguen apoyándose en infraestructura Pico SDK. No es un firmware totalmente independiente del SDK. El modo SDK produce la misma comunicación nominal; su manejo de errores no es idéntico al de `regs`.

## Diferencias entre plataformas

| Elemento | RP2040 | RP2350 |
|---|---|---|
| UART0_BASE | 0x40034000 | 0x40070000 |
| UART1_BASE | 0x40038000 | 0x40078000 |
| RESETS_BASE | 0x4000C000 | 0x40020000 |
| RESET UART0 | bit 22 | bit 26 |
| IO_BANK0_BASE | 0x40014000 | 0x40028000 |
| IRQ del TIMER usado | TIMER_IRQ_0 | TIMER0_IRQ_0 |
| Procesador en este build | Cortex-M0+ | Cortex-M33 |
| Configuración UART usada | 9600 8N1 | 9600 8N1 |

Los offsets y campos UART usados son comunes. Hay dos UART y FIFO de 32 entradas en ambas. El código utiliza macros del SDK específicas de cada chip, no direcciones copiadas de RP2040. El reloj periférico se consulta, no se asume igual a la frecuencia máxima de CPU. Los pines físicos elegidos coinciden en ambas placas.

## Demostración y entrega

Sigue `docs/PRUEBAS.md`, repite los dos modos en cada placa y guarda fotos/video y logs auténticos en `evidencias/`. La carpeta `firmware/` incluye los binarios si se pudieron compilar y su estado está en `docs/VALIDACION.md`. Compilar no demuestra funcionamiento eléctrico.

La presentación está en `docs/Presentacion_UART.pdf` y su versión editable en PPTX. El guion incluye distribución para ambas integrantes y 45 minutos de exposición. El mapa completo está en `docs/REGISTROS.md`. Para dudas técnicas, usar `docs/PREGUNTAS.md`.

Sube este contenido a un repositorio accesible al grupo, sin `build*` ni la copia del SDK. El paquete no publica por sí solo un repositorio. Agrega la URL real y las evidencias después de probar. No inventar resultados ni entregar plantillas de evidencia como si fueran pruebas realizadas.
