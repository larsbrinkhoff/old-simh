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

   graphic2	(PDP-7, PDP-9) Graphic-II display.
*/


#include "pdp18b_defs.h"

#if defined(GRAPHIC2)
#include "display/graphics2.h"
#include "display/display.h"
#include "sim_video.h"

extern int32 int_hwre[API_HLVL+1];

#define DBG_IOT         001         /* IOT instructions. */
#define DBG_IRQ         002         /* Interrupts. */
#define DBG_INS         004         /* 340 instructions. */

#define BUTTON1  0400000
#define BUTTON2  0200000
#define BUTTON3  0100000
#define BUTTON4  0040000
#define BUTTON5  0020000
#define BUTTON6  0010000
#define BUTTON7  0004000
#define BUTTON8  0002000

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
static t_stat graphics2_svc (UNIT *uptr);
static t_stat graphics2_reset (DEVICE *dptr);

DIB graphics2_dib1 = { DEV_G2D1, 4, NULL,
		       { &iot05, &iot06, &iot07, &iot10 } };
DIB graphics2_dib2 = { DEV_G2D3, 1, NULL, { &iot14 } };
DIB graphics2_dib3 = { DEV_G2D4, 1, NULL, { &iot34 } };
DIB graphics2_dib4 = { DEV_G2UNK, 4, NULL,
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
    "GRAPHIC2", graphics2_unit, NULL, NULL,
    1, 8, 24, 1, 8, 18,
    NULL, NULL, &graphics2_reset,
    NULL, NULL, NULL,
    &graphics2_dib1, DEV_DISABLE | DEV_DIS | DEV_DEBUG, 0,
    graphics2_deb, NULL, NULL
    };

DEVICE graphics2b_dev = {
    "GRAPHIC2B", NULL, NULL, NULL,
    0, 8, 24, 1, 8, 18,
    NULL, NULL, &graphics2_reset,
    NULL, NULL, NULL,
    &graphics2_dib2, DEV_DISABLE | DEV_DIS | DEV_DEBUG, 0,
    graphics2_deb, NULL, NULL
    };

DEVICE graphics2c_dev = {
    "GRAPHIC2C", NULL, NULL, NULL,
    0, 8, 24, 1, 8, 18,
    NULL, NULL, &graphics2_reset,
    NULL, NULL, NULL,
    &graphics2_dib3, DEV_DISABLE | DEV_DIS | DEV_DEBUG, 0,
    graphics2_deb, NULL, NULL
    };

DEVICE graphics2d_dev = {
    "GRAPHIC2D", NULL, NULL, NULL,
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
  g2_cycle(DPY_CYCLE_US);

  return SCPE_OK;
}

g2word g2_fetch(g2word addr)
{
    extern int32 *M;
    return (g2word)M[addr];
}

static int key_shift = 0;
static int key_control = 0;
static int key_code = 0;
static int buttons = 0;

static int g2_modifiers (SIM_KEY_EVENT *kev)
{
  if (kev->state == SIM_KEYPRESS_DOWN) {
    switch (kev->key) {
    case SIM_KEY_SHIFT_L:
    case SIM_KEY_SHIFT_R:
      key_shift = 1;
      return 1;
    case SIM_KEY_CTRL_L:
    case SIM_KEY_CTRL_R:
    case SIM_KEY_CAPS_LOCK:
      key_control = 1;
      return 1;
    case SIM_KEY_WIN_L:
    case SIM_KEY_WIN_R:
    case SIM_KEY_ALT_L:
    case SIM_KEY_ALT_R:
      return 1;
    }
  } else if (kev->state == SIM_KEYPRESS_UP) {
    switch (kev->key) {
    case SIM_KEY_SHIFT_L:
    case SIM_KEY_SHIFT_R:
      key_shift = 0;
      return 1;
    case SIM_KEY_CTRL_L:
    case SIM_KEY_CTRL_R:
    case SIM_KEY_CAPS_LOCK:
      key_control = 0;
      return 1;
    case SIM_KEY_WIN_L:
    case SIM_KEY_WIN_R:
    case SIM_KEY_ALT_L:
    case SIM_KEY_ALT_R:
      return 1;
    }
  }
  return 0;
}

static int g2_keys (SIM_KEY_EVENT *kev)
{
  if (kev->state == SIM_KEYPRESS_UP)
    return 0;

  switch (kev->key) {
  case SIM_KEY_0:
    key_code = key_shift ? ')' : '0';
    return 1;
  case SIM_KEY_1:
    key_code = key_shift ? '!' : '1';
    return 1;
  case SIM_KEY_2:
    key_code = key_shift ? '@' : '2';
    return 1;
  case SIM_KEY_3:
    key_code = key_shift ? '#' : '3';
    return 1;
  case SIM_KEY_4:
    key_code = key_shift ? '$' : '4';
    return 1;
  case SIM_KEY_5:
    key_code = key_shift ? '%' : '5';
    return 1;
  case SIM_KEY_6:
    key_code = key_shift ? '^' : '6';
    return 1;
  case SIM_KEY_7:
    key_code = key_shift ? '&' : '7';
    return 1;
  case SIM_KEY_8:
    key_code = key_shift ? '*' : '8';
    return 1;
  case SIM_KEY_9:
    key_code = key_shift ? '(' : '9';
    return 1;
  case SIM_KEY_A:
    key_code = key_shift ? 'A' : 'a';
    return 1;
  case SIM_KEY_B:
    key_code = key_shift ? 'B' : 'b';
    return 1;
  case SIM_KEY_C:
    key_code = key_shift ? 'C' : 'c';
    return 1;
  case SIM_KEY_D:
    key_code = key_shift ? 'D' : 'd';
    return 1;
  case SIM_KEY_E:
    key_code = key_shift ? 'E' : 'e';
    return 1;
  case SIM_KEY_F:
    key_code = key_shift ? 'F' : 'f';
    return 1;
  case SIM_KEY_G:
    key_code = key_shift ? 'G' : 'g';
    return 1;
  case SIM_KEY_H:
    key_code = key_shift ? 'H' : 'h';
    return 1;
  case SIM_KEY_I:
    key_code = key_shift ? 'I' : 'i';
    return 1;
  case SIM_KEY_J:
    key_code = key_shift ? 'J' : 'j';
    return 1;
  case SIM_KEY_K:
    key_code = key_shift ? 'K' : 'k';
    return 1;
  case SIM_KEY_L:
    key_code = key_shift ? 'L' : 'l';
    return 1;
  case SIM_KEY_M:
    key_code = key_shift ? 'M' : 'm';
    return 1;
  case SIM_KEY_N:
    key_code = key_shift ? 'N' : 'n';
    return 1;
  case SIM_KEY_O:
    key_code = key_shift ? 'O' : 'o';
    return 1;
  case SIM_KEY_P:
    key_code = key_shift ? 'P' : 'p';
    return 1;
  case SIM_KEY_Q:
    key_code = key_shift ? 'Q' : 'q';
    return 1;
  case SIM_KEY_R:
    key_code = key_shift ? 'R' : 'r';
    return 1;
  case SIM_KEY_S:
    key_code = key_shift ? 'S' : 's';
    return 1;
  case SIM_KEY_T:
    key_code = key_shift ? 'T' : 't';
    return 1;
  case SIM_KEY_U:
    key_code = key_shift ? 'U' : 'u';
    return 1;
  case SIM_KEY_V:
    key_code = key_shift ? 'V' : 'v';
    return 1;
  case SIM_KEY_W:
    key_code = key_shift ? 'W' : 'w';
    return 1;
  case SIM_KEY_X:
    key_code = key_shift ? 'X' : 'x';
    return 1;
  case SIM_KEY_Y:
    key_code = key_shift ? 'Y' : 'y';
    return 1;
  case SIM_KEY_Z:
    key_code = key_shift ? 'Z' : 'z';
    return 1;
  case SIM_KEY_BACKQUOTE:
    key_code = key_shift ? '~' : '`';
    return 1;
  case SIM_KEY_MINUS:
    key_code = key_shift ? '_' : '-';
    return 1;
  case SIM_KEY_EQUALS:
    key_code = key_shift ? '+': '=';
    return 1;
  case SIM_KEY_LEFT_BRACKET:
    key_code = key_shift ? '{' : '[';
    return 1;
  case SIM_KEY_RIGHT_BRACKET:
    key_code = key_shift ? '}' : ']';
    return 1;
  case SIM_KEY_SEMICOLON:
    key_code = key_shift ? ':' : ';';
    return 1;
  case SIM_KEY_SINGLE_QUOTE:
    key_code = key_shift ? '"' : '\'';
    return 1;
  case SIM_KEY_BACKSLASH:
  case SIM_KEY_LEFT_BACKSLASH:
    key_code = key_shift ? '|' : '\\';
    return 1;
  case SIM_KEY_COMMA:
    key_code = key_shift ? '<' : ',';
    return 1;
  case SIM_KEY_PERIOD:
    key_code = key_shift ? '>' : '.';
    return 1;
  case SIM_KEY_SLASH:
    key_code = key_shift ? '?' : '/';
    return 1;
  case SIM_KEY_ESC:
    key_code = 033;
    return 1;
  case SIM_KEY_BACKSPACE:
  case SIM_KEY_DELETE:
    key_code = 010;
    return 1;
  case SIM_KEY_TAB:
    key_code = 011;
    return 1;
  case SIM_KEY_ENTER:
    key_code = 012;
    return 1;
  case SIM_KEY_SPACE:
    key_code = ' ';
    return 1;
  default:
    return 0;
  }
}

static int g2_buttons (SIM_KEY_EVENT *kev)
{
  int mask;

  switch (kev->key) {
  case SIM_KEY_F1:
    mask = BUTTON1;
    break;
  case SIM_KEY_F2:
    mask = BUTTON2;
    break;
  case SIM_KEY_F3:
    mask = BUTTON3;
    break;
  case SIM_KEY_F4:
    mask = BUTTON4;
    break;
  case SIM_KEY_F5:
    mask = BUTTON5;
    break;
  case SIM_KEY_F6:
    mask = BUTTON6;
    break;
  case SIM_KEY_F7:
    mask = BUTTON7;
    break;
  case SIM_KEY_F8:
    mask = BUTTON8;
    break;
  default:
    return 0;
  }

  if (kev->state == SIM_KEYPRESS_DOWN)
    buttons |= mask;
  else if (kev->state == SIM_KEYPRESS_UP)
    buttons &= ~mask;
  else
    return 0;

  return 1;
}

static int g2_keyboard (SIM_KEY_EVENT *kev)
{
    if (g2_modifiers (kev))
      return 0;
    
    if (g2_keys (kev)) {
      if (key_control && key_code >= '@')
	key_code &= 037;
      g2_set_flags (G2_KEY);
      SET_INT (G2);
      return 0;
    }

    if (g2_buttons (kev)) {
      g2_set_flags (G2_BUT);
      SET_INT (G2);
      return 0;
    }

    return 1;
}

t_stat graphics2_reset (DEVICE *dptr)
{
  if (!(dptr->flags & DEV_DIS)) {
    display_reset();
    gr2_reset(dptr);
    vid_display_kb_event_process = g2_keyboard;
  }
  sim_cancel (&graphics2_unit[0]);
  return SCPE_OK;
}

void
g2_lp_int(g2word x, g2word y)
{
  SET_INT (G2);
}

void
g2_rfd(void)
{
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
    g2_clear_flags (G2_STEP);
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
    sim_activate_abs (graphics2_unit, 0);
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

  if (pulse & 1) {
    if (pulse & 2)
      /* 21 DLP */
      g2_set_lp (0);
    else
      /* 01 ELP */
      g2_set_lp (1);
  }

  if (pulse & 2) {
    if (pulse & 040)
      /* 42 RAEF, return after edge flag */
      g2_clear_flags (G2_EDGE|G2_REDGE|G2_LEDGE|G2_TEDGE|G2_BEDGE);
    else {
      /* 22 RLPE, 23 RLPD */
      g2_clear_flags (G2_LP);
      CLR_INT (G2);
    }
  }

  return dat;
}

static int32 iot10 (int32 dev, int32 pulse, int32 dat)
{
  sim_debug(DBG_IOT, &graphics2_dev, "7010%02o, %06o\n", pulse, dat);

  if (pulse & 1) {
    if (pulse & 020)
      /* 21 DCS, disable conditional stop */
      ;
    else
      /* 01 ECS, enable conditional stop */
      ;
  }

  if (pulse & 2) {
    if (pulse & 020)
      /* 32 LPM, load parameter mode command */
      dat |= 0;
    else if (pulse & 040)
      /* 52 LDS, load display status */
      dat |= g2_get_flags ();
    else
      /* 12 LDA, load display address */
      dat |= g2_get_address ();
  }

  return dat;
}

static int32 iot14 (int32 dev, int32 pulse, int32 dat)
{
  sim_debug(DBG_IOT, &graphics2_dev, "7014%02o, %06o\n", pulse, dat);

  if (pulse & 001) {
    /* 01 EIS, enable immediate stop */
    g2_set_flags (G2_IMM);
  }

  if (pulse & 002) {
    /* 12 LX, load x. */
    dat |= 0;
  }

  return dat;
}

static int32 iot34 (int32 dev, int32 pulse, int32 dat)
{
  sim_debug(DBG_IOT, &graphics2_dev, "7034%02o, %06o\n", pulse, dat);

  if (pulse & 001) {
    if (pulse & 002)
      /* 21 D0V, disable override. */
      g2_clear_flags (G2_OVER);
    else
      /* 01 E0V, enable override */
      g2_set_flags (G2_OVER);
  }

  if (pulse & 002) {
    /* 12 LY, load y. */
    dat |= 0;
  }

  return dat;
}

static int32 iot42 (int32 dev, int32 pulse, int32 dat)
{
  sim_debug(DBG_IOT, &graphics2_dev, "7042%02o, %06o\n", pulse, dat);

  /* 06 WCGA, unknown.  Unix passes 3072 in AC. */
  /* Maybe write character generator address. */
  return dat;
}

static int32 iot43 (int32 dev, int32 pulse, int32 dat)
{
  sim_debug(DBG_IOT, &graphics2_dev, "7043%02o, %06o\n", pulse, dat);

  if (pulse & 1) {
    if (g2_sense (G2_KEY))
	dat |= IOT_SKP;
  }

  if (pulse & 2) {
      /* 02 0CK, or console keyboard */
      /* 12 LCK, load console keboard. */
      dat |= key_code;
      key_code = 0;
  }

  if (pulse & 4) {
      /* 04 CCK, clear console keboard. */
      g2_clear_flags (G2_KEY);
      CLR_INT(G2);
  }

  sim_debug(DBG_IOT, &graphics2_dev, "IOT return %06o\n", dat);
  return dat;
}

static int32 iot44 (int32 dev, int32 pulse, int32 dat)
{
  sim_debug(DBG_IOT, &graphics2_dev, "7044%02o, %06o\n", pulse, dat);

  if (pulse & 1) {
    /* 01 SPB, skip push button */
    if (g2_sense (G2_BUT))
      dat |= IOT_SKP;
  }

  if (pulse & 2) {
    if (pulse & 020)
      /* 32 LBL, load button lights. */
      dat |= 0;
    else      
      /* 02 0PB, or push button */
      /* 12 LPB, load push button. */
      dat |= buttons;
  }

  if (pulse & 4) {
    if (pulse & 020)
      /* 24 WBL, write button lights. */
      ;
    else {
      /* 04 CPB, clear push button. */
      g2_clear_flags (G2_BUT);
      CLR_INT (G2);
    }
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

#endif /* GRAPHIC2 */
