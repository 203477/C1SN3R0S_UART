# Preguntas para ambas integrantes

1. **¿UART y RS-232 son lo mismo?** UART organiza bytes y tramas asíncronas; RS-232 define una interfaz eléctrica. MAX3232 adapta los niveles.
2. **¿Por qué no USB de la Pico?** La consigna exige UART físico con RS-232. El adaptador USB está en la PC y traduce a RS-232; la Pico comunica por GP0/GP1.
3. **¿Qué hace MAX3232?** Convierte lógica 3.3 V a señales RS-232 y viceversa mediante drivers, receptores y bomba de carga. No procesa el comando.
4. **¿Base UART0?** 0x40034000 en RP2040, 0x40070000 en RP2350.
5. **¿Dónde escribes datos?** DR a offset0. Se espera TXFF=0 antes de escribir.
6. **¿Cómo recibes?** Se espera RXFE=0 y se lee DR. Esa lectura consume un byte.
7. **¿Qué es volatile?** Solicita accesos observables al registro en lugar de reutilizar un valor optimizado. No es un mutex.
8. **¿Qué vale CR?** 0x301: UARTEN, TXE y RXE en1.
9. **¿Qué vale LCR_H?** 0x70: ocho bits y FIFO activa, sin paridad y un stop.
10. **¿Qué hay en argv[1]?** En `uart regs`, la cadena "regs"; en `rs232 hola`, la cadena "hola". argc vale2 en ambos casos e incluye el nombre del comando.
11. **¿Cómo eliges baud?** Dividiendo clk_peri con IBRD/FBRD. Se lee el reloj real, no se fija la frecuencia de CPU como supuesto.
12. **¿Qué hace el SDK?** Configura los mismos registros con funciones y macros portables. Reduce errores, pero aún hay que conocer hardware y cableado.
13. **¿Qué cambia en RP2350?** Direcciones base, bit de reset UART0, CPU y nombre de la IRQ TIMER, entre otros. Los offsets UART usados son compatibles.
14. **¿Se usan IRQ UART?** No; polling. Sí hay IRQ del TIMER heredado si se activa timer_on.
15. **¿FIFO elimina cualquier pérdida?** No. Solo amortigua ráfagas; si el programa no atiende RX y se llena, puede haber overrun. El REPL no es un receptor de flujo continuo de alta tasa.
16. **¿Por qué esperar BUSY?** TXFF=0 solo significa que cabe un byte; BUSY=0 permite saber que terminó la transmisión antes del reset.
17. **¿Qué ocurre al ejecutar rs232?** La PC envía caracteres, MAX3232 los adapta, UART los recibe, el parser llama al handler y este transmite una respuesta de regreso.
18. **¿Se puede demostrar RP2040 con Pico2W?** No. El código común y la compilación no reemplazan la demostración física de la otra plataforma.
19. **¿Qué falla si alimento el módulo a 5 V?** Su salida lógica puede ser incompatible con RX de3.3 V. Este proyecto alimenta MAX3232 a 3.3 V.
20. **¿Son idénticos ambos backends?** En la función nominal sí; el modo registros además descarta bytes con errores y los cuenta. La infraestructura común sigue usando SDK.
