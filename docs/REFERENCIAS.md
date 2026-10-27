# Referencias técnicas

Consulta: 26 de octubre de 2026. La implementación se fija a Pico SDK 2.2.0 para reproducibilidad.

1. Raspberry Pi. RP2040 Datasheet. Capítulo UART, mapa de memoria, RESETS, IO_BANK0 y SIO. https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf
2. Raspberry Pi. RP2350 Datasheet. Secciones UART, mapa de memoria y resets. https://datasheets.raspberrypi.com/rp2350/rp2350-datasheet.pdf
3. Raspberry Pi. Pico SDK, Hardware APIs, hardware_uart, hardware_gpio y hardware_clocks. https://www.raspberrypi.com/documentation/pico-sdk/hardware.html
4. Raspberry Pi. Pico SDK 2.2.0, fuente hardware_uart/uart.c y headers hardware_regs de cada chip. https://github.com/raspberrypi/pico-sdk/tree/2.2.0
5. Texas Instruments. MAX3232, SLLS410O, revisión junio 2021. Pinout y circuito de aplicación. https://www.ti.com/lit/ds/symlink/max3232.pdf
6. Raspberry Pi. Documentación de placas Pico. https://www.raspberrypi.com/documentation/microcontrollers/pico-series.html
7. Analog Devices. Fundamentals of RS-232 Serial Communications. Contexto y características del enlace. https://www.analog.com/en/resources/technical-articles/fundamentals-of-rs232-serial-communications.html

Las capacidades máximas dependen del fabricante y variante concreta del MAX3232. No asumir que la especificación de TI describe cualquier módulo genérico. La selección de 9600 baud evita depender de su máxima velocidad.
