#include <stdio.h>

extern void foo(void);
extern void bar(void);

extern unsigned long __start_mcount_loc[];
extern unsigned long __stop_mcount_loc[];

struct dyn_ftrace {
    union {
        unsigned long        ip; /* address of mcount call-site */
        struct dyn_ftrace    *freelist;
    };
    unsigned long        flags;
    // struct dyn_arch_ftrace        arch;
};

#define PAGE_SIZE 4 * 1024
#define ENTRY_SIZE sizeof(struct dyn_ftrace)
#define ENTRIES_PER_PAGE (PAGE_SIZE / ENTRY_SIZE)

static void debug(void);
static void ftrace_init(void);
static int ftrace_dyn_table_alloc(unsigned long num_to_init);

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

static void ftrace_init(void)
{
    unsigned long count;
    int ret;

    count = __stop_mcount_loc - __start_mcount_loc;
    ret = ftrace_dyn_table_alloc(count);
}

static int ftrace_dyn_table_alloc(unsigned long num_to_init)
{
    int cnt;

    if (!num_to_init) {
        fprintf(stderr, "ftrace: No functions to be traced?\n");
        return -1;
    }

    cnt = num_to_init / ENTRIES_PER_PAGE;
    fprintf(stderr, "ftrace: allocating %ld entries in %d pages\n",
        num_to_init, cnt + 1);

    return 0; 	
}
