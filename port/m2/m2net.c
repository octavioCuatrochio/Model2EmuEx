/* Model 2 network ring between boards in one process; see m2net.h and
   QUIRKS.md ("Network"). */
#include "m2net.h"

#include <stdlib.h>
#include <string.h>

#define BLOCK      0xe00     /* each node's data: net RAM +0x2000 */
#define BLOCK_AT   0x2000
#define SLOT_MODE1 0x1c0     /* orig 0x574dd8 with mode 0x574ddc = 1 (its initial value) */

/* The network board's shared RAM as the i960 sees it at 0x1a10000 (orig
   0x5aa750 = M2ZSHARED + 0x8000); registers at +0x4000 are net_reg0/2. */
static uint8_t *net_ram(m2_board *b) { return b->zshared + 0x8000; }
static void st16(uint8_t *p, uint16_t v) { memcpy(p, &v, 2); }

typedef struct {
    m2_board *b;
    int       state;         /* orig 0x10ed8bc: 0 searching, 1 link-up, 3 running */
    int       master;        /* orig 0x10ed8b9 */
    uint32_t  slot;          /* orig 0x574dd8: where the received block goes */
} node;

struct m2_net {
    int     n;
    node    node[M2NET_MAX];
    uint8_t sent[M2NET_MAX][BLOCK];
};

m2_net *m2net_create(m2_board *const *boards, int n)
{
    if (n < 1 || n > M2NET_MAX)
        return NULL;
    m2_net *r = calloc(1, sizeof *r);
    if (!r)
        return NULL;
    r->n = n;
    for (int i = 0; i < n; i++) {
        r->node[i].b = boards[i];
        boards[i]->net_enabled = 1;   /* orig 0x10ed8b8: networking started */
    }
    return r;
}

void m2net_destroy(m2_net *r) { free(r); }

void m2net_reset(m2_net *r, int i)
{
    if (i >= 0 && i < r->n)
        r->node[i].state = 0;
}

int m2net_state(const m2_net *r, int i) { return i >= 0 && i < r->n ? r->node[i].state : 0; }

void m2net_frame(m2_net *r)
{
    /* 1. search (thread 0x4cf740, state 0). The thread polls the socket
       without waiting, so its 60 attempts pass as soon as the game turns
       the board on (register 0 bit 0); the last attempt's values stay. */
    for (int i = 0; i < r->n; i++) {
        node *d = &r->node[i];
        m2_board *b = d->b;
        uint8_t *m = net_ram(b);
        if (!(b->net_reg0 & 1)) {
            m[0] = 0;
            d->state = 0;
            continue;
        }
        if (d->state == 0) {
            b->net_reg2 |= 0x80;
            m[0] = 0;
            st16(m + 2, 0xffff);
            st16(m + 0x12, BLOCK);
            d->state = 1;
            d->master = b->net_reg2 & 1;   /* written by the game: link mode MASTER */
            d->slot = SLOT_MODE1;
            st16(m + 0x14, SLOT_MODE1);
            memset(m + BLOCK_AT, 0, BLOCK);
        }
    }

    /* 2. link-up (receive 0x4cf3d0, packet types 0 and 1). The master sends
       a hello round the ring every 100 ms; each slave adds itself, and when
       the hello is back the master sends the node count and numbers round.
       A node that isn't listening yet drops the packet, so this completes
       once every node is in state 1 and exactly one of them is the master.
       Numbers: the master is 1; the k-th node after it gets byte 6 = k + 1
       and stores n - (k + 1) + 2 as its number (as the original). */
    int master = -1, masters = 0, listening = 1;
    for (int i = 0; i < r->n; i++) {
        if (r->node[i].state != 1 && r->node[i].state != 3)
            listening = 0;
        if (r->node[i].state == 1 && r->node[i].master) {
            master = i;
            masters++;
        }
    }
    int linking = 0;
    for (int i = 0; i < r->n; i++)
        linking |= r->node[i].state == 1;
    if (linking && listening && masters == 1) {
        for (int k = 0; k < r->n; k++) {
            node *d = &r->node[(master + k) % r->n];
            uint8_t *m = net_ram(d->b);
            if (d->state != 1)
                continue;
            m[3] = (uint8_t)r->n;
            m[2] = (uint8_t)(k == 0 ? 1 : r->n - (k + 1) + 2);
            d->b->net_reg2 = (uint8_t)((d->b->net_reg2 & 0x7f) ^ 0x80);
            m[0] = 1;
            d->state = 3;
        }
    }

    /* 3. running (frame end of 0x4cc7a0 and type-2 packets): each node's
       block goes to the next node in the ring, which stores it at its slot
       and toggles the heartbeat. Taken from every node before any is
       written, as all were sent at the end of the same frame. */
    for (int i = 0; i < r->n; i++)
        if (r->node[i].state == 3)
            memcpy(r->sent[i], net_ram(r->node[i].b) + BLOCK_AT, BLOCK);
    for (int i = 0; i < r->n; i++) {
        node *d = &r->node[i];
        int prev = (i + r->n - 1) % r->n;
        if (d->state != 3 || r->node[prev].state != 3 || r->n < 2)
            continue;
        memcpy(net_ram(d->b) + BLOCK_AT + d->slot, r->sent[prev], BLOCK);
        d->b->net_reg2 ^= 0x80;
    }
}
