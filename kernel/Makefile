CFLAGS?=-O2 -g
CPPFLAGS?=
LDFLAGS?=-T linker.ld
LIBS?=

DESTDIR?=
PREFIX?=/usr/local
EXEC_PREFIX?=$(PREFIX)
BOOTDIR?=$(EXEC_PREFIX)/boot
INCLUDEDIR?=$(PREFIX)/include

CFLAGS:=$(CFLAGS) -ffreestanding -fbuiltin -Wall -Wextra -Wno-format
CPPFLAGS:=$(CPPFLAGS) -D_KERNEL_ -Iinclude
LDFLAGS:=$(LDFLAGS)
LIBS:=$(LIBS) -nostdlib -lgcc

OBJS:=$(patsubst %.c,%.o,$(wildcard src/*/*/*.c))
OBJS+=$(patsubst %.c,%.o,$(wildcard src/*/*.c))
OBJS+=$(patsubst %.c,%.o,$(wildcard src/*.c))

OBJS+=$(patsubst %.S,%.o,$(wildcard src/*/*/*.S))
OBJS+=$(patsubst %.S,%.o,$(wildcard src/*/*.S))
OBJS+=$(patsubst %.S,%.o,$(wildcard src/*.S))

OBJS+=$(patsubst %.asm,%.o,$(wildcard src/*/*/*.asm))
OBJS+=$(patsubst %.asm,%.o,$(wildcard src/*/*.asm))
OBJS+=$(patsubst %.asm,%.o,$(wildcard src/*.asm))


all: OGOS.kernel

.PHONY: all clean install install-headers install-kernel

OGOS.kernel: $(OBJS) linker.ld
	$(CC) -o $@ $(CFLAGS) $(OBJS) $(LDFLAGS) $(LIBS)

%.o: %.c
	$(CC) -c $< -o $@ -include stddef.h -std=gnu11 $(CFLAGS) $(CPPFLAGS)

%.o: %.S
	$(CC) -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

%.o: %.asm
	nasm -f elf $< -o $@


clean:
	rm -f OGOS.kernel $(OBJS) *.o */*.o */*/*.o

install: install-headers install-kernel

install-headers:
	mkdir -p $(DESTDIR)$(INCLUDEDIR)
	cp -RTv include $(DESTDIR)$(INCLUDEDIR)

install-kernel: OGOS.kernel
	mkdir -p $(DESTDIR)$(BOOTDIR)
	cp OGOS.kernel $(DESTDIR)$(BOOTDIR)
