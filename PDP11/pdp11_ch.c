/* pdp11_ch.c: CH11 Chaosnet interface.
  ------------------------------------------------------------------------------

   Copyright (c) 2018, Lars Brinkhoff.

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
   THE AUTHOR BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
   IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
   CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

   Except as contained in this notice, the name of the author shall not be
   used in advertising or otherwise to promote the sale, use or other dealings
   in this Software without prior written authorization from the author.

  ------------------------------------------------------------------------------

*/

#if defined (VM_PDP11)
#include "pdp11_defs.h"

#elif defined (VM_PDP10)
#include "pdp10_defs.h"

#elif defined (VM_VAX)
#include "vax_defs.h"

#else
#error The CH11 device only works with Unibus machines.
#endif

#include "sim_tmxr.h"

/* CSR bits */
#define TIE   0000001 /* Timer interrupt enable */
#define LOOP  0000002 /* Loop back */
#define SPY   0000004 /* Loop back */
#define CRX   0000010 /* Clear receiver */
#define RXIE  0000020 /* Receive interrupt enable */
#define TXIE  0000040 /* Transmit interrupt enable */
#define TXA   0000100 /* Transmit abort */
#define TXD   0000200 /* Transmit done */
#define CTX   0000400 /* Clear transmitter */
#define LOST  0017000 /* Lost count */
#define RESET 0020000 /* Reset */
#define CRC   0040000 /* CRC error */
#define RXD   0100000 /* Receive done */

#define STATUS_BITS (TIE|LOOP|SPY|RXIE|TXIE|TXA|TXD|LOST|CRC|RXD)
#define COMMAND_BITS (TIE|LOOP|SPY|RXIE|TXIE)

#define CHUDP_HEADER 4
#define IOLN_CH 010
#define DBG_TRC  0x0001

t_stat ch_svc(UNIT *);
t_stat ch_reset (DEVICE *);
t_stat ch_attach (UNIT *, CONST char *);
t_stat ch_detach (UNIT *);
t_stat ch_rd(int32 *, int32, int32);
t_stat ch_wr(int32, int32, int32);
int32 ch_int (void);
t_stat ch_ex (t_value *, t_addr, UNIT *, int32);
t_stat ch_dep (t_value, t_addr, UNIT *, int32);
t_stat ch_help (FILE *, DEVICE *, UNIT *, int32, const char *);
const char *ch_description (DEVICE *);

int port = 42043;
int status;
int address;
int rx_count;
int tx_count;
int lost_count;
int rx_buffer[252 + CHUDP_HEADER];
int tx_buffer[252 + CHUDP_HEADER];

TMLN udp_lines[1] = { {0} };
TMXR udp_tmxr = { 1, NULL, 0, udp_lines};

UNIT ch_unit[] = {
  { UDATA (&ch_svc, UNIT_IDLE|UNIT_ATTABLE, 0) },
};

REG ch_reg[] = {
  { NULL }  };

MTAB ch_mod[] = {
#if defined (VM_PDP11)
  { MTAB_XTD|MTAB_VDV|MTAB_VALR, 010, "ADDRESS", "ADDRESS",
    &set_addr, &show_addr, NULL },
  { MTAB_XTD|MTAB_VDV, 0, NULL, "AUTOCONFIGURE",
    &set_addr_flt, NULL, NULL },
  { MTAB_XTD|MTAB_VDV|MTAB_VALR, 0, "VECTOR", NULL,
    &set_vec, &show_vec, NULL },
#else
  { MTAB_XTD|MTAB_VDV, 0, "ADDRESS", NULL,
    NULL, &show_addr, NULL, "Unibus address" },
  { MTAB_XTD|MTAB_VDV, 0, "VECTOR", NULL,
    NULL, &show_vec, NULL, "Interrupt vector" },
#endif
  { 0 },
};

DIB ch_dib = {
  IOBA_AUTO, IOLN_CH, &ch_rd, &ch_wr,
  1, IVCL (CH), 0, { &ch_int }, IOLN_CH
};

DEBTAB ch_debug[] = {
    { "TRC",       DBG_TRC,   "Trace" },
    { 0 }
};

DEVICE ch_dev = {
    "CH", ch_unit, ch_reg, ch_mod,
    1, 8, 16, 1, 8, 16,
    &ch_ex, &ch_dep, &ch_reset,
    NULL, &ch_attach, &ch_detach,
    &ch_dib, DEV_DISABLE | DEV_DIS | DEV_UBUS | DEV_DEBUG,
    0, ch_debug, NULL, NULL, &ch_help, NULL, NULL,
    &ch_description
  };

void ch_transmit ()
{
  tmxr_poll_tx (&udp_tmxr);
  if (tmxr_put_packet_ln (&udp_lines[0], (const uint8 *)&tx_buffer,
                          CHUDP_HEADER + 2 * (size_t)tx_count) == SCPE_OK) {
    status |= TXD;
    ch_int ();
  }
}

void ch_receive (void)
{
  size_t count;
  const uint8 *p;

  tmxr_poll_rx (&udp_tmxr);
  tmxr_get_packet_ln (&udp_lines[0], &p, &count);
  if (p) {
    sim_debug (DBG_TRC, &ch_dev, "Received packet, %d bytes\n", (int)count);
    if ((status & RXD) == 0) {
      count = (count + 1) & 0376;
      memcpy (rx_buffer + (252 - count), p + CHUDP_HEADER, count);
      rx_count = count >> 1;
      sim_debug (DBG_TRC, &ch_dev, "Rx count, %d\n", rx_count);
      status |= RXD;
      ch_int ();
    }
  } else {
    sim_debug (DBG_TRC, &ch_dev, "Lost packet\n");
    lost_count++;
  }
}

t_stat ch_rd (int32 *data, int32 PA, int32 access)
{
  int reg = (PA >> 1) & 07;

  switch (reg) {
  case 00: /* Status */
    *data = (status & STATUS_BITS) | ((lost_count << 9) & LOST);
    sim_debug (DBG_TRC, &ch_dev, "Read status: %06o\n", *data);
    break;
  case 01: /* Address */
    *data = address;
    sim_debug (DBG_TRC, &ch_dev, "Read address: %06o\n", *data);
    break;
  case 02: /* Read */
    if (rx_count == 0) {
      *data = 0;
      sim_debug (DBG_TRC, &ch_dev, "Read empty buffer\n");
    } else {
      *data = rx_buffer[126-rx_count];
      sim_debug (DBG_TRC, &ch_dev, "Read buffer word %d: %06o\n",
                 126-rx_count, *data);
      rx_count--;
    }
    break;
  case 03: /* Count */
    *data = ((16 * rx_count) - 1) & 07777;
    sim_debug (DBG_TRC, &ch_dev, "Read bit count: %d\n", *data);
    break;
  case 05: /* Start */
    ch_transmit ();
    sim_debug (DBG_TRC, &ch_dev, "Start transmission\n");
    status &= ~TXD;
    tx_count = 0;
    *data = address;
    break;
  default:
    *data = 0;
    break;
  }

  return SCPE_OK;
}

t_stat ch_command (int32 data)
{
  if (data & CRX) {
    sim_debug (DBG_TRC, &ch_dev, "Clear RX\n");
    status &= ~RXD;
    lost_count = 0;
  }
  if (data & CTX) {
    sim_debug (DBG_TRC, &ch_dev, "Clear TX\n");
    status |= TXD;
  }
  if (data & RESET) {
    sim_debug (DBG_TRC, &ch_dev, "Reset\n");
    ch_reset (&ch_dev);
  }
}

t_stat ch_wr (int32 data, int32 PA, int32 access)
{
  int reg = (PA >> 1) & 07;

  switch (reg) {
  case 00: /* Command */
    if (data & TIE)
      sim_debug (DBG_TRC, &ch_dev, "Timer interrupt enable\n");
    if (data & LOOP)
      sim_debug (DBG_TRC, &ch_dev, "Loopback\n");
    if (data & SPY)
      sim_debug (DBG_TRC, &ch_dev, "Spy mode\n");
    if (data & RXIE)
      sim_debug (DBG_TRC, &ch_dev, "RX interrupt enable\n");
    if (data & TXIE)
      sim_debug (DBG_TRC, &ch_dev, "TX interrupt enable\n");
    status = (status & ~COMMAND_BITS) | (data & COMMAND_BITS);
    return ch_command (data);
  case 01: /* Write */
    if (tx_count < 126) {
      sim_debug (DBG_TRC, &ch_dev, "Write buffer word %d: %06o\n",
                 tx_count, data);
      tx_buffer[tx_count + CHUDP_HEADER] = data;
      tx_count++;
    } else {
      sim_debug (DBG_TRC, &ch_dev, "Write buffer overflow\n");
    }
    break;
  }

  return SCPE_OK;
}

t_stat ch_svc(UNIT *uptr)
{
  ch_receive ();
  return SCPE_OK;
}

int32 ch_int (void)
{
  if (status & (RXD | TXD)) {
    sim_debug (DBG_TRC, &ch_dev, "Interrupt\n");
    return 0270;
  }
  return 0;
}

t_stat ch_attach (UNIT *uptr, CONST char *cptr)
{
  char linkinfo[256];

  sprintf (linkinfo, "Buffer=%d,Line=%d,%d,UDP,Connect=%s",
           (int)sizeof tx_buffer, 0, port, cptr);
  tmxr_open_master (&udp_tmxr, linkinfo);

  return SCPE_OK;
}

t_stat ch_detach (UNIT *uptr)
{
  tmxr_detach_ln (&udp_lines[0]);
  return SCPE_OK;
}

t_stat ch_reset (DEVICE *dptr)
{

  status = 0;
  rx_count = 0;
  tx_count = 0;
  lost_count = 0;

  tx_buffer[0] = 1; /* CHUDP header */
  tx_buffer[1] = 1;
  tx_buffer[2] = 0;
  tx_buffer[3] = 0;

  return auto_config (dptr->name, (dptr->flags & DEV_DIS)? 0 : 1);  /* auto config */
}

t_stat ch_ex (t_value *vptr, t_addr addr, UNIT *uptr, int32 sw)
{
  return SCPE_OK;
}

t_stat ch_dep (t_value val, t_addr addr, UNIT *uptr, int32 sw)
{
  return SCPE_OK;
}

const char *ch_description (DEVICE *dptr)
{
  return "CH11 Chaosnet interface";
}

t_stat ch_help (FILE *st, DEVICE *dptr, UNIT *uptr, int32 flag, const char *cptr)
{
  fprintf (st, "CH11 Unibus Chaosnet interface\n\n");
}
