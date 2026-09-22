/*
 * What a function rewritten by hand has to keep (see ../README.md).
 *
 * It replaces the machine code of one i960 function, so it must leave
 * behind what that code left behind: the global registers its callers
 * read (g0-g7 carry arguments and results), every write to guest memory in
 * the same order, the condition code, and the cycles the machine code would
 * have spent, instruction by instruction: the board's time slices end, and
 * its interrupts come in, between two particular instructions. The local
 * registers r0-r15 belong to the frame and are dropped by the return.
 *
 * The cycles are paid with a cursor into the machine code: after the C that
 * does what the instructions up to address `b` did, UPTO(b) charges them one
 * by one. So the frame interrupt, which can arrive in the middle of a
 * function, sees exactly the memory it would have seen.
 *
 *   ENTRY(a)            the function's machine code starts at a
 *   UPTO(b)             the instructions from the cursor up to b have run
 *   BRANCH(b, t)        ... the last of them a branch taken to t, where the
 *                       cursor goes next. b is past the branch instruction,
 *                       so `cmpdeco` followed by `bl` is one BRANCH past
 *                       both: leaving the bl unpaid moves every later time
 *                       slice.
 *   RET()               the ret at the cursor
 *   RET_BX(target)      the bx at the cursor that ends a bal procedure
 *   TAIL(fn)            fall into another function (the cursor is at its entry)
 *   CALL(fn, entry)     the call at the cursor (it must be a call instruction)
 *   BAL(fn, entry)      the bal at the cursor
 *   CALLX(target)       the callx at the cursor, through a pointer
 */
#ifndef DECOMP_H
#define DECOMP_H

#include "rt.h"
#include "funcs.h"

/* the i960's global registers: g15 is the frame pointer */
#define G(n) s->r[16 + (n)]

/* The geometry processor (TGP), as the machine code reaches it: g11 holds
   the command page (0x880000) and g12 the step to the parameter page.
   A command is sent by storing anything to command * 16 of g11; its
   parameters and results go through the parameter page (notes/geometry.md). */
#define TGP_CMD_AS(c, v) wr32(s, G(11) + (c) * 16, (v))
#define TGP_CMD(c)    TGP_CMD_AS(c, G(14))
#define TGP_PARAM(v)  wr32(s, G(11) + G(12), (v))
#define TGP_RESULT()  rd32(s, G(11) + G(12))

/* The display list, which the copro board takes through g10: a word
   written at command * 16 carries a float whose exponent comes from the
   address, and the parameter page (g12) takes plain words
   (port/tgp/tgp.c, tgp_m2_ctrl_write32). */
#define DL_EXP(e, v)  wr32(s, G(10) + (e) * 16, (v))
#define DL_WORD(v)    wr32(s, G(10) + G(12), (v))

/* the length of the instruction at a (4 or 8 bytes) */
#define INSN_LEN(a) ((rt_insn_word(a) & 0x8000) ? 8u : 4u)

#define ENTRY(a) uint32_t pc_ = (a)

#define UPTO(b) do { \
        if (rt_pay(s, pc_, (b), 0)) \
            return rt_resume(s, ret_ip, ret_fp); \
        pc_ = (b); \
    } while (0)

#define BRANCH(b, target) do { \
        if (rt_pay(s, pc_, (b), (target))) \
            return rt_resume(s, ret_ip, ret_fp); \
        pc_ = (target); \
    } while (0)

#define RET() return rt_return_at(s, pc_)

#define RET_BX(target) return rt_return_bx(s, pc_, (target), ret_ip, ret_fp)

#define TAIL(fn) return RT_TAIL(fn)

/* a call to another guest function: the frame the call instruction sets
   up, its cycles (after the transfer, as the interpreter charges them), and
   the check that the callee came back where it should */
#define CALL(fn, entry) do { \
        uint32_t ret_ = pc_ + INSN_LEN(pc_), fp_ = s->r[31] & ~0x3fu; \
        s->ip = ret_; \
        i960_call(s, (entry)); \
        if (rt_charge_call(s, rt_insn_word(pc_) & 0x7fff, pc_, (entry))) \
            return rt_resume(s, ret_ip, ret_fp); \
        rt_call(fn, s, ret_, fp_); \
        if (s->ip != ret_ || s->r[31] != fp_) \
            return rt_resume(s, ret_ip, ret_fp); \
        pc_ = ret_; \
    } while (0)

/* ... to a bal procedure, which runs in the caller's frame and returns to
   the address left in g14 */
#define BAL(fn, entry) do { \
        uint32_t ret_ = pc_ + INSN_LEN(pc_), fp_ = s->r[31]; \
        G(14) = ret_; \
        s->ip = (entry); \
        if (rt_charge_call(s, rt_insn_word(pc_) & 0x7fff, pc_, (entry))) \
            return rt_resume(s, ret_ip, ret_fp); \
        rt_call(fn, s, ret_, fp_); \
        if (s->ip != ret_ || s->r[31] != fp_) \
            return rt_resume(s, ret_ip, ret_fp); \
        pc_ = ret_; \
    } while (0)

/* falling (no instruction) into another function's entry, which comes back
   in this frame at `retaddr` (a bal procedure given g14 = retaddr): the
   cursor goes on from there */
#define INTO(fn, retaddr) do { \
        uint32_t fp_ = s->r[31]; \
        rt_call(fn, s, (retaddr), fp_); \
        if (s->ip != (retaddr) || s->r[31] != fp_) \
            return rt_resume(s, ret_ip, ret_fp); \
        pc_ = (retaddr); \
    } while (0)

/* ... and through a pointer (callx), where the target is only known when
   it runs */
#define CALLX(target) do { \
        uint32_t t_ = (target), ret_ = pc_ + INSN_LEN(pc_), fp_ = s->r[31] & ~0x3fu; \
        s->ip = ret_; \
        i960_call(s, t_); \
        if (rt_charge_call(s, rt_insn_word(pc_) & 0x7fff, pc_, t_)) \
            return rt_resume(s, ret_ip, ret_fp); \
        rt_run(s, t_, ret_, fp_); \
        if (s->ip != ret_ || s->r[31] != fp_) \
            return rt_resume(s, ret_ip, ret_fp); \
        pc_ = ret_; \
    } while (0)

#endif
