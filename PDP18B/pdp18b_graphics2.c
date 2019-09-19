/* pdp18b_graphics2.c: PDP-7 GRAPHCIS-2 display

   Copyright (c) 2019, Lars Brinkhoff

   Permission is hereby granted, free of charge, to any person obtaining a
   copy of this software and associated documentation files (the "Software"),
   to deal in the Software without restriction, including without limitation
   the rights to use, copy, modify, merge, publish, distribute, sublicense,
   and/or sell copies of the Software, and to permit persons to whom the
   Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
   LARS BRINKHOFF BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
   IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
   CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

   Except as contained in this notice, the name of Lars Brinkhoff shall not be
   used in advertising or otherwise to promote the sale, use or other dealings
   in this Software without prior written authorization from Lars Brinkhoff.

   graphics2	(PDP-7, PDP-9) GRAPHICS-2 display.
*/


#include "pdp18b_defs.h"

#if defined(GRAPHICS2)
#include "display/graphics2.h"
#include "display/display.h"

#define DBG_IOT         001         /* IOT instructions. */
#define DBG_IRQ         002         /* Interrupts. */
#define DBG_INS         004         /* 340 instructions. */

/*
 * Number of microseconds between svc calls.  Used to age display and
 * poll for WS events.
 */
#define DPY_CYCLE_US    100

int32 graphics205 (int32 dev, int32 pulse, int32 dat);
int32 graphics206 (int32 dev, int32 pulse, int32 dat);
int32 graphics207 (int32 dev, int32 pulse, int32 dat);
int32 graphics210 (int32 dev, int32 pulse, int32 dat);
int32 graphics2_iors (void);
t_stat graphics2_svc (UNIT *uptr);
t_stat graphics2_reset (DEVICE *dptr);

DIB graphics2_dib = { DEV_G2D1, 4, &graphics2_iors, { &graphics205, &graphics206, &graphics207, &graphics210 } };

UNIT graphics2_unit[] = {
    { UDATA (&graphics2_svc, 0, 0) },
};

DEBTAB graphics2_deb[] = {
    { "IOT", DBG_IOT },
    { "IRQ", DBG_IRQ },
    { "INS", DBG_INS },
    { NULL, 0 }
    };

DEVICE graphics2_dev = {
    "GRAPHICS2", graphics2_unit, NULL, NULL,
    1, 8, 24, 1, 8, 18,
    NULL, NULL, &graphics2_reset,
    NULL, NULL, NULL,
    &graphics2_dib, DEV_DISABLE | DEV_DIS | DEV_DEBUG, 0,
    graphics2_deb, NULL, NULL
    };

t_stat graphics2_svc (UNIT *uptr)
{
  extern int32 *M;

  sim_activate_after(uptr, DPY_CYCLE_US);
  display_age(DPY_CYCLE_US, 0);
  g2_cycle();
  return SCPE_OK;
}

g2word g2_fetch(g2word addr)
{
    extern int32 *M;
    return (g2word)M[addr];
}

t_stat graphics2_reset (DEVICE *dptr)
{
  if (!(dptr->flags & DEV_DIS)) {
    display_reset();
    gr2_reset(dptr);
  }
  sim_cancel (&graphics2_unit[0]);
  return SCPE_OK;
}

void
g2_lp_int(g2word x, g2word y)
{
}

void
g2_rfd(void)
{
}

int32 graphics2_iors (void)
{
#if defined IOS_LPEN
  return IOS_LPEN;
#else
  return 0;
#endif
}

int32 graphics205 (int32 dev, int32 pulse, int32 dat)
{
  sim_debug(DBG_IOT, &graphics2_dev, "7005%02o, %06o\n", pulse, dat);

  if (pulse & 001) {
    if (g2_sense(ST340_VEDGE))
      dat |= IOT_SKP;
  }

  if (pulse & 002) {
    dat |= g2_get_dac();
  }

  if (pulse & 004) {
    g2_clear (ST340_LPHIT);
    sim_activate_abs (graphics2_unit, 0);
  }

  return dat;
}

int32 graphics206 (int32 dev, int32 pulse, int32 dat)
{
  sim_debug(DBG_IOT, &graphics2_dev, "7006%02o, %06o\n", pulse, dat);

  if (pulse & 001) {
    if (g2_sense(ST340_STOPPED))
      dat |= IOT_SKP;
  }

  if (pulse & 002) {
    g2_set_dac (0);
  }

  if (pulse & 004) {
    if ((pulse & 010) == 0)
      g2_set_dac (dat & 07777);
    g2_clear (ST340_STOPPED|ST340_STOP_INT);
    sim_activate_abs (graphics2_unit, 0);
  }

  return dat;
}

int32 graphics207 (int32 dev, int32 pulse, int32 dat)
{
  sim_debug(DBG_IOT, &graphics2_dev, "7007%02o, %06o\n", pulse, dat);

  if (pulse & 001) {
    if (g2_sense(ST340_LPHIT))
      dat |= IOT_SKP;
  }

  if (pulse & 002) {
    dat |= 0; // X, Y
  }

  if (pulse & 004) {
    g2_clear(~0);
  }

  return dat;
}

int32 graphics210 (int32 dev, int32 pulse, int32 dat)
{
  sim_debug(DBG_IOT, &graphics2_dev, "7010%02o, %06o\n", pulse, dat);

  if (pulse & 001) {
    if (g2_sense(ST340_HEDGE))
      dat |= IOT_SKP;
  }

  if (pulse & 002) {
    dat |= g2_get_asr();
  }

  if (pulse & 004) {
    dat |= 0; // Light pen.
  }

  return dat;
}

#endif /* GRAPHICS2 */
