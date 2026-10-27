// UART0: GP0 TX, GP1 RX. Solo MAX3232 a 3.3 V en el lado logico.
#include "uart_link.h"
#include <stdio.h>
#include <stdarg.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/clocks.h"
#include "hardware/regs/addressmap.h"
#include "hardware/regs/resets.h"
#define BAUD 9600u
// Desreferenciacion explicita de punteros volatile de 32 bits.
#define REG(base,off) (*(volatile uint32_t *)((uintptr_t)(base)+(off)))
#define U(off) REG(UART0_BASE,off)
#define DR 0x000u
#define ECR 0x004u
#define FR 0x018u
#define IBRD 0x024u
#define FBRD 0x028u
#define LCRH 0x02cu
#define CR 0x030u
#define IMSC 0x038u
#define ICR 0x044u
#define DMACR 0x048u
static link_mode_t active;
static bool initialized;
static uint32_t rx_errors;
void link_flush(void) {
    if (!initialized) return;
    if (active == LINK_SDK) uart_tx_wait_blocking(uart0);
    else while (U(FR) & (1u << 3)) tight_loop_contents(); // BUSY
}
void link_init(link_mode_t mode) {
    link_flush(); // Terminar respuesta anterior antes de resetear UART.
    // Infraestructura comun SDK: reloj de arranque y multiplexor de GPIO.
    gpio_set_function(0, GPIO_FUNC_UART);
    gpio_set_function(1, GPIO_FUNC_UART);
    if (mode == LINK_REGS) {
        // Alias atomicos SET (+0x2000) y CLR (+0x3000) de RESET.
        REG(RESETS_BASE, 0x2000) = RESETS_RESET_UART0_BITS;
        REG(RESETS_BASE, 0x3000) = RESETS_RESET_UART0_BITS;
        while (!(REG(RESETS_BASE, 0x008) & RESETS_RESET_UART0_BITS))
            tight_loop_contents();
        U(CR) = 0;
        U(IMSC) = 0; U(ICR) = 0x7ff; U(ECR) = 0; U(DMACR) = 0;
        // D = clk_peri/(16*baud), redondeado a pasos de 1/64.
        uint32_t d64 = (uint32_t)(((uint64_t)clock_get_hz(clk_peri)*4u + BAUD/2u)/BAUD);
        U(IBRD) = d64/64u;
        U(FBRD) = d64%64u;
        U(LCRH) = (3u << 5) | (1u << 4); // WLEN=8, FIFO, N, 1 stop
        U(CR) = (1u<<9) | (1u<<8) | 1u; // RXE TXE UARTEN
    } else {
        uart_init(uart0, BAUD);
        uart_set_hw_flow(uart0, false, false);
        uart_set_format(uart0, 8, 1, UART_PARITY_NONE);
        uart_set_fifo_enabled(uart0, true);
        uart_set_irq_enables(uart0, false, false);
        // DMA no participa. uart_init habilita DREQ por defecto en SDK 2.2.0.
        U(DMACR) = 0;
    }
    active = mode; initialized = true;
}
void link_putc(char c) {
    if (active == LINK_SDK) uart_putc_raw(uart0,c);
    else { while (U(FR)&(1u<<5)) tight_loop_contents(); U(DR)=(uint8_t)c; }
}
int link_getc(void) {
    if (active == LINK_SDK) return (unsigned char)uart_getc(uart0);
    for (;;) {
        while (U(FR)&(1u<<4)) tight_loop_contents(); // RXFE
        uint32_t value=U(DR);
        if (value & 0xf00u) { ++rx_errors; U(ECR)=0; continue; }
        return (int)(value&0xffu);
    }
}
int console_putc(int c) { if(c=='\n') link_putc('\r'); link_putc((char)c); return c; }
int console_printf(const char *format, ...) {
    char buf[512]; va_list ap; va_start(ap,format);
    int n=vsnprintf(buf,sizeof(buf),format,ap); va_end(ap);
    if(n<0) return n;
    for(size_t i=0;i<sizeof(buf)-1 && buf[i];++i) console_putc(buf[i]);
    return n;
}
void link_report(void) {
    console_printf("modo=%s UART0=0x%08lx clk_peri=%lu baud=9600 8N1 TX=GP0 RX=GP1\n",
      active==LINK_REGS?"regs":"sdk",(unsigned long)UART0_BASE,(unsigned long)clock_get_hz(clk_peri));
    console_printf("IBRD=%lu FBRD=%lu LCR_H=0x%lx CR=0x%lx FR=0x%lx\n",
      (unsigned long)U(IBRD),(unsigned long)U(FBRD),(unsigned long)U(LCRH),(unsigned long)U(CR),(unsigned long)U(FR));
    console_printf("IMSC=0x%lx DMACR=0x%lx errores_RX_regs=%lu\n",
      (unsigned long)U(IMSC),(unsigned long)U(DMACR),(unsigned long)rx_errors);
}
