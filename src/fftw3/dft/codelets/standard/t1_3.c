/*
 * Copyright (c) 2003 Matteo Frigo
 * Copyright (c) 2003 Massachusetts Institute of Technology
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

/* This file was automatically generated --- DO NOT EDIT */
/* Generated on Sat Jul  5 21:29:51 EDT 2003 */

#include "codelet-dft.h"

/* Generated by: /homee/stevenj/cvs/fftw3.0.1/genfft/gen_twiddle -compact -variables 4 -n 3 -name t1_3 -include t.h */

/*
 * This function contains 16 FP additions, 12 FP multiplications,
 * (or, 10 additions, 6 multiplications, 6 fused multiply/add),
 * 15 stack variables, and 12 memory accesses
 */
/*
 * Generator Id's : 
 * $Id: t1_3.c,v 1.1 2008-10-17 06:11:09 scuri Exp $
 * $Id: t1_3.c,v 1.1 2008-10-17 06:11:09 scuri Exp $
 * $Id: t1_3.c,v 1.1 2008-10-17 06:11:09 scuri Exp $
 */

#include "t.h"

static const R *t1_3(R *ri, R *ii, const R *W, stride ios, int m, int dist)
{
     DK(KP866025403, +0.866025403784438646763723170752936183471402627);
     DK(KP500000000, +0.500000000000000000000000000000000000000000000);
     int i;
     for (i = m; i > 0; i = i - 1, ri = ri + dist, ii = ii + dist, W = W + 4) {
	  E T1, Ti, T6, Te, Tb, Tf, Tc, Th;
	  T1 = ri[0];
	  Ti = ii[0];
	  {
	       E T3, T5, T2, T4;
	       T3 = ri[WS(ios, 1)];
	       T5 = ii[WS(ios, 1)];
	       T2 = W[0];
	       T4 = W[1];
	       T6 = FMA(T2, T3, T4 * T5);
	       Te = FNMS(T4, T3, T2 * T5);
	  }
	  {
	       E T8, Ta, T7, T9;
	       T8 = ri[WS(ios, 2)];
	       Ta = ii[WS(ios, 2)];
	       T7 = W[2];
	       T9 = W[3];
	       Tb = FMA(T7, T8, T9 * Ta);
	       Tf = FNMS(T9, T8, T7 * Ta);
	  }
	  Tc = T6 + Tb;
	  Th = Te + Tf;
	  ri[0] = T1 + Tc;
	  ii[0] = Th + Ti;
	  {
	       E Td, Tg, Tj, Tk;
	       Td = FNMS(KP500000000, Tc, T1);
	       Tg = KP866025403 * (Te - Tf);
	       ri[WS(ios, 2)] = Td - Tg;
	       ri[WS(ios, 1)] = Td + Tg;
	       Tj = KP866025403 * (Tb - T6);
	       Tk = FNMS(KP500000000, Th, Ti);
	       ii[WS(ios, 1)] = Tj + Tk;
	       ii[WS(ios, 2)] = Tk - Tj;
	  }
     }
     return W;
}

static const tw_instr twinstr[] = {
     {TW_FULL, 0, 3},
     {TW_NEXT, 1, 0}
};

static const ct_desc desc = { 3, "t1_3", twinstr, {10, 6, 6, 0}, &GENUS, 0, 0, 0 };

void X(codelet_t1_3) (planner *p) {
     X(kdft_dit_register) (p, t1_3, &desc);
}
