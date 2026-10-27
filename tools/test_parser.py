#!/usr/bin/env python3
"""Prueba host de las funciones reales read_line y split_line (requiere cc)."""
from pathlib import Path
import subprocess, tempfile
source=(Path(__file__).resolve().parents[1]/'src/main.c').read_text()
parser=source[source.index('static void read_line('):source.index('static void execute_command(')]
harness=r'''
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
static const char *input;
static int fake_get(void) { assert(*input); return (unsigned char)*input++; }
static int fake_put(int c) { return c; }
static int fake_print(const char *fmt, ...) { (void)fmt; return 0; }
#define getchar fake_get
#define putchar fake_put
#define printf fake_print
'''+parser+r'''
int main(void) {
 char b[64]; char *args[16];
 input="rs232 hola\r\nuart info\n";
 read_line(b,sizeof b); assert(strcmp(b,"rs232 hola")==0);
 read_line(b,sizeof b); assert(strcmp(b,"uart info")==0);
 input="helx\bp\n";read_line(b,sizeof b);assert(strcmp(b,"help")==0);
 char longline[66];memset(longline,'a',64);longline[64]='\n';longline[65]=0;
 input=longline;read_line(b,sizeof b);assert(b[0]==0);
 char exact[65];memset(exact,'a',63);exact[63]='\n';exact[64]=0;
 input=exact;read_line(b,sizeof b);assert(strlen(b)==63);
 char many[]="a b c d e f g h i j k l m n o p q";
 assert(split_line(many,args,16)==-1);
 char valid[]="rs232 hola desde la Pico";
 assert(split_line(valid,args,16)==5);assert(strcmp(args[1],"hola")==0);
 char empty[]=" \t ";assert(split_line(empty,args,16)==0);
 return 0;
}
'''
with tempfile.TemporaryDirectory() as d:
 c=Path(d)/'test.c'; c.write_text(harness)
 exe=Path(d)/'test'
 subprocess.run(['cc','-std=c11','-Wall','-Wextra','-Werror',str(c),'-o',str(exe)],check=True)
 subprocess.run([str(exe)],check=True)
print('PASS: CRLF, LF, backspace, overflow, exact length, token overflow, argc/argv, empty line')
