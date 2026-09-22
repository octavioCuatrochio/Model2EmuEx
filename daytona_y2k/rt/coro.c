#include "coro.h"

#include <stdint.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>

struct coro {
    void *fiber, *host;
    void (*fn)(void *);
    void *arg;
};

static VOID CALLBACK fiber_main(LPVOID p)
{
    coro *c = p;
    c->fn(c->arg);
}

coro *coro_create(void (*fn)(void *), void *arg, size_t stack_size)
{
    coro *c = calloc(1, sizeof *c);
    if (!c)
        return NULL;
    c->fn = fn;
    c->arg = arg;
    c->fiber = CreateFiber(stack_size, fiber_main, c);
    if (!c->fiber) {
        free(c);
        return NULL;
    }
    return c;
}

void coro_free(coro *c)
{
    if (!c)
        return;
    DeleteFiber(c->fiber);
    free(c);
}

void coro_resume(coro *c)
{
    /* the board may run on any thread: make this one a fiber on first use */
    if (!IsThreadAFiber())
        ConvertThreadToFiber(NULL);
    c->host = GetCurrentFiber();
    SwitchToFiber(c->fiber);
}

void coro_yield(coro *c)
{
    SwitchToFiber(c->host);
}

#else
#include <ucontext.h>

struct coro {
    ucontext_t ctx, host;
    void *stack;
    void (*fn)(void *);
    void *arg;
};

static void trampoline(unsigned lo, unsigned hi)
{
    coro *c = (coro *)(((uintptr_t)hi << 16 << 16) | (uintptr_t)lo);
    c->fn(c->arg);
}

/* getcontext in a function of its own: it returns twice as far as the
   compiler knows, which would make locals of the caller suspect */
static int get_ctx(ucontext_t *u) { return getcontext(u); }

coro *coro_create(void (*fn)(void *), void *arg, size_t stack_size)
{
    coro *c = calloc(1, sizeof *c);
    if (!c)
        return NULL;
    c->stack = malloc(stack_size);
    if (!c->stack || get_ctx(&c->ctx)) {
        free(c->stack);
        free(c);
        return NULL;
    }
    c->fn = fn;
    c->arg = arg;
    c->ctx.uc_stack.ss_sp = c->stack;
    c->ctx.uc_stack.ss_size = stack_size;
    c->ctx.uc_link = NULL;
    uintptr_t p = (uintptr_t)c;
    makecontext(&c->ctx, (void (*)(void))trampoline, 2, (unsigned)(p & 0xffffffffu), (unsigned)(p >> 16 >> 16));
    return c;
}

void coro_free(coro *c)
{
    if (!c)
        return;
    free(c->stack);
    free(c);
}

void coro_resume(coro *c) { swapcontext(&c->host, &c->ctx); }
void coro_yield(coro *c)  { swapcontext(&c->ctx, &c->host); }
#endif
