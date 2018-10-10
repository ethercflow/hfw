#include <stdio.h>

__attribute__((used)) static void bar0(void) {}

__attribute__((used)) static void bar1(void) {}

void bar(void) {
	fprintf(stderr, "Bar!\n");
}

void bar2(void) {}

void bar3(void) {}
