#include <stdio.h>
#include <stdlib.h>

extern void foo(void);
extern void bar(void);

extern unsigned long __start_mcount_loc[];
extern unsigned long __stop_mcount_loc[];

#define PAGE_SIZE 4 * 1024
#define ENTRY_SIZE sizeof(struct dyn_ftrace)
#define ENTRIES_PER_PAGE (PAGE_SIZE / ENTRY_SIZE)

struct dyn_ftrace {
    union {
        unsigned long        ip; /* address of mcount call-site */
        struct dyn_ftrace    *freelist;
    };
    unsigned long        flags;
    // struct dyn_arch_ftrace        arch;
};

struct ftrace_page {
    struct ftrace_page    *next;
    struct dyn_ftrace    *records;
    int            index;
    int            size;
};

static struct ftrace_page    *ftrace_pages_start;
static struct ftrace_page    *ftrace_pages;

static int ftrace_dyn_table_alloc(unsigned long num_to_init);
static int ftrace_process_locs(unsigned long *start, unsigned long *end);
static struct ftrace_page *ftrace_allocate_pages(unsigned long num_to_init);
static int ftrace_allocate_records(struct ftrace_page *pg);
static inline unsigned long ftrace_call_adjust(unsigned long addr);
static int ftrace_update_code(struct ftrace_page *new_pgs);

void ftrace_init(void)
{
    unsigned long count;
    int ret;

    count = __stop_mcount_loc - __start_mcount_loc;
    ret = ftrace_dyn_table_alloc(count);
    if (ret)
        goto end;
    ftrace_process_locs(__start_mcount_loc, __stop_mcount_loc);
    
end:
    return;
}

static int ftrace_allocate_records(struct ftrace_page *pg)
{
    pg->records = malloc(sizeof(*(pg->records)));
    if (!pg->records)
	return -1;
    return 1; 
}

static struct ftrace_page *
ftrace_allocate_pages(unsigned long num_to_init)
{
    struct ftrace_page *start_pg;
    struct ftrace_page *pg;
    int cnt;

    if (!num_to_init)
        return 0;

    start_pg = pg = malloc(sizeof(*pg));
    if (!pg)
        return NULL;

    for (;;) {
        cnt = ftrace_allocate_records(pg);
        if (cnt < 0)
            goto free_pages;

        num_to_init -= cnt;
        if (!num_to_init)
            break;

        pg->next = malloc(sizeof(*pg));
        if (!pg->next)
            goto free_pages;

        pg = pg->next;
    }

    return start_pg;

free_pages:
    while (start_pg) {
        free(pg->records);
        start_pg = pg->next;
        free(pg);
        pg = start_pg;
    }
    fprintf(stderr, "ftrace: FAILED to allocate memory for functions\n");
    return NULL;
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

static int ftrace_process_locs(unsigned long *start, unsigned long *end)
{
    struct ftrace_page *start_pg;
    struct ftrace_page *pg;
    struct dyn_ftrace *rec;
    unsigned long count;
    unsigned long *p;
    unsigned long addr;
    int ret = -1;

    count = end - start;

    if (!count)
        return 0;

    start_pg = ftrace_allocate_pages(count);
    if (!start_pg)
        return -1;

    ftrace_pages = ftrace_pages_start = start_pg;

    p = start;
    pg = start_pg;
    while (p < end) {
        addr = ftrace_call_adjust(*p++);
        if (!addr)
            continue;

        if (pg->index == pg->size) {
            if (!pg->next)
                break;
            pg = pg->next;
        }

        rec = &pg->records[pg->index++];
        rec->ip = addr;
    }

    ftrace_pages = pg;

    ftrace_update_code(start_pg);
    ret = 0;

    return ret;
}

static inline unsigned long ftrace_call_adjust(unsigned long addr)
{
    return addr;
}

static int ftrace_update_code(struct ftrace_page *new_pgs)
{
    fprintf(stderr, "ftrace_update_code\n");
    return 0;
}
