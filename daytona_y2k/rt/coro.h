/*
 * Minimal coroutines: the recompiled program runs on its own stack, and
 * gives the CPU back to the board at the end of each time slice.
 * POSIX: ucontext; Windows: fibers.
 */
#ifndef CORO_H
#define CORO_H

#include <stddef.h>

typedef struct coro coro;

/* fn(arg) starts at the first coro_resume; it must never return. */
coro *coro_create(void (*fn)(void *), void *arg, size_t stack_size);
void  coro_free(coro *c);
/* From the host: run the coroutine until it yields. */
void  coro_resume(coro *c);
/* From inside the coroutine: back to the coro_resume caller. */
void  coro_yield(coro *c);

#endif
