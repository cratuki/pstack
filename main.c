#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>


// --------------------------------------------------------
//  stack
// --------------------------------------------------------
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
    *(stack.top) = 0xffffffffffffffff;
}

void stack_print(char c) {
    uint64_t stack_size = stack.top - stack.mem;
    printf("{%c%14d      n dec                  hex\n", c, stack_size);
    for (int i=0; i<stack_size+1; i++) {
        printf("  %p %6" PRIu64 " %-20" PRIu64 " %-16" PRIx64 "\n", stack.mem+i, i, *(stack.mem+i), *(stack.mem+i));
    }
    printf("}\n");
}


// --------------------------------------------------------
//  debug
// --------------------------------------------------------
#define ASSERT_AT(offset, v) assert(offset, v, __LINE__)
#define ASSERT(v) assert(0, v, __LINE__)
#define ASSERT_STACK_EMPTY() assert_stack_empty(__LINE__);

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

void assert_stack_empty(uint64_t line) {
    if (stack.top != stack.mem) {
        printf("ERROR: Stack not empty (line %d). %d %d", line, stack.top, stack.mem);
        exit(1);
    }
}


// --------------------------------------------------------
//  structures
// --------------------------------------------------------



// --------------------------------------------------------
//  operators
// --------------------------------------------------------
void add() { *(stack.top) += *(stack.top--); }

void eval() { ((void (*)()) *(stack.top--))(); }

void peek_u64() {
    printf("%" PRIu64 "\n", *stack.top);
}

void pop() {
    stack.top--;
}

void push_u64(uint64_t v) { *(++stack.top) = v; }

void push_fn(void (*fn)()) { *(++stack.top) = (uint64_t) fn; }

void time_nanosleep() {
    // Half a second is 500000000L
    // You can't do one second or more. See nanosleep(2).
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = *(stack.top--);
    nanosleep(&ts, &ts);
}

void unistd_sleep() {
    sleep(*(stack.top--));
}


// --------------------------------------------------------
//  alg
// --------------------------------------------------------
void test() {
    // test:push_u64
    // test:add
    push_u64(8); ASSERT(8);
    push_u64(15); ASSERT(15);
    add(); ASSERT(23);
    pop();
    ASSERT_STACK_EMPTY();

}

void alg() {
    ASSERT_STACK_EMPTY();

    push_u64(5);
    push_u64(6);

    push_fn(&add);
    stack_print('a');

    eval();
    peek_u64();

    push_u64(2);
}

int main() {
    stack_init(500);

    test();
    alg();

    return 0;
}
