# Conexión eléctrica y diagnóstico

## Antes de conectar

Distinguir el integrado MAX3232 del módulo armado. Si es un módulo, verificar su esquema y que incluya los capacitores de bomba de carga y desacoplo. No confiar solo en TX/RX impresos. DIN/T1IN es una entrada lógica al MAX3232 y se conecta al TX de la Pico; ROUT/R1OUT es una salida lógica del MAX3232 y se conecta al RX de la Pico.

Para un MAX3232 TI de 16 terminales: VCC=16, GND=15, T1IN/DIN1=11, T1OUT/DOUT1=14, R1IN=13, R1OUT=12. Confirma fabricante, encapsulado y orientación antes de usar estos números. A 3.3 V, el circuito de referencia TI usa C1 de C1+ pin1 a C1− pin3; C2 de C2+ pin4 a C2− pin5; C3 de V+ pin2 a GND; C4 de V− pin6 a GND, más desacoplo de VCC a GND. Para esta tensión, 0.1 µF por capacitor según la tabla del fabricante; preferir cerámicos no polarizados. No conectar V+ o V− a un GPIO. Si se utilizan capacitores polarizados, verificar polaridad del esquema, especialmente V−. Entradas lógicas no usadas deben quedar a nivel definido según el fabricante.

## DE-9 y sentido de las señales

La conexión documentada supone que el adaptador de PC es DTE estándar: pin2 RXD, pin3 TXD y pin5 GND. Usar la numeración moldeada del conector; la vista frontal y la vista de soldadura se reflejan. El sexo del conector no garantiza el rol DTE/DCE.

Si el módulo MAX3232 ya tiene DE-9, comprobar continuidad del conector a T1OUT y R1IN. Si T1OUT termina en pin2 y R1IN en pin3, corresponde al cableado DCE para conectar a un DTE con cable directo. Si ambas salidas están en pin3, hace falta cruzar 2 y 3 (null-modem apropiado). No añadir otro cruce a ciegas. No conectar dos salidas entre sí. No se usan RTS, CTS, DTR ni DSR.

## Pruebas por etapas

1. Adaptador separado del MAX3232: si es un USB–RS-232 DTE documentado, unir temporalmente pines2 y3 y escribir con eco local apagado. Debe volver el texto. Retirar puente. Esto comprueba el adaptador, no la Pico.
2. Verificar sin alimentación el cableado y GND común. Conectar 3.3 V al módulo.
3. Conectar trayectos de datos completos. Abrir terminal 9600 8N1 sin control de flujo y pulsar Enter.
4. Ejecutar `uart info` y `rs232 hola`, cambiar backend y repetir.

Si solo transmite un sentido, comprobar cada trayecto por separado. Si el texto es ilegible, comparar baud/formato y masa. Si se ve cada carácter duplicado, apagar eco local. Si no enciende el LED, confirmar GP15 físico20 y polaridad/resistencia. GP25 no es el LED integrado de la Pico 2 W. Si el botón siempre vale1, revisar contacto hacia GND y orientación del pulsador de cuatro patas; muchas patas del mismo lado ya están unidas.

Con un osciloscopio o analizador lógico compatible, observar GP0/GP1 solo en el lado 3.3 V. El lado RS-232 requiere una entrada que soporte tensiones positivas y negativas. UART idle lógico es alto; RS-232 idle es negativo. MAX3232 adapta niveles e invierte la polaridad, pero no interpreta comandos ni selecciona el baud.
