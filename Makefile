src = $(wildcard *.c)
obj = $(src:.c=.o)

CC?=        cc
CFLAGS+=    -Wall -Og -pg -g -std=c11
LDFLAGS =   -Thfw.lds  

.PHONY: all clean 

all: hfw 

hfw: $(obj)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<
	@./recordmcount.pl "x86_64" ""  "64" "objdump" "objcopy" "cc" "ld" "nm" "" "" "0" $@ > /dev/null 2>&1

clean:
	rm -f $(obj) hfw 
