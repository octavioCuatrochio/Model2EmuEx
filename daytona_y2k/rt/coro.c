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

#elif defined(__ANDROID__) && defined(__arm__)
/* Android has no ucontext functions: a stack switch by hand (32-bit ARM,
   AAPCS: r4-r11, lr and d8-d15 are the callee's to keep). */
#include <stddef.h>

struct coro {
    void *sp, *host_sp;   /* saved stack pointers (coro_switch frames) */
    void *stack;
    void (*fn)(void *);
    void *arg;
};

/* Saves the callee-saved registers on the current stack, stores sp in
   *save, and resumes the frame at `to` */
__attribute__((naked, noinline)) static void coro_switch(void **save, void *to)
{
    __asm__ volatile(
        "push {r4-r11, lr}\n"
        "vpush {d8-d15}\n"
        "mov r2, sp\n"
        "str r2, [r0]\n"
        "mov sp, r1\n"
        "vpop {d8-d15}\n"
        "pop {r4-r11, pc}\n");
}

__attribute__((used, noreturn)) void m2_coro_start(coro *c)
{
    c->fn(c->arg);
    abort();   /* fn must not return */
}

/* a new coroutine's first frame "returns" here with the coro in r4 */
__attribute__((naked, noinline)) static void coro_boot(void)
{
    __asm__ volatile(
        "mov r0, r4\n"
        "bl m2_coro_start\n");
}

coro *coro_create(void (*fn)(void *), void *arg, size_t stack_size)
{
    coro *c = calloc(1, sizeof *c);
    if (!c)
        return NULL;
    c->stack = malloc(stack_size);
    if (!c->stack) {
        free(c);
        return NULL;
    }
    c->fn = fn;
    c->arg = arg;
    /* the frame coro_switch pops: d8-d15, then r4-r11 and pc; the stack
       is 8-byte aligned once it is popped */
    uintptr_t top = ((uintptr_t)c->stack + stack_size) & ~(uintptr_t)15;
    uint32_t *f = (uint32_t *)(top - (16 + 9) * 4);
    for (int i = 0; i < 16 + 9; i++)
        f[i] = 0;
    f[16] = (uint32_t)(uintptr_t)c;                 /* r4 */
    f[16 + 8] = (uint32_t)(uintptr_t)coro_boot;     /* pc (Thumb bit included) */
    c->sp = f;
    return c;
}

void coro_free(coro *c)
{
    if (!c)
        return;
    free(c->stack);
    free(c);
}

void coro_resume(coro *c) { coro_switch(&c->host_sp, c->sp); }
void coro_yield(coro *c)  { coro_switch(&c->sp, c->host_sp); }

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
