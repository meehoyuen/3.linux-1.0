
HEAD = head.o
SYSTEM = ../tools/zSystem
#LD = gcc
#TEST = -DTEST_DRIVER

zOBJECTS = $(HEAD) inflate.o unzip.o misc.o

CFLAGS = -O2 -m32 -fno-stack-protector -DSTDC_HEADERS $(TEST)

.c.s:
	$(CC) $(CFLAGS) -S -o $*.s $<
.s.o:
	$(AS) -c -o $*.o $<
.c.o:
	$(CC) $(CFLAGS) -c -o $*.o $<

all:	zSystem

zSystem:piggy.o $(zOBJECTS)
	$(LD) $(LDFLAGS) -o zSystem -Ttext 0x1000 -e startup_32 $(zOBJECTS) piggy.o

head.o:	head.s

head.s: head.S ../include/linux/tasks.h
	$(CPP) -traditional head.S -o head.s
xtract: xtract.c
	$(CC) -o $@ $<
piggy.o: $(SYSTEM) xtract piggyback
	./xtract $(SYSTEM) > ../vmlinux
	./xtract $(SYSTEM) | gzip -9 | ./piggyback > piggy.o

$(SYSTEM):
	$(MAKE) -C .. tools/zSystem
