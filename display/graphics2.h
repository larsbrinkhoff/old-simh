/*
 * external interface for graphics2e.c
 * Simulator Independent Bell Labs GRAPHIC-2 Graphic Display Processor
 */

/*
 * Copyright (c) 2018, Philip L. Budne
 * Copyright (c) 2019, Lars Brinkhoff
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

typedef unsigned int g2word;

#define G2_TRAP   0400000
#define G2_EDGE   0200000
#define G2_LP     0100000
#define G2_STOP   0040000
#define G2_COND   0020000
#define G2_BUT    0010000
#define G2_KEY    0004000
#define G2_PHONE  0002000
#define G2_BYTE   0001000
#define G2_CE     0000400
#define G2_IMM    0000200
#define G2_STEP   0000100
#define G2_DATA   0000040
#define G2_OVER   0000020
#define G2_REDGE  0000010
#define G2_LEDGE  0000004
#define G2_TEDGE  0000002
#define G2_BEDGE  0000001

#define G2_DISPLAY_FLAGS \
  (G2_TRAP|G2_EDGE|G2_STOP|G2_COND|G2_BYTE|G2_CE|G2_IMM|G2_STEP| \
   G2_DATA|G2_OVER|G2_REDGE|G2_LEDGE|G2_TEDGE|G2_BEDGE)

/*
 * Calls from host into graphics2.c
 */
g2word gr2_reset(void *);
g2word g2_status(void);
g2word g2_instruction(g2word inst);
g2word g2_get_address(void);
g2word g2_sense(g2word);
g2word g2_buttons(void);
g2word g2_get_lights(void);
g2word g2_get_flags(void);
void g2_cycle(void);
void g2_clear_flags(g2word);
void g2_set_address(g2word addr);
void g2_set_flags(g2word);
void g2_set_lights(g2word);

/*
 * calls from type340.c into host simulator
 */
extern g2word g2_fetch(g2word);
extern void g2_store(g2word, g2word);
extern void g2_lp_int(g2word x, g2word y);
extern void g2_rfd(void);
