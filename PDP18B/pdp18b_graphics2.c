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

static int32 iot05 (int32 dev, int32 pulse, int32 dat);
static int32 iot06 (int32 dev, int32 pulse, int32 dat);
static int32 iot07 (int32 dev, int32 pulse, int32 dat);
static int32 iot10 (int32 dev, int32 pulse, int32 dat);
static int32 iot14 (int32 dev, int32 pulse, int32 dat);
static int32 iot34 (int32 dev, int32 pulse, int32 dat);
static int32 iot42 (int32 dev, int32 pulse, int32 dat);
static int32 iot43 (int32 dev, int32 pulse, int32 dat);
static int32 iot44 (int32 dev, int32 pulse, int32 dat);
static int32 iot45 (int32 dev, int32 pulse, int32 dat);
int32 graphics2_iors (void);
t_stat graphics2_svc (UNIT *uptr);
t_stat graphics2_reset (DEVICE *dptr);

DIB graphics2_dib1 = { DEV_G2D1, 4, &graphics2_iors,
		       { &iot05, &iot06, &iot07, &iot10 } };
DIB graphics2_dib2 = { DEV_G2D3, 1, &graphics2_iors, { &iot14 } };
DIB graphics2_dib3 = { DEV_G2D4, 1, &graphics2_iors, { &iot34 } };
DIB graphics2_dib4 = { DEV_G2UNK, 4, &graphics2_iors,
		       { &iot42, &iot43,  &iot44,  &iot45 } };

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
    &graphics2_dib1, DEV_DISABLE | DEV_DIS | DEV_DEBUG, 0,
    graphics2_deb, NULL, NULL
    };

DEVICE graphics2b_dev = {
    "GRAPHICS2B", NULL, NULL, NULL,
    0, 8, 24, 1, 8, 18,
    NULL, NULL, &graphics2_reset,
    NULL, NULL, NULL,
    &graphics2_dib2, DEV_DISABLE | DEV_DIS | DEV_DEBUG, 0,
    graphics2_deb, NULL, NULL
    };

DEVICE graphics2c_dev = {
    "GRAPHICS2C", NULL, NULL, NULL,
    0, 8, 24, 1, 8, 18,
    NULL, NULL, &graphics2_reset,
    NULL, NULL, NULL,
    &graphics2_dib3, DEV_DISABLE | DEV_DIS | DEV_DEBUG, 0,
    graphics2_deb, NULL, NULL
    };

DEVICE graphics2d_dev = {
    "GRAPHICS2D", NULL, NULL, NULL,
    0, 8, 24, 1, 8, 18,
    NULL, NULL, &graphics2_reset,
    NULL, NULL, NULL,
    &graphics2_dib4, DEV_DISABLE | DEV_DIS | DEV_DEBUG, 0,
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

/* Used by Unix:

cdf, lds, raef, rlpd, beg, lda,
spb, cpb, lpb, wbl
sck, cck, lck
*/

static int32 iot05 (int32 dev, int32 pulse, int32 dat)
{
  sim_debug(DBG_IOT, &graphics2_dev, "7005%02o, %06o\n", pulse, dat);

  if (pulse & 001) {
    /* CDF, clear display flags */
    g2_clear_flags (G2_DISPLAY_FLAGS);
  }

  if (pulse & 002) {
    /* WDA, write display address */
    g2_set_address (dat);
  }

  if (pulse & 004) {
    /* ECR, enable continuous run */
    g2_set_flags (G2_RUN);
    sim_activate_abs (graphics2_unit, 0);
  }

  if (pulse & 020) {
    /* 24 ESS, enable single step */
    g2_set_flags (G2_STEP);
  }

  if (pulse & 040) {
    /* data request turned on */
    /* 45 C0N, continue */
    /* 47 BEG, begin */
    g2_set_flags (G2_DATA);
  }

  return dat;
}

static int32 iot06 (int32 dev, int32 pulse, int32 dat)
{
  sim_debug(DBG_IOT, &graphics2_dev, "7006%02o, %06o\n", pulse, dat);

  /* 05 WDBC */
  /* 12 LDB */
  /* 25 WDBS */

  return dat;
}

static int32 iot07 (int32 dev, int32 pulse, int32 dat)
{
  sim_debug(DBG_IOT, &graphics2_dev, "7007%02o, %06o\n", pulse, dat);

  /* 01 ELP */
  /* 21 DLP */
  /* 22 RLPE */
  /* 23 RLPD, return after light pen */
  /* 42 RAEF, return after edge flag */

  return dat;
}

static int32 iot10 (int32 dev, int32 pulse, int32 dat)
{
  sim_debug(DBG_IOT, &graphics2_dev, "7010%02o, %06o\n", pulse, dat);

  /* 01 ECS, enable conditional stop */
  /* 12 LDA, load display address */
  /* 21 DCS, disable conditional stop */
  /* 32 LPM, load parameter mode command */
  /* 52 LDS, load display status */

  switch (pulse) {
  case 002:
    dat = g2_get_address ();
    break;
  case 042:
    dat = g2_get_flags ();
    break;
  }

  return dat;
}

static int32 iot14 (int32 dev, int32 pulse, int32 dat)
{
  sim_debug(DBG_IOT, &graphics2_dev, "7014%02o, %06o\n", pulse, dat);

  /* 01 EIS, enable immediate stop */
  /* 12 LX, load x. */

  return dat;
}

static int32 iot34 (int32 dev, int32 pulse, int32 dat)
{
  sim_debug(DBG_IOT, &graphics2_dev, "7034%02o, %06o\n", pulse, dat);

  /* 01 E0V, enable override */
  /* 12 LY, load y. */
  /* 21 D0V, disable override. */

  return dat;
}

static int32 iot42 (int32 dev, int32 pulse, int32 dat)
{
  sim_debug(DBG_IOT, &graphics2_dev, "7042%02o, %06o\n", pulse, dat);

  /* 06 WCGA, unknown.  Unix passes 3072 in AC. */
}

static int32 iot43 (int32 dev, int32 pulse, int32 dat)
{
  sim_debug(DBG_IOT, &graphics2_dev, "7043%02o, %06o\n", pulse, dat);

  /* 01 SCK, skip on console keyboard */
  /* 02 0CK, or console keyboard */
  /* 04 CCK, clear console keboard. */
  /* 12 LCK, load console keboard. */

  if (pulse & 1) {
    if (g2_sense (G2_KEY))
      dat |= IOT_SKP;
  }

  if (pulse & 2) {
      dat |= g2_key ();
  }

  if (pulse & 4) {
      g2_clear_flags (G2_KEY);
  }

  return dat;
}

static int32 iot44 (int32 dev, int32 pulse, int32 dat)
{
  sim_debug(DBG_IOT, &graphics2_dev, "7044%02o, %06o\n", pulse, dat);

  /* 01 SPB, skip push button */
  /* 02 0PB, or push button */
  /* 04 CPB, clear push button. */
  /* 12 LPB, load push button. */
  /* 24 WBL, write button lights. */
  /* 32 LBL, load button lights. */

  if (pulse & 1) {
    if (g2_sense (G2_BUT)) {
      dat |= IOT_SKP;
    }
  }

  if (pulse & 2) {
    if (pulse & 020)
      dat |= g2_get_lights ();
    else      
      dat |= g2_buttons ();
  }

  if (pulse & 4) {
    if (pulse & 020)
      g2_set_lights (dat);
    else
      g2_clear_flags (G2_BUT);
  }

  return dat;
}

static int32 iot45 (int32 dev, int32 pulse, int32 dat)
{
  sim_debug(DBG_IOT, &graphics2_dev, "7045%02o, %06o\n", pulse, dat);

  /* 01 EIM, enable interrupt mask */
  /* 12 LIM, load interrupt mask */
  /* 21 DIM, disable interrupt mask, */

  return dat;
}

#endif /* GRAPHICS2 */
