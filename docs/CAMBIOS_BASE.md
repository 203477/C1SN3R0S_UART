# Relación con main.c proporcionado

Se conserva la organización del curso: command_t, tabla commands, argc/argv, split_line, execute_command y handlers de LED, botón, SIO y TIMER. La copia original conserva autor y fecha. Las integrantes figuran como autoras de la adaptación.

Cambios: backend UART explícito en archivo separado; printf/putchar/getchar del monitor se redirigen a funciones del enlace; se elimina stdio_init_all; USB CDC y stdio UART automático se deshabilitan en CMake. Nuevos comandos uart y rs232. LED fijo GP15 externo en ambas tarjetas. Botón pasa de pull-down a pull-up y se conecta a GND. read_line acepta CR/LF/CRLF, retroceso y rechaza líneas largas completas. split_line detecta exceso de tokens. timer_on valida entero completo y rango. ISR TIMER ya no hace printf, por lo que no entremezcla transmisiones con el REPL. El contador se muestra al detener.

No se activa la interrupción de botón comentada en el original. UART usa polling; la interrupción existente corresponde exclusivamente al TIMER. No se utiliza segundo núcleo, Wi-Fi ni Bluetooth.
