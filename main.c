#include <stdio.h>
#include <stdlib.h>

extern void foo(void);
extern void bar(void);

extern unsigned long __start_mcount_loc[];
extern unsigned long __stop_mcount_loc[];

static void debug(void);
extern void ftrace_init(void);

int main(int argc, char *argv[])
{
    debug();

    ftrace_init();

    return 0;
}

static void debug(void)
{
    unsigned long *call_p;

    fprintf(stderr, "__stop_mcount_loc = %p, __start_mcount_loc = %p, size = %ld\n", 
	__stop_mcount_loc, __start_mcount_loc, __stop_mcount_loc - __start_mcount_loc);

    call_p = __start_mcount_loc;
    do {
	fprintf(stderr, "*call_p = %p\n", (void*)*call_p);
        ++call_p;
    } while (call_p < __stop_mcount_loc);

    fprintf(stderr, "foo+4: %p, bar+4: %p, main+9: %p\n",
	&foo+4, &bar+4, &main+9);
}

