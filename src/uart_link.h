#pragma once
#include <stdbool.h>
#include <stdint.h>
typedef enum { LINK_REGS, LINK_SDK } link_mode_t;
void link_init(link_mode_t mode);
void link_flush(void);
void link_putc(char c);
int link_getc(void);
int console_putc(int c);
int console_printf(const char *format, ...);
void link_report(void);
