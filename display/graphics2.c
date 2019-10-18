/*
 * wrap, or clip?
 * skip vector points when scaling?
 * are scaled character sizes right??
 */

/*
 * $Id: type340.c,v 1.6 2005/01/14 18:58:00 phil Exp $
 * Simulator Independent DEC Type 340 Graphic Display Processor Simulation
 * Phil Budne <phil@ultimate.com>
 * September 20, 2003
 * from vt11.c
 *
 * First displayed PDP-6/10 SPCWAR Feb 2018!
 *
 * The Type 340 was used on the PDP-{4,6,7,9,10}
 * and used 18-bit words, with bits numbered 0 thru 17
 * (most significant to least)
 *
 * This file simulates ONLY the 340 proper
 * and not CPU specific interfacing details
 *
 * see:
 * http://www.bitsavers.org/pdf/dec/graphics/H-340_Type_340_Precision_Incremental_CRT_System_Nov64.pdf
 *
 * Initial information from DECUS 7-13:
 * http://www.bitsavers.org/pdf/dec/graphics/7-13_340_Display_Programming_Manual.pdf
 * pre-bitsavers location(!!):
 * http://www.spies.com/~aek/pdf/dec/pdp7/7-13_340displayProgMan.pdf
 *
 * NOTE!!! The 340 is an async processor, with multiple control signals
 * running in parallel.  No attempt has been made to simulate this.
 * And while it might be fun to try to implement it as a bit vector
 * of signals, and run code triggered by those signals in the next
 * service interval, BUT unless/until this is proven necessary, I'm
 * resisting that impulse (pun not intended).
 */

/*
 * Copyright (c) 2003-2018, Philip L. Budne
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of the author shall
 * not be used in advertising or otherwise to promote the sale, use or
 * other dealings in this Software without prior written authorization
 * from the authors.
 */

#include "display.h"                 /* XY plot interface */
#include "graphics2.h"                 /* interface definitions */

#define BITMASK(N) (1<<(17-(N)))

/* mask for a field */
#define FIELDMASK(START,END) ((1<<((END)-(START)+1))-1)

/* extract a field */
#define GETFIELD(W,START,END) (((W)>>(17-(END)))&FIELDMASK(START,END))

/* extract a 1-bit field */
#define TESTBIT(W,B) (((W) & BITMASK(B)) != 0)

#ifdef DEBUG_G2
#include <stdio.h>
#define DEBUGF(X) printf X
#else
#define DEBUGF(X)
#endif
enum mode { PARAM=0, POINT, SLAVE, CHAR, VECTOR, VCONT, INCR, SUBR };

#define G2_UNITS 1

enum jump_type { DJP=2, DJS=3, DDS=1 }; /* type 347 */

/* Codes for special characters. */
#define CH_LF     0001   /* Line feed. */
#define CH_CR     0002   /* Carriage return. */
#define CH_UC     0003   /* Shift in. */
#define CH_LC     0004   /* Shift out. */
#define CH_ESC    0005   /* Escape to parameter mode. */
#define CH_NSPC   0006   /* Non spacing. */
#define CH_D      0007   /* Descender. */
#define CH_BS     0010   /* Backspace. */
#define CH_SUB    0011   /* Subscript. */
#define CH_SUP    0012   /* Superscript. */

/* put all the state in a struct "just in case" */
static struct graphics2 {
    g2word DAC;              /* Display Address Counter */
    g2word status;           /* see ST340_XXX in type340.h */
    signed short xpos, ypos;    /* 10 bits, signed (for OOB checks) */
    char initialized;           /* 0 before display_init */
    /* only using (evil) bitfield syntax to limit enum size */
    enum mode mode : 8;         /* 3 bits */
    unsigned char lp_ena;       /* 1 bit */
    unsigned char scale;        /* multiplier: 1,2,4,8 */
    unsigned char intensity;    /* 3 bits */
} u340[G2_UNITS];

#if G2_UNITS == 1
#define UNIT(N) u340
#else
#define UNIT(N) (u340+(N))
#endif

void
g2_set_address(g2word addr)
{
    struct graphics2 *u = UNIT(0);
    u->DAC = addr;
    DEBUGF(("set DAC %06o\r\n", u->DAC));

    /* XXX only when reset? */
    u->mode = PARAM;
    u->status = 0;               /* XXX just clear stopped? */
    g2_rfd();                 /* ready for data */
}

void
g2_cycle(void)
{
    struct graphics2 *u = UNIT(0);

    if (u->status & G2_RUN) {
        g2word insn = g2_fetch(u->DAC);
        u->status = g2_instruction (insn);
        u->DAC = (u->DAC + 1) & 07777;
    }
}

g2word
g2_get_address(void)
{
    struct graphics2 *u = UNIT(0);
    return u->DAC;
}

g2word
g2_buttons(void)
{
    struct graphics2 *u = UNIT(0);
    return 0;
}

g2word
g2_key(void)
{
    struct graphics2 *u = UNIT(0);
    return 0;
}

g2word
g2_get_lights(void)
{
    struct graphics2 *u = UNIT(0);
    return 0;
}

void
g2_set_lights(g2word x)
{
    struct graphics2 *u = UNIT(0);
}

g2word
g2_sense(g2word flags)
{
    struct graphics2 *u = UNIT(0);
    return u->status & flags;
}

void
g2_clear_flags(g2word flags)
{
    struct graphics2 *u = UNIT(0);
    u->status &= ~flags;
}

void
g2_set_flags(g2word flags)
{
    struct graphics2 *u = UNIT(0);
    u->status |= flags;
}

g2word
gr2_reset(void *dptr)
{
    struct graphics2 *u = UNIT(0);
#ifndef G2_NODISPLAY
    if (!u->initialized) {
        display_init(DIS_GRAPHICS2, 1, dptr); /* XXX check return? */
        u->initialized = 1;
    }
#endif
    u->xpos = u->ypos = 0;
    u->mode = PARAM;
    u->status = 0;
    u->scale = 1;
    g2_rfd();                /* ready for data */
    return u->status;
}

static int
point(int x, int y, int seq)
{
    struct graphics2 *u = UNIT(0);
    int i;

#ifdef TYPE340_POINT
    DEBUGF(("type340 point %d %d %d\r\n", x, y, seq));
#endif

    i = DISPLAY_INT_MAX-7+u->intensity;
    if (i <= 0)
        i = 1;

    if (x < 0 || x > 1023) {
        /* XXX clip? wrap?? */
        u->status |= G2_EDGE;
        return 0;
    }
    if (y < 0 || y > 1023) {
        /* XXX clip? wrap?? */
        u->status |= G2_EDGE;
        return 0;
    }

#ifndef G2_NODISPLAY
    if (display_point(x, y, i, 0)) {
        /*
         * in real life: type340 pauses
         * until CPU reads coordinates
         */
        u->status |= G2_LP;
        if (u->lp_ena)
            g2_lp_int(x, y);
    }
#endif
    return 1;
}

static void
lpoint(int x, int y)
{
#ifdef TYPE340_LPOINT
    DEBUGF(("type340 lpoint %d %d\r\n", x, y));
#endif
    point(x, y, 0);
}

/*
 * two-step algorithm, developed by Xiaolin Wu
 * from http://graphics.lcs.mit.edu/~mcmillan/comp136/Lecture6/Lines.html
 */

/*
 * The two-step algorithm takes the interesting approach of treating
 * line drawing as a automaton, or finite state machine. If one looks
 * at the possible configurations for the next two pixels of a line,
 * it is easy to see that only a finite set of possibilities exist.
 * The two-step algorithm shown here also exploits the symmetry of
 * line-drawing by simultaneously drawn from both ends towards the
 * midpoint.
 */

static void
lineTwoStep(int x0, int y0, int x1, int y1)
{
    int dy = y1 - y0;
    int dx = x1 - x0;
    int stepx, stepy;

    if (dy < 0) { dy = -dy;  stepy = -1; } else { stepy = 1; }
    if (dx < 0) { dx = -dx;  stepx = -1; } else { stepx = 1; }

    lpoint(x0,y0);
    if (dx == 0 && dy == 0)             /* following algorithm won't work */
        return;                         /* just the one dot */
    lpoint(x1, y1);
    if (dx > dy) {
        int length = (dx - 1) >> 2;
        int extras = (dx - 1) & 3;
        int incr2 = (dy << 2) - (dx << 1);
        if (incr2 < 0) {
            int c = dy << 1;
            int incr1 = c << 1;
            int d =  incr1 - dx;
            int i;

            for (i = 0; i < length; i++) {
                x0 += stepx;
                x1 -= stepx;
                if (d < 0) {                            /* Pattern: */
                    lpoint(x0, y0);
                    lpoint(x0 += stepx, y0);            /*  x o o   */
                    lpoint(x1, y1);
                    lpoint(x1 -= stepx, y1);
                    d += incr1;
                }
                else {
                    if (d < c) {                          /* Pattern: */
                        lpoint(x0, y0);                   /*      o   */
                        lpoint(x0 += stepx, y0 += stepy); /*  x o     */
                        lpoint(x1, y1);
                        lpoint(x1 -= stepx, y1 -= stepy);
                    } else {
                        lpoint(x0, y0 += stepy);        /* Pattern: */
                        lpoint(x0 += stepx, y0);        /*    o o   */
                        lpoint(x1, y1 -= stepy);        /*  x       */
                        lpoint(x1 -= stepx, y1);
                    }
                    d += incr2;
                }
            }
            if (extras > 0) {
                if (d < 0) {
                    lpoint(x0 += stepx, y0);
                    if (extras > 1) lpoint(x0 += stepx, y0);
                    if (extras > 2) lpoint(x1 -= stepx, y1);
                } else
                    if (d < c) {
                        lpoint(x0 += stepx, y0);
                        if (extras > 1) lpoint(x0 += stepx, y0 += stepy);
                        if (extras > 2) lpoint(x1 -= stepx, y1);
                    } else {
                        lpoint(x0 += stepx, y0 += stepy);
                        if (extras > 1) lpoint(x0 += stepx, y0);
                        if (extras > 2) lpoint(x1 -= stepx, y1 -= stepy);
                    }
            }
        } else {
            int c = (dy - dx) << 1;
            int incr1 = c << 1;
            int d =  incr1 + dx;
            int i;
            for (i = 0; i < length; i++) {
                x0 += stepx;
                x1 -= stepx;
                if (d > 0) {
                    lpoint(x0, y0 += stepy);            /* Pattern: */
                    lpoint(x0 += stepx, y0 += stepy);   /*      o   */
                    lpoint(x1, y1 -= stepy);            /*    o     */
                    lpoint(x1 -= stepx, y1 -= stepy);   /*  x       */
                    d += incr1;
                } else {
                    if (d < c) {
                        lpoint(x0, y0);                   /* Pattern: */
                        lpoint(x0 += stepx, y0 += stepy); /*      o   */
                        lpoint(x1, y1);                   /*  x o     */
                        lpoint(x1 -= stepx, y1 -= stepy);
                    } else {
                        lpoint(x0, y0 += stepy);        /* Pattern: */
                        lpoint(x0 += stepx, y0);        /*    o o   */
                        lpoint(x1, y1 -= stepy);        /*  x       */
                        lpoint(x1 -= stepx, y1);
                    }
                    d += incr2;
                }
            }
            if (extras > 0) {
                if (d > 0) {
                    lpoint(x0 += stepx, y0 += stepy);
                    if (extras > 1) lpoint(x0 += stepx, y0 += stepy);
                    if (extras > 2) lpoint(x1 -= stepx, y1 -= stepy);
                } else if (d < c) {
                    lpoint(x0 += stepx, y0);
                    if (extras > 1) lpoint(x0 += stepx, y0 += stepy);
                    if (extras > 2) lpoint(x1 -= stepx, y1);
                } else {
                    lpoint(x0 += stepx, y0 += stepy);
                    if (extras > 1) lpoint(x0 += stepx, y0);
                    if (extras > 2) {
                        if (d > c)
                            lpoint(x1 -= stepx, y1 -= stepy);
                        else
                            lpoint(x1 -= stepx, y1);
                    }
                }
            }
        }
    } else {
        int length = (dy - 1) >> 2;
        int extras = (dy - 1) & 3;
        int incr2 = (dx << 2) - (dy << 1);
        if (incr2 < 0) {
            int c = dx << 1;
            int incr1 = c << 1;
            int d =  incr1 - dy;
            int i;
            for (i = 0; i < length; i++) {
                y0 += stepy;
                y1 -= stepy;
                if (d < 0) {
                    lpoint(x0, y0);
                    lpoint(x0, y0 += stepy);
                    lpoint(x1, y1);
                    lpoint(x1, y1 -= stepy);
                    d += incr1;
                } else {
                    if (d < c) {
                        lpoint(x0, y0);
                        lpoint(x0 += stepx, y0 += stepy);
                        lpoint(x1, y1);
                        lpoint(x1 -= stepx, y1 -= stepy);
                    } else {
                        lpoint(x0 += stepx, y0);
                        lpoint(x0, y0 += stepy);
                        lpoint(x1 -= stepx, y1);
                        lpoint(x1, y1 -= stepy);
                    }
                    d += incr2;
                }
            }
            if (extras > 0) {
                if (d < 0) {
                    lpoint(x0, y0 += stepy);
                    if (extras > 1) lpoint(x0, y0 += stepy);
                    if (extras > 2) lpoint(x1, y1 -= stepy);
                } else
                    if (d < c) {
                        lpoint(x0, y0 += stepy);
                        if (extras > 1) lpoint(x0 += stepx, y0 += stepy);
                        if (extras > 2) lpoint(x1, y1 -= stepy);
                    } else {
                        lpoint(x0 += stepx, y0 += stepy);
                        if (extras > 1) lpoint(x0, y0 += stepy);
                        if (extras > 2) lpoint(x1 -= stepx, y1 -= stepy);
                    }
            }
        } else {
            int c = (dx - dy) << 1;
            int incr1 = c << 1;
            int d =  incr1 + dy;
            int i;
            for (i = 0; i < length; i++) {
                y0 += stepy;
                y1 -= stepy;
                if (d > 0) {
                    lpoint(x0 += stepx, y0);
                    lpoint(x0 += stepx, y0 += stepy);
                    lpoint(x1 -= stepx, y1);
                    lpoint(x1 -= stepx, y1 -= stepy);
                    d += incr1;
                } else {
                    if (d < c) {
                        lpoint(x0, y0);
                        lpoint(x0 += stepx, y0 += stepy);
                        lpoint(x1, y1);
                        lpoint(x1 -= stepx, y1 -= stepy);
                    } else {
                        lpoint(x0 += stepx, y0);
                        lpoint(x0, y0 += stepy);
                        lpoint(x1 -= stepx, y1);
                        lpoint(x1, y1 -= stepy);
                    }
                    d += incr2;
                }
            }
            if (extras > 0) {
                if (d > 0) {
                    lpoint(x0 += stepx, y0 += stepy);
                    if (extras > 1) lpoint(x0 += stepx, y0 += stepy);
                    if (extras > 2) lpoint(x1 -= stepx, y1 -= stepy);
                } else if (d < c) {
                    lpoint(x0, y0 += stepy);
                    if (extras > 1) lpoint(x0 += stepx, y0 += stepy);
                    if (extras > 2) lpoint(x1, y1 -= stepy);
                } else {
                    lpoint(x0 += stepx, y0 += stepy);
                    if (extras > 1) lpoint(x0, y0 += stepy);
                    if (extras > 2) {
                        if (d > c)
                            lpoint(x1 -= stepx, y1 -= stepy);
                        else
                            lpoint(x1, y1 -= stepy);
                    }
                }
            }
        }
    }
} /* lineTwoStep */

/* here in VECTOR & VCONT modes */
static int
vector(int i, int sy, int dy, int sx, int dx)
{
    struct graphics2 *u = UNIT(0);
    int x0, y0, x1, y1;
    int flags = 0;

    DEBUGF(("v i%d y%c%d x%c%d\r\n", i,
            (sy ? '-' : '+'), dy,
            (sx ? '-' : '+'), dx));
    x0 = u->xpos;
    y0 = u->ypos;

    if (sx) {
        x1 = x0 - dx * u->scale;
        if (x1 < 0) {
            x1 = 0;
            flags = G2_EDGE;
        }
    }
    else {
        x1 = x0 + dx * u->scale;
        if (x1 > 1023) {
            x1 = 1023;
            flags = G2_EDGE;
        }
    }

    if (sy) {
        y1 = y0 - dy * u->scale;
        if (y1 < 0) {
            y1 = 0;
            flags |= G2_EDGE;
        }
    }
    else {
        y1 = y0 + dy * u->scale;
        if (y1 > 1023) {
            y1 = 1023;
            flags |= G2_EDGE;
        }
    }

    DEBUGF(("vector i%d (%d,%d) to (%d,%d)\r\n", i, x0, y0, x1, y1));
    if (i)                              /* XXX need OLD value??? */
        lineTwoStep(x0, y0, x1, y1);

    u->xpos = x1;
    u->ypos = y1;
    u->status |= flags;                 /* ?? */
    return flags;
}

/*
 * incremental vector
 * return true on raster violation
 *
 * i is intensify
 * n is subvector number
 * byte is 4 bits
 */
static int
ipoint(int i, int n, unsigned char byte)
{
    struct graphics2 *u = UNIT(0);
    DEBUGF(("graphics2 ipoint i%d n%d %#o\r\n", i, n, byte));
    if (byte & 010) {                   /* left/right */
        if (byte & 04) {                /* left */
            u->xpos -= u->scale;
            if (u->xpos < 0) {
                u->xpos = 0;            /* XXX wrap? */
                u->status |= G2_EDGE; /* save flags & continue?? */
                return 1;               /* escape */
            }
        }
        else {                          /* right */
            u->xpos += u->scale;
            if (u->xpos > 1023) {
                u->xpos = 1023;         /* XXX wrap? */
                u->status |= G2_EDGE;
                return 1;
            }
        }
    }
    if (byte & 02) {                    /* up/down */
        if (byte & 01) {                /* down */
            u->ypos -= u->scale;
            if (u->ypos < 0) {
                u->ypos = 0;            /* XXX wrap? */
                u->status |= G2_EDGE;
                return 1;
            }
        }
        else {
            u->ypos += u->scale;
            if (u->ypos > 1023) {
                u->ypos = 1023;         /* XXX wrap? */
                u->status |= G2_EDGE;
                return 1;
            }
        }
    }
    if (i)
        point(u->xpos, u->ypos, n);

    return 0;                           /* no escape */
}

/*
 * Execute one GRAPHICS-2 instruction.
 * returns status word
 */
g2word
g2_instruction(g2word inst)
{
    struct graphics2 *u = UNIT(0);

    switch ((inst & 0777777) >> 14) {
    case 000: /* character */
      break;
    case 001: /* parameter */
      break;
    case 002: /* long vector */
      break;
    case 003: /* x-y */
      break;
    case 004: /* short vector */
      break;
    case 006: /* incremental */
      break;
    case 007: /* slave */
      break;
    case 010: case 011: case 012: case 013: /* trap */
    case 014: case 015: case 016: case 017:
      break;
    }

    if (u->status & G2_RUN)
        g2_rfd();                    /* ready for data */
    return u->status;
}

g2word
g2_get_flags(void)
{
    struct graphics2 *u = UNIT(0);
    return u->status & (G2_TRAP|G2_EDGE|G2_LP);
}
