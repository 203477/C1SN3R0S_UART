# Validación del paquete

Fecha de construcción: 26 de octubre de 2026.

## Completado en el entorno de desarrollo

- Compilación y enlace para `PICO_BOARD=pico` (RP2040) y `PICO_BOARD=pico2_w` (RP2350 ARM Secure).
- Pico SDK 2.2.0, GNU Arm Embedded GCC 13.2.1, CMake 3.28.3, compilación Release.
- Archivos ELF y UF2 generados por el SDK y picotool. Metadatos revisados con `picotool info -a`: chip y placa correctos.
- Estructura de bloques UF2 y SHA256 verificados. Pico2W puede incluir un bloque con familia de compatibilidad además de la familia RP2350 ARM Secure; es salida de picotool.
- Pruebas host sobre las funciones reales read_line y split_line: CRLF, LF, retroceso, línea de 63 caracteres, rechazo de 64, exceso de tokens, argc/argv y línea vacía.
- CMake deshabilita stdio USB y stdio UART automático. No se llama stdio_init_all. La consola usa exclusivamente UART0 mediante uart_link.

Los logs de construcción están en `evidencias/compilacion_RP2040.txt` y `evidencias/compilacion_RP2350.txt`. Las rutas temporales registradas pertenecen al entorno donde se compiló; usa README para compilar en tu computadora. Los avisos de submódulos USB/Bluetooth/Wi-Fi ausentes en la configuración son esperables: el proyecto no usa esas bibliotecas.

## Pendiente en el hardware real

Cableado y orientación de tu módulo MAX3232, identificación del adaptador, recepción/transmisión real, configuración del puerto, temporización observada, botón, LED y funcionamiento en las dos placas. No se dispone de tus dispositivos conectados para medir estos puntos. Ejecutar `docs/PRUEBAS.md` antes de presentar.

## Límites conocidos

El REPL es una consola de órdenes cortas, no un receptor de tráfico continuo. Un envío masivo puede saturar RX mientras el monitor imprime ayuda. La aplicación usa polling, sin control de flujo. Los cambios de backend reinician UART: no enviar durante el cambio. El contador de errores corresponde únicamente a la recepción por registros. El mapa UART está verificado con headers específicos de Pico SDK 2.2.0; no reutilizar direcciones de una familia en otra.

## Adaptación tomando el LDR como referencia

Se añadieron `rs232 regs <texto>` y `rs232 sdk <texto>` conservando los comandos anteriores. Ambas plataformas se recompilaron después del cambio. El handler real pasó pruebas con transporte simulado: selección regs/sdk, texto de varias palabras, argumentos faltantes sin reinicialización y conservación del modo actual. Ver evidencias/pruebas_rs232_host.txt. El circuito sigue pendiente de probar.
