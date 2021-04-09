#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>



/* --------------------------------------------------------
  macro
-------------------------------------------------------- */
#define ASSERT_AT(offset, v) assert(offset, v, __LINE__)
#define ASSERT(v) assert(0, v, __LINE__)


/* --------------------------------------------------------
  stack
-------------------------------------------------------- */
typedef struct Stack_st {
    uint64_t* mem;
    size_t memsize;
    uint64_t* top;
} Stack;

static Stack stack;

void stack_init(size_t memsize) {
    stack.mem = (uint64_t*) malloc(memsize * sizeof(uint64_t));
    stack.memsize = memsize;
    stack.top = stack.mem;
}

void stack_print(char c) {
    uint64_t stack_size = stack.top - stack.mem;
    printf("{ %c %d \n", c, stack_size);
    for (int i=0; i<stack_size+1; i++) {
        printf("  %p %d %d\n", stack.mem+i, i, *(stack.mem+i));
    }
    printf("}\n");
}


/* --------------------------------------------------------
  structures
-------------------------------------------------------- */


/* --------------------------------------------------------
  operators
-------------------------------------------------------- */
void assert(uint64_t offset, uint64_t v, uint64_t line) {
    uint64_t stack_size = stack.top - stack.mem;
    if (offset > stack_size) {
        printf("ERROR: Offset %d is larger than stack size %d\n", offset, stack_size);
        exit(1);
    }
    if (*(stack.top-offset) != v) {
        printf("ERROR: Assert failed at %d. (%d)\n", line, *stack.top);
        exit(1);
    }
}

void push_u64(uint64_t v) { *(++stack.top) = v; }

void push_fn(void (*fn)()) { *(++stack.top) = (uint64_t) fn; }

void add() { *(stack.top) += *(stack.top--); }

void eval() { ((void (*)()) *(stack.top--))(); }

void peek_u64() {
    printf("%d\n", *stack.top);
}


/* --------------------------------------------------------
  alg
-------------------------------------------------------- */
int main() {
    stack_init(500);

    push_u64(5); ASSERT_AT(0, 5);
    push_u64(6); ASSERT_AT(0, 6);

    push_fn(&add);
    stack_print('a');

    eval(); ASSERT(11);

    push_u64(7); ASSERT(7);
    add();
    peek_u64();

    return 0;
}
