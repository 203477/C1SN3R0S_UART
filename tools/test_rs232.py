#!/usr/bin/env python3
"""Prueba host del handler real con transporte simulado; no prueba el circuito."""
from pathlib import Path
import subprocess,tempfile
s=(Path(__file__).resolve().parents[1]/'src/main.c').read_text()
handler=s[s.index('static int cmd_rs232(int argc, char **argv) {'):]
c=r'''
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <assert.h>
enum { LINK_REGS, LINK_SDK };
static int mode=LINK_REGS, changes=0;
static char output[1024];
static void link_init(int m) {mode=m;++changes;}
static int capture(const char *fmt,...) {
 va_list ap;va_start(ap,fmt);int n=vsnprintf(output+strlen(output),sizeof(output)-strlen(output),fmt,ap);va_end(ap);return n;
}
#define printf capture
'''+handler+r'''
int main(void) {
 char *r[]={"rs232","regs","hola"};assert(cmd_rs232(3,r)==0);assert(mode==LINK_REGS);assert(strstr(output,"[RS232] hola\n"));
 output[0]=0;char *s[]={"rs232","sdk","hola","mundo"};assert(cmd_rs232(4,s)==0);assert(mode==LINK_SDK);assert(strstr(output,"[RS232] hola mundo\n"));
 int before=changes;char *missing[]={"rs232","regs"};assert(cmd_rs232(2,missing)==1);assert(changes==before);
 char *missing_sdk[]={"rs232","sdk"};assert(cmd_rs232(2,missing_sdk)==1);assert(changes==before);
 output[0]=0;char *plain[]={"rs232","mensaje"};assert(cmd_rs232(2,plain)==0);assert(changes==before);assert(strstr(output,"[RS232] mensaje\n"));
 char *none[]={"rs232"};assert(cmd_rs232(1,none)==1);assert(changes==before);
 return 0;
}
'''
with tempfile.TemporaryDirectory() as d:
 f=Path(d)/'test.c';f.write_text(c);exe=Path(d)/'test'
 subprocess.run(['cc','-std=c11','-Wall','-Wextra','-Werror',str(f),'-o',str(exe)],check=True)
 subprocess.run([str(exe)],check=True)
print('PASS: regs, sdk, multiword, missing text without reinit, current mode, missing arguments')
