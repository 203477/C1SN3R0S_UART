# Plan de pruebas y evidencia real

Estado inicial: pruebas físicas pendientes. Cada fila debe repetirse en RP2040 y RP2350 y, cuando aplique, en `regs` y `sdk`. No llenar resultados como aprobados sin medirlos.

| ID | Acción | Resultado esperado | Resultado real / archivo |
|---|---|---|---|
| P01 | Compilar ambas placas | ELF y UF2 separados | Ver VALIDACION.md |
| P02 | Terminal 9600 8N1, Enter | Prompt UART> por adaptador externo | Pendiente |
| P03 | uart info | Base correspondiente a la placa, LCR_H=70, CR=301 | Pendiente |
| P04 | rs232 hola | [RS232] hola después del eco | Pendiente |
| P05 | uart sdk, rs232 prueba sdk | Respuesta por mismo circuito | Pendiente |
| P06 | uart regs, rs232 prueba regs | Respuesta por mismo circuito | Pendiente |
| P07 | on, off, toggle | LED externo cambia | Pendiente |
| P08 | read con botón libre/presionado | 1 libre, 0 presionado | Pendiente |
| P09 | uart otro / uart / rs232 | Mensaje de uso, sin bloqueo | Pendiente |
| P10 | timer_on abc / 0 / 4295 / 1x | Rechazo sin iniciar | Pendiente |
| P11 | Línea de 64 caracteres | Error y descarte completo | Pendiente |
| P12 | Enter CR, LF y CRLF | Un comando por línea | Pendiente |
| P13 | timer_on 1, esperar, timer_off | LED cambia, contador >0 | Pendiente |
| P14 | Reinicio de alimentación | Arranque en regs, sin USB CDC | Pendiente |
| P15 | 17 palabras separadas | Rechazo por exceso de argumentos | Pendiente |
