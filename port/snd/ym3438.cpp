// YM3438 through ymfm; see ym3438.h.
#include "ym3438.h"
#include "../ymfm/ymfm_opn.h"

#include <new>

namespace {

class host : public ymfm::ymfm_interface
{
public:
    int32_t timer[2] = { -1, -1 };   // clocks left, -1 = stopped

    void ymfm_set_timer(uint32_t tnum, int32_t duration_in_clocks) override
    {
        if (tnum < 2)
            timer[tnum] = duration_in_clocks;
    }

    // advance by `clocks` input clocks, firing expired timers
    void tick(int32_t clocks)
    {
        for (int t = 0; t < 2; t++) {
            if (timer[t] < 0)
                continue;
            timer[t] -= clocks;
            if (timer[t] <= 0) {
                timer[t] = -1;
                m_engine->engine_timer_expired((uint32_t)t);   // may restart it
            }
        }
    }
};

} // namespace

struct ym3438 {
    host intf;
    ymfm::ym3438 chip;
    uint32_t clock, native_rate, out_rate;
    double pos;           // native samples still to consume before the next output
    int32_t prev[2], cur[2];

    ym3438(uint32_t c, uint32_t r) : chip(intf), clock(c), out_rate(r)
    {
        native_rate = chip.sample_rate(c);
        reset();
    }

    void reset()
    {
        chip.reset();
        intf.timer[0] = intf.timer[1] = -1;
        pos = 0;
        prev[0] = prev[1] = cur[0] = cur[1] = 0;
    }

    void step_native()
    {
        ymfm::ym3438::output_data o;
        chip.generate(&o);
        prev[0] = cur[0];
        prev[1] = cur[1];
        cur[0] = o.data[0];
        cur[1] = o.data[1];
        intf.tick((int32_t)(clock / native_rate));
    }
};

extern "C" {

ym3438 *ym3438_create(uint32_t clock, uint32_t out_rate)
{
    return new (std::nothrow) ym3438(clock, out_rate);
}

void ym3438_destroy(ym3438 *y) { delete y; }
void ym3438_reset(ym3438 *y) { y->reset(); }
void ym3438_write(ym3438 *y, int offset, uint8_t data) { y->chip.write((uint32_t)offset & 3, data); }
uint8_t ym3438_read(ym3438 *y, int offset) { return y->chip.read((uint32_t)offset & 3); }

void ym3438_render(ym3438 *y, int32_t *out, int n, float gain)
{
    const double step = (double)y->native_rate / y->out_rate;
    for (int i = 0; i < n; i++) {
        y->pos += step;
        while (y->pos >= 1.0) {
            y->step_native();
            y->pos -= 1.0;
        }
        // linear interpolation between the last two native samples
        double f = y->pos;
        for (int c = 0; c < 2; c++) {
            double v = y->prev[c] + (y->cur[c] - y->prev[c]) * f;
            out[i * 2 + c] += (int32_t)(v * gain);
        }
    }
}

} // extern "C"
