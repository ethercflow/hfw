#include <stdio.h>

extern void foo(void);
extern void bar(void);

typedef void (*funcptr_t)(void);
extern funcptr_t __start_mcount_loc, __stop_mcount_loc;

static void debug(void);

int main(int argc, char *argv[])
{
    debug();

    return 0;
}

static void debug(void)
{
    funcptr_t *call_p;

    fprintf(stderr, "__stop_mcount_loc = %p, __start_mcount_loc = %p, size = %ld\n", 
	&__stop_mcount_loc, &__start_mcount_loc, &__stop_mcount_loc - &__start_mcount_loc);

    call_p = &__start_mcount_loc;
    do {
	fprintf(stderr, "*call_p = %p\n", *call_p);
        ++call_p;
    } while (call_p < &__stop_mcount_loc);

    fprintf(stderr, "foo+4: %p, bar+4: %p, main+9: %p\n",
	&foo+4, &bar+4, &main+9);
}
