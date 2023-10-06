/*
 * jdarith.c
 *
 * Developed 1997 by Guido Vollbeding.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 *
 * This file contains portable arithmetic entropy decoding routines for JPEG
 * (implementing the ISO/IEC IS 10918-1 and CCITT Recommendation ITU-T T.81).
 *
 * Both sequential and progressive modes are supported in this single module.
 *
 * Suspension is not currently supported in this module.
 */

#define JPEG_INTERNALS
#include "jinclude.h"
#include "jpeglib.h"

#define V(a,b,c,d) (((INT32)a << 16) | ((INT32)c << 8) | ((INT32)d << 7) | b)

const INT32 jaritab[113] = {
/*
 * Index, Qe_Value, Next_Index_LPS, Next_Index_MPS, Switch_MPS
 */
/*   0 */  V( 0x5a1d,   1,   1, 1 ),
/*   1 */  V( 0x2586,  14,   2, 0 ),
/*   2 */  V( 0x1114,  16,   3, 0 ),
/*   3 */  V( 0x080b,  18,   4, 0 ),
/*   4 */  V( 0x03d8,  20,   5, 0 ),
/*   5 */  V( 0x01da,  23,   6, 0 ),
/*   6 */  V( 0x00e5,  25,   7, 0 ),
/*   7 */  V( 0x006f,  28,   8, 0 ),
/*   8 */  V( 0x0036,  30,   9, 0 ),
/*   9 */  V( 0x001a,  33,  10, 0 ),
/*  10 */  V( 0x000d,  35,  11, 0 ),
/*  11 */  V( 0x0006,   9,  12, 0 ),
/*  12 */  V( 0x0003,  10,  13, 0 ),
/*  13 */  V( 0x0001,  12,  13, 0 ),
/*  14 */  V( 0x5a7f,  15,  15, 1 ),
/*  15 */  V( 0x3f25,  36,  16, 0 ),
/*  16 */  V( 0x2cf2,  38,  17, 0 ),
/*  17 */  V( 0x207c,  39,  18, 0 ),
/*  18 */  V( 0x17b9,  40,  19, 0 ),
/*  19 */  V( 0x1182,  42,  20, 0 ),
/*  20 */  V( 0x0cef,  43,  21, 0 ),
/*  21 */  V( 0x09a1,  45,  22, 0 ),
/*  22 */  V( 0x072f,  46,  23, 0 ),
/*  23 */  V( 0x055c,  48,  24, 0 ),
/*  24 */  V( 0x0406,  49,  25, 0 ),
/*  25 */  V( 0x0303,  51,  26, 0 ),
/*  26 */  V( 0x0240,  52,  27, 0 ),
/*  27 */  V( 0x01b1,  54,  28, 0 ),
/*  28 */  V( 0x0144,  56,  29, 0 ),
/*  29 */  V( 0x00f5,  57,  30, 0 ),
/*  30 */  V( 0x00b7,  59,  31, 0 ),
/*  31 */  V( 0x008a,  60,  32, 0 ),
/*  32 */  V( 0x0068,  62,  33, 0 ),
/*  33 */  V( 0x004e,  63,  34, 0 ),
/*  34 */  V( 0x003b,  32,  35, 0 ),
/*  35 */  V( 0x002c,  33,   9, 0 ),
/*  36 */  V( 0x5ae1,  37,  37, 1 ),
/*  37 */  V( 0x484c,  64,  38, 0 ),
/*  38 */  V( 0x3a0d,  65,  39, 0 ),
/*  39 */  V( 0x2ef1,  67,  40, 0 ),
/*  40 */  V( 0x261f,  68,  41, 0 ),
/*  41 */  V( 0x1f33,  69,  42, 0 ),
/*  42 */  V( 0x19a8,  70,  43, 0 ),
/*  43 */  V( 0x1518,  72,  44, 0 ),
/*  44 */  V( 0x1177,  73,  45, 0 ),
/*  45 */  V( 0x0e74,  74,  46, 0 ),
/*  46 */  V( 0x0bfb,  75,  47, 0 ),
/*  47 */  V( 0x09f8,  77,  48, 0 ),
/*  48 */  V( 0x0861,  78,  49, 0 ),
/*  49 */  V( 0x0706,  79,  50, 0 ),
/*  50 */  V( 0x05cd,  48,  51, 0 ),
/*  51 */  V( 0x04de,  50,  52, 0 ),
/*  52 */  V( 0x040f,  50,  53, 0 ),
/*  53 */  V( 0x0363,  51,  54, 0 ),
/*  54 */  V( 0x02d4,  52,  55, 0 ),
/*  55 */  V( 0x025c,  53,  56, 0 ),
/*  56 */  V( 0x01f8,  54,  57, 0 ),
/*  57 */  V( 0x01a4,  55,  58, 0 ),
/*  58 */  V( 0x0160,  56,  59, 0 ),
/*  59 */  V( 0x0125,  57,  60, 0 ),
/*  60 */  V( 0x00f6,  58,  61, 0 ),
/*  61 */  V( 0x00cb,  59,  62, 0 ),
/*  62 */  V( 0x00ab,  61,  63, 0 ),
/*  63 */  V( 0x008f,  61,  32, 0 ),
/*  64 */  V( 0x5b12,  65,  65, 1 ),
/*  65 */  V( 0x4d04,  80,  66, 0 ),
/*  66 */  V( 0x412c,  81,  67, 0 ),
/*  67 */  V( 0x37d8,  82,  68, 0 ),
/*  68 */  V( 0x2fe8,  83,  69, 0 ),
/*  69 */  V( 0x293c,  84,  70, 0 ),
/*  70 */  V( 0x2379,  86,  71, 0 ),
/*  71 */  V( 0x1edf,  87,  72, 0 ),
/*  72 */  V( 0x1aa9,  87,  73, 0 ),
/*  73 */  V( 0x174e,  72,  74, 0 ),
/*  74 */  V( 0x1424,  72,  75, 0 ),
/*  75 */  V( 0x119c,  74,  76, 0 ),
/*  76 */  V( 0x0f6b,  74,  77, 0 ),
/*  77 */  V( 0x0d51,  75,  78, 0 ),
/*  78 */  V( 0x0bb6,  77,  79, 0 ),
/*  79 */  V( 0x0a40,  77,  48, 0 ),
/*  80 */  V( 0x5832,  80,  81, 1 ),
/*  81 */  V( 0x4d1c,  88,  82, 0 ),
/*  82 */  V( 0x438e,  89,  83, 0 ),
/*  83 */  V( 0x3bdd,  90,  84, 0 ),
/*  84 */  V( 0x34ee,  91,  85, 0 ),
/*  85 */  V( 0x2eae,  92,  86, 0 ),
/*  86 */  V( 0x299a,  93,  87, 0 ),
/*  87 */  V( 0x2516,  86,  71, 0 ),
/*  88 */  V( 0x5570,  88,  89, 1 ),
/*  89 */  V( 0x4ca9,  95,  90, 0 ),
/*  90 */  V( 0x44d9,  96,  91, 0 ),
/*  91 */  V( 0x3e22,  97,  92, 0 ),
/*  92 */  V( 0x3824,  99,  93, 0 ),
/*  93 */  V( 0x32b4,  99,  94, 0 ),
/*  94 */  V( 0x2e17,  93,  86, 0 ),
/*  95 */  V( 0x56a8,  95,  96, 1 ),
/*  96 */  V( 0x4f46, 101,  97, 0 ),
/*  97 */  V( 0x47e5, 102,  98, 0 ),
/*  98 */  V( 0x41cf, 103,  99, 0 ),
/*  99 */  V( 0x3c3d, 104, 100, 0 ),
/* 100 */  V( 0x375e,  99,  93, 0 ),
/* 101 */  V( 0x5231, 105, 102, 0 ),
/* 102 */  V( 0x4c0f, 106, 103, 0 ),
/* 103 */  V( 0x4639, 107, 104, 0 ),
/* 104 */  V( 0x415e, 103,  99, 0 ),
/* 105 */  V( 0x5627, 105, 106, 1 ),
/* 106 */  V( 0x50e7, 108, 107, 0 ),
/* 107 */  V( 0x4b85, 109, 103, 0 ),
/* 108 */  V( 0x5597, 110, 109, 0 ),
/* 109 */  V( 0x504f, 111, 107, 0 ),
/* 110 */  V( 0x5a10, 110, 111, 1 ),
/* 111 */  V( 0x5522, 112, 109, 0 ),
/* 112 */  V( 0x59eb, 112, 111, 1 )
};


/* Expanded entropy decoder object for arithmetic decoding. */

typedef struct {
  struct jpeg_entropy_decoder pub; /* public fields */

  INT32 c;       /* C register, base of coding interval + input bit buffer */
  INT32 a;               /* A register, normalized size of coding interval */
  int ct;     /* bit shift counter, # of bits left in bit buffer part of C */
                                                         /* init: ct = -16 */
                                                         /* run: ct = 0..7 */
                                                         /* error: ct = -1 */
  int last_dc_val[MAX_COMPS_IN_SCAN]; /* last DC coef for each component */
  int dc_context[MAX_COMPS_IN_SCAN]; /* context index for DC conditioning */

  unsigned int restarts_to_go;	/* MCUs left in this restart interval */

  /* Pointers to statistics areas (these workspaces have image lifespan) */
  unsigned char * dc_stats[NUM_ARITH_TBLS];
  unsigned char * ac_stats[NUM_ARITH_TBLS];
} arith_entropy_decoder;

typedef arith_entropy_decoder * arith_entropy_ptr;

/* The following two definitions specify the allocation chunk size
 * for the statistics area.
 * According to sections F.1.4.4.1.3 and F.1.4.4.2, we need at least
 * 49 statistics bins for DC, and 245 statistics bins for AC coding.
 * Note that we use one additional AC bin for codings with fixed
 * probability (0.5), thus the minimum number for AC is 246.
 *
 * We use a compact representation with 1 byte per statistics bin,
 * thus the numbers directly represent byte sizes.
 * This 1 byte per statistics bin contains the meaning of the MPS
 * (more probable symbol) in the highest bit (mask 0x80), and the
 * index into the probability estimation state machine table
 * in the lower bits (mask 0x7F).
 */

#define DC_STAT_BINS 64
#define AC_STAT_BINS 256


LOCAL(int)
get_byte (j_decompress_ptr cinfo)
/* Read next input byte; we do not support suspension in this module. */
{
  struct jpeg_source_mgr * src = cinfo->src;

  if (src->bytes_in_buffer == 0)
    if (! (*src->fill_input_buffer) (cinfo))
      ERREXIT(cinfo, JERR_CANT_SUSPEND);
  src->bytes_in_buffer--;
  return GETJOCTET(*src->next_input_byte++);
}


/*
 * The core arithmetic decoding routine (common in JPEG and JBIG).
 * This needs to go as fast as possible.
 * Machine-dependent optimization facilities
 * are not utilized in this portable implementation.
 * However, this code should be fairly efficient and
 * may be a good base for further optimizations anyway.
 *
 * Return value is 0 or 1 (binary decision).
 *
 * Note: I've changed the handling of the code base & bit
 * buffer register C compared to other implementations
 * based on the standards layout & procedures.
 * While it also contains both the actual base of the
 * coding interval (16 bits) and the next-bits buffer,
 * the cut-point between these two parts is floating
 * (instead of fixed) with the bit shift counter CT.
 * Thus, we also need only one (variable instead of
 * fixed size) shift for the LPS/MPS decision, and
 * we can get away with any renormalization update
 * of C (except for new data insertion, of course).
 *
 * I've also introduced a new scheme for accessing
 * the probability estimation state machine table,
 * derived from Markus Kuhn's JBIG implementation.
 */

LOCAL(int)
arith_decode (j_decompress_ptr cinfo, unsigned char *st)
{
  extern const INT32 jaritab[];
  register arith_entropy_ptr e = (arith_entropy_ptr) cinfo->entropy;
  register unsigned char nl, nm;
  register INT32 qe, temp;
  register int sv, data;

  /* Renormalization & data input per section D.2.6 */
  while (e->a < 0x8000L) {
    if (--e->ct < 0) {
      /* Need to fetch next data byte */
      if (cinfo->unread_marker)
	data = 0;		/* stuff zero data */
      else {
	data = get_byte(cinfo);	/* read next input byte */
	if (data == 0xFF) {	/* zero stuff or marker code */
	  do data = get_byte(cinfo);
	  while (data == 0xFF);	/* swallow extra 0xFF bytes */
	  if (data == 0)
	    data = 0xFF;	/* discard stuffed zero byte */
	  else {
	    /* Note: Different from the Huffman decoder, hitting
	     * a marker while processing the compressed data
	     * segment is legal in arithmetic coding.
	     * The convention is to supply zero data
	     * then until decoding is complete.
	     */
	    cinfo->unread_marker = data;
	    data = 0;
	  }
	}
      }
      e->c = (e->c << 8) | data; /* insert data into C register */
      if ((e->ct += 8) < 0)	 /* update bit shift counter */
	/* Need more initial bytes */
	if (++e->ct == 0)
	  /* Got 2 initial bytes -> re-init A and exit loop */
	  e->a = 0x8000L; /* => e->a = 0x10000L after loop exit */
    }
    e->a <<= 1;
  }

  /* Fetch values from our compact representation of Table D.2:
   * Qe values and probability estimation state machine
   */
  sv = *st;
  qe = jaritab[sv & 0x7F];	/* => Qe_Value */
  nl = qe & 0xFF; qe >>= 8;	/* Next_Index_LPS + Switch_MPS */
  nm = qe & 0xFF; qe >>= 8;	/* Next_Index_MPS */

  /* Decode & estimation procedures per sections D.2.4 & D.2.5 */
  temp = e->a - qe;
  e->a = temp;
  temp <<= e->ct;
  if (e->c >= temp) {
    e->c -= temp;
    /* Conditional LPS (less probable symbol) exchange */
    if (e->a < qe) {
      e->a = qe;
      *st = (sv & 0x80) ^ nm;	/* Estimate_after_MPS */
    } else {
      e->a = qe;
      *st = (sv & 0x80) ^ nl;	/* Estimate_after_LPS */
      sv ^= 0x80;		/* Exchange LPS/MPS */
    }
  } else if (e->a < 0x8000L) {
    /* Conditional MPS (more probable symbol) exchange */
    if (e->a < qe) {
      *st = (sv & 0x80) ^ nl;	/* Estimate_after_LPS */
      sv ^= 0x80;		/* Exchange LPS/MPS */
    } else {
      *st = (sv & 0x80) ^ nm;	/* Estimate_after_MPS */
    }
  }

  return sv >> 7;
}


/*
 * Check for a restart marker & resynchronize decoder.
 */

LOCAL(void)
process_restart (j_decompress_ptr cinfo)
{
  arith_entropy_ptr entropy = (arith_entropy_ptr) cinfo->entropy;
  int ci;
  jpeg_component_info * compptr;

  /* Advance past the RSTn marker */
  if (! (*cinfo->marker->read_restart_marker) (cinfo))
    ERREXIT(cinfo, JERR_CANT_SUSPEND);

  for (ci = 0; ci < cinfo->comps_in_scan; ci++) {
    compptr = cinfo->cur_comp_info[ci];
    /* Re-initialize statistics areas */
    if (cinfo->progressive_mode == 0 || (cinfo->Ss == 0 && cinfo->Ah == 0)) {
      MEMZERO(entropy->dc_stats[compptr->dc_tbl_no], DC_STAT_BINS);
      /* Reset DC predictions to 0 */
      entropy->last_dc_val[ci] = 0;
      entropy->dc_context[ci] = 0;
    }
    if (cinfo->progressive_mode == 0 || cinfo->Ss) {
      MEMZERO(entropy->ac_stats[compptr->ac_tbl_no], AC_STAT_BINS);
    }
  }

  /* Reset arithmetic decoding variables */
  entropy->c = 0;
  entropy->a = 0;
  entropy->ct = -16;	/* force reading 2 initial bytes to fill C */

  /* Reset restart counter */
  entropy->restarts_to_go = cinfo->restart_interval;
}


/*
 * Arithmetic MCU decoding.
 * Each of these routines decodes and returns one MCU's worth of
 * arithmetic-compressed coefficients.
 * The coefficients are reordered from zigzag order into natural array order,
 * but are not dequantized.
 *
 * The i'th block of the MCU is stored into the block pointed to by
 * MCU_data[i].  WE ASSUME THIS AREA IS INITIALLY ZEROED BY THE CALLER.
 */

/*
 * MCU decoding for DC initial scan (either spectral selection,
 * or first pass of successive approximation).
 */

METHODDEF(boolean)
decode_mcu_DC_first (j_decompress_ptr cinfo, JBLOCKROW *MCU_data)
{
  arith_entropy_ptr entropy = (arith_entropy_ptr) cinfo->entropy;
  JBLOCKROW block;
  unsigned char *st;
  int blkn, ci, tbl, sign;
  int v, m;

  /* Process restart marker if needed */
  if (cinfo->restart_interval) {
    if (entropy->restarts_to_go == 0)
      process_restart(cinfo);
    entropy->restarts_to_go--;
  }

  if (entropy->ct == -1) return TRUE;	/* if error do nothing */

  /* Outer loop handles each block in the MCU */

  for (blkn = 0; blkn < cinfo->blocks_in_MCU; blkn++) {
    block = MCU_data[blkn];
    ci = cinfo->MCU_membership[blkn];
    tbl = cinfo->cur_comp_info[ci]->dc_tbl_no;

    /* Sections F.2.4.1 & F.1.4.4.1: Decoding of DC coefficients */

    /* Table F.4: Point to statistics bin S0 for DC coefficient coding */
    st = entropy->dc_stats[tbl] + entropy->dc_context[ci];

    /* Figure F.19: Decode_DC_DIFF */
    if (arith_decode(cinfo, st) == 0)
      entropy->dc_context[ci] = 0;
    else {
      /* Figure F.21: Decoding nonzero value v */
      /* Figure F.22: Decoding the sign of v */
      sign = arith_decode(cinfo, st + 1);
      st += 2; st += sign;
      /* Figure F.23: Decoding the magnitude category of v */
      if ((m = arith_decode(cinfo, st)) != 0) {
	st = entropy->dc_stats[tbl] + 20;	/* Table F.4: X1 = 20 */
	while (arith_decode(cinfo, st)) {
	  if ((m <<= 1) == 0x8000) {
	    WARNMS(cinfo, JWRN_ARITH_BAD_CODE);
	    entropy->ct = -1;			/* magnitude overflow */
	    return TRUE;
	  }
	  st += 1;
	}
      }
      /* Section F.1.4.4.1.2: Establish dc_context conditioning category */
      if (m < (int) (((INT32) 1 << cinfo->arith_dc_L[tbl]) >> 1))
	entropy->dc_context[ci] = 0;		   /* zero diff category */
      else if (m > (int) (((INT32) 1 << cinfo->arith_dc_U[tbl]) >> 1))
	entropy->dc_context[ci] = 12 + (sign * 4); /* large diff category */
      else
	entropy->dc_context[ci] = 4 + (sign * 4);  /* small diff category */
      v = m;
      /* Figure F.24: Decoding the magnitude bit pattern of v */
      st += 14;
      while (m >>= 1)
	if (arith_decode(cinfo, st)) v |= m;
      v += 1; if (sign) v = -v;
      entropy->last_dc_val[ci] += v;
    }

    /* Scale and output the DC coefficient (assumes jpeg_natural_order[0]=0) */
    (*block)[0] = (JCOEF) (entropy->last_dc_val[ci] << cinfo->Al);
  }

  return TRUE;
}


/*
 * MCU decoding for AC initial scan (either spectral selection,
 * or first pass of successive approximation).
 */

METHODDEF(boolean)
decode_mcu_AC_first (j_decompress_ptr cinfo, JBLOCKROW *MCU_data)
{
  arith_entropy_ptr entropy = (arith_entropy_ptr) cinfo->entropy;
  JBLOCKROW block;
  unsigned char *st;
  int tbl, sign, k;
  int v, m;

  /* Process restart marker if needed */
  if (cinfo->restart_interval) {
    if (entropy->restarts_to_go == 0)
      process_restart(cinfo);
    entropy->restarts_to_go--;
  }

  if (entropy->ct == -1) return TRUE;	/* if error do nothing */

  /* There is always only one block per MCU */
  block = MCU_data[0];
  tbl = cinfo->cur_comp_info[0]->ac_tbl_no;

  /* Sections F.2.4.2 & F.1.4.4.2: Decoding of AC coefficients */

  /* Figure F.20: Decode_AC_coefficients */
  for (k = cinfo->Ss; k <= cinfo->Se; k++) {
    st = entropy->ac_stats[tbl] + 3 * (k - 1);
    if (arith_decode(cinfo, st)) break;		/* EOB flag */
    while (arith_decode(cinfo, st + 1) == 0) {
      st += 3; k++;
      if (k > cinfo->Se) {
	WARNMS(cinfo, JWRN_ARITH_BAD_CODE);
	entropy->ct = -1;			/* spectral overflow */
	return TRUE;
      }
    }
    /* Figure F.21: Decoding nonzero value v */
    /* Figure F.22: Decoding the sign of v */
    entropy->ac_stats[tbl][245] = 0;
    sign = arith_decode(cinfo, entropy->ac_stats[tbl] + 245);
    st += 2;
    /* Figure F.23: Decoding the magnitude category of v */
    if ((m = arith_decode(cinfo, st)) != 0) {
      if (arith_decode(cinfo, st)) {
	m <<= 1;
	st = entropy->ac_stats[tbl] +
	     (k <= cinfo->arith_ac_K[tbl] ? 189 : 217);
	while (arith_decode(cinfo, st)) {
	  if ((m <<= 1) == 0x8000) {
	    WARNMS(cinfo, JWRN_ARITH_BAD_CODE);
	    entropy->ct = -1;			/* magnitude overflow */
	    return TRUE;
	  }
	  st += 1;
	}
      }
    }
    v = m;
    /* Figure F.24: Decoding the magnitude bit pattern of v */
    st += 14;
    while (m >>= 1)
      if (arith_decode(cinfo, st)) v |= m;
    v += 1; if (sign) v = -v;
    /* Scale and output coefficient in natural (dezigzagged) order */
    (*block)[jpeg_natural_order[k]] = (JCOEF) (v << cinfo->Al);
  }

  return TRUE;
}


/*
 * MCU decoding for DC successive approximation refinement scan.
 */

METHODDEF(boolean)
decode_mcu_DC_refine (j_decompress_ptr cinfo, JBLOCKROW *MCU_data)
{
  arith_entropy_ptr entropy = (arith_entropy_ptr) cinfo->entropy;
  unsigned char st[4];
  int p1, blkn;

  /* Process restart marker if needed */
  if (cinfo->restart_interval) {
    if (entropy->restarts_to_go == 0)
      process_restart(cinfo);
    entropy->restarts_to_go--;
  }

  p1 = 1 << cinfo->Al;		/* 1 in the bit position being coded */

  /* Outer loop handles each block in the MCU */

  for (blkn = 0; blkn < cinfo->blocks_in_MCU; blkn++) {
    st[0] = 0;	/* use fixed probability estimation */
    /* Encoded data is simply the next bit of the two's-complement DC value */
    if (arith_decode(cinfo, st))
      MCU_data[blkn][0][0] |= p1;
  }

  return TRUE;
}


/*
 * MCU decoding for AC successive approximation refinement scan.
 */

METHODDEF(boolean)
decode_mcu_AC_refine (j_decompress_ptr cinfo, JBLOCKROW *MCU_data)
{
  arith_entropy_ptr entropy = (arith_entropy_ptr) cinfo->entropy;
  JBLOCKROW block;
  JCOEFPTR thiscoef;
  unsigned char *st;
  int tbl, k, kex;
  int p1, m1;

  /* Process restart marker if needed */
  if (cinfo->restart_interval) {
    if (entropy->restarts_to_go == 0)
      process_restart(cinfo);
    entropy->restarts_to_go--;
  }

  if (entropy->ct == -1) return TRUE;	/* if error do nothing */

  /* There is always only one block per MCU */
  block = MCU_data[0];
  tbl = cinfo->cur_comp_info[0]->ac_tbl_no;

  p1 = 1 << cinfo->Al;		/* 1 in the bit position being coded */
  m1 = (-1) << cinfo->Al;	/* -1 in the bit position being coded */

  /* Establish EOBx (previous stage end-of-block) index */
  for (kex = cinfo->Se + 1; kex > 1; kex--)
    if ((*block)[jpeg_natural_order[kex - 1]]) break;

  for (k = cinfo->Ss; k <= cinfo->Se; k++) {
    st = entropy->ac_stats[tbl] + 3 * (k - 1);
    if (k >= kex)
      if (arith_decode(cinfo, st)) break;	/* EOB flag */
    for (;;) {
      thiscoef = *block + jpeg_natural_order[k];
      if (*thiscoef) {				/* previously nonzero coef */
	if (arith_decode(cinfo, st + 2)) {
	  if (*thiscoef < 0)
	    *thiscoef += m1;
	  else
	    *thiscoef += p1;
	}
	break;
      }
      if (arith_decode(cinfo, st + 1)) {	/* newly nonzero coef */
	entropy->ac_stats[tbl][245] = 0;
	if (arith_decode(cinfo, entropy->ac_stats[tbl] + 245))
	  *thiscoef = m1;
	else
	  *thiscoef = p1;
	break;
      }
      st += 3; k++;
      if (k > cinfo->Se) {
	WARNMS(cinfo, JWRN_ARITH_BAD_CODE);
	entropy->ct = -1;			/* spectral overflow */
	return TRUE;
      }
    }
  }

  return TRUE;
}


/*
 * Decode one MCU's worth of arithmetic-compressed coefficients.
 */

METHODDEF(boolean)
decode_mcu (j_decompress_ptr cinfo, JBLOCKROW *MCU_data)
{
  arith_entropy_ptr entropy = (arith_entropy_ptr) cinfo->entropy;
  jpeg_component_info * compptr;
  JBLOCKROW block;
  unsigned char *st;
  int blkn, ci, tbl, sign, k;
  int v, m;

  /* Process restart marker if needed */
  if (cinfo->restart_interval) {
    if (entropy->restarts_to_go == 0)
      process_restart(cinfo);
    entropy->restarts_to_go--;
  }

  if (entropy->ct == -1) return TRUE;	/* if error do nothing */

  /* Outer loop handles each block in the MCU */

  for (blkn = 0; blkn < cinfo->blocks_in_MCU; blkn++) {
    block = MCU_data[blkn];
    ci = cinfo->MCU_membership[blkn];
    compptr = cinfo->cur_comp_info[ci];

    /* Sections F.2.4.1 & F.1.4.4.1: Decoding of DC coefficients */

    tbl = compptr->dc_tbl_no;

    /* Table F.4: Point to statistics bin S0 for DC coefficient coding */
    st = entropy->dc_stats[tbl] + entropy->dc_context[ci];

    /* Figure F.19: Decode_DC_DIFF */
    if (arith_decode(cinfo, st) == 0)
      entropy->dc_context[ci] = 0;
    else {
      /* Figure F.21: Decoding nonzero value v */
      /* Figure F.22: Decoding the sign of v */
      sign = arith_decode(cinfo, st + 1);
      st += 2; st += sign;
      /* Figure F.23: Decoding the magnitude category of v */
      if ((m = arith_decode(cinfo, st)) != 0) {
	st = entropy->dc_stats[tbl] + 20;	/* Table F.4: X1 = 20 */
	while (arith_decode(cinfo, st)) {
	  if ((m <<= 1) == 0x8000) {
	    WARNMS(cinfo, JWRN_ARITH_BAD_CODE);
	    entropy->ct = -1;			/* magnitude overflow */
	    return TRUE;
	  }
	  st += 1;
	}
      }
      /* Section F.1.4.4.1.2: Establish dc_context conditioning category */
      if (m < (int) (((INT32) 1 << cinfo->arith_dc_L[tbl]) >> 1))
	entropy->dc_context[ci] = 0;		   /* zero diff category */
      else if (m > (int) (((INT32) 1 << cinfo->arith_dc_U[tbl]) >> 1))
	entropy->dc_context[ci] = 12 + (sign * 4); /* large diff category */
      else
	entropy->dc_context[ci] = 4 + (sign * 4);  /* small diff category */
      v = m;
      /* Figure F.24: Decoding the magnitude bit pattern of v */
      st += 14;
      while (m >>= 1)
	if (arith_decode(cinfo, st)) v |= m;
      v += 1; if (sign) v = -v;
      entropy->last_dc_val[ci] += v;
    }

    (*block)[0] = (JCOEF) entropy->last_dc_val[ci];

    /* Sections F.2.4.2 & F.1.4.4.2: Decoding of AC coefficients */

    tbl = compptr->ac_tbl_no;

    /* Figure F.20: Decode_AC_coefficients */
    for (k = 1; k < DCTSIZE2; k++) {
      st = entropy->ac_stats[tbl] + 3 * (k - 1);
      if (arith_decode(cinfo, st)) break;	/* EOB flag */
      while (arith_decode(cinfo, st + 1) == 0) {
	st += 3; k++;
	if (k >= DCTSIZE2) {
	  WARNMS(cinfo, JWRN_ARITH_BAD_CODE);
	  entropy->ct = -1;			/* spectral overflow */
	  return TRUE;
	}
      }
      /* Figure F.21: Decoding nonzero value v */
      /* Figure F.22: Decoding the sign of v */
      entropy->ac_stats[tbl][245] = 0;
      sign = arith_decode(cinfo, entropy->ac_stats[tbl] + 245);
      st += 2;
      /* Figure F.23: Decoding the magnitude category of v */
      if ((m = arith_decode(cinfo, st)) != 0) {
	if (arith_decode(cinfo, st)) {
	  m <<= 1;
	  st = entropy->ac_stats[tbl] +
	       (k <= cinfo->arith_ac_K[tbl] ? 189 : 217);
	  while (arith_decode(cinfo, st)) {
	    if ((m <<= 1) == 0x8000) {
	      WARNMS(cinfo, JWRN_ARITH_BAD_CODE);
	      entropy->ct = -1;			/* magnitude overflow */
	      return TRUE;
	    }
	    st += 1;
	  }
	}
      }
      v = m;
      /* Figure F.24: Decoding the magnitude bit pattern of v */
      st += 14;
      while (m >>= 1)
	if (arith_decode(cinfo, st)) v |= m;
      v += 1; if (sign) v = -v;
      (*block)[jpeg_natural_order[k]] = (JCOEF) v;
    }
  }

  return TRUE;
}


/*
 * Initialize for an arithmetic-compressed scan.
 */

METHODDEF(void)
start_pass (j_decompress_ptr cinfo)
{
  arith_entropy_ptr entropy = (arith_entropy_ptr) cinfo->entropy;
  int ci, tbl;
  jpeg_component_info * compptr;

  if (cinfo->progressive_mode) {
    /* Validate progressive scan parameters */
    if (cinfo->Ss == 0) {
      if (cinfo->Se != 0)
	goto bad;
    } else {
      /* need not check Ss/Se < 0 since they came from unsigned bytes */
      if (cinfo->Se < cinfo->Ss || cinfo->Se >= DCTSIZE2)
	goto bad;
      /* AC scans may have only one component */
      if (cinfo->comps_in_scan != 1)
	goto bad;
    }
    if (cinfo->Ah != 0) {
      /* Successive approximation refinement scan: must have Al = Ah-1. */
      if (cinfo->Ah-1 != cinfo->Al)
	goto bad;
    }
    if (cinfo->Al > 13) {	/* need not check for < 0 */
      bad:
      ERREXIT4(cinfo, JERR_BAD_PROGRESSION,
	       cinfo->Ss, cinfo->Se, cinfo->Ah, cinfo->Al);
    }
    /* Update progression status, and verify that scan order is legal.
     * Note that inter-scan inconsistencies are treated as warnings
     * not fatal errors ... not clear if this is right way to behave.
     */
    for (ci = 0; ci < cinfo->comps_in_scan; ci++) {
      int coefi, cindex = cinfo->cur_comp_info[ci]->component_index;
      int *coef_bit_ptr = & cinfo->coef_bits[cindex][0];
      if (cinfo->Ss && coef_bit_ptr[0] < 0) /* AC without prior DC scan */
	WARNMS2(cinfo, JWRN_BOGUS_PROGRESSION, cindex, 0);
      for (coefi = cinfo->Ss; coefi <= cinfo->Se; coefi++) {
	int expected = (coef_bit_ptr[coefi] < 0) ? 0 : coef_bit_ptr[coefi];
	if (cinfo->Ah != expected)
	  WARNMS2(cinfo, JWRN_BOGUS_PROGRESSION, cindex, coefi);
	coef_bit_ptr[coefi] = cinfo->Al;
      }
    }
    /* Select MCU decoding routine */
    if (cinfo->Ah == 0) {
      if (cinfo->Ss == 0)
	entropy->pub.decode_mcu = decode_mcu_DC_first;
      else
	entropy->pub.decode_mcu = decode_mcu_AC_first;
    } else {
      if (cinfo->Ss == 0)
	entropy->pub.decode_mcu = decode_mcu_DC_refine;
      else
	entropy->pub.decode_mcu = decode_mcu_AC_refine;
    }
  } else {
    /* Check that the scan parameters Ss, Se, Ah/Al are OK for sequential JPEG.
     * This ought to be an error condition, but we make it a warning because
     * there are some baseline files out there with all zeroes in these bytes.
     */
    if (cinfo->Ss != 0 || cinfo->Se != DCTSIZE2-1 ||
	cinfo->Ah != 0 || cinfo->Al != 0)
      WARNMS(cinfo, JWRN_NOT_SEQUENTIAL);
    /* Select MCU decoding routine */
    entropy->pub.decode_mcu = decode_mcu;
  }

  for (ci = 0; ci < cinfo->comps_in_scan; ci++) {
    compptr = cinfo->cur_comp_info[ci];
    /* Allocate & initialize requested statistics areas */
    if (cinfo->progressive_mode == 0 || (cinfo->Ss == 0 && cinfo->Ah == 0)) {
      tbl = compptr->dc_tbl_no;
      if (tbl < 0 || tbl >= NUM_ARITH_TBLS)
	ERREXIT1(cinfo, JERR_NO_ARITH_TABLE, tbl);
      if (entropy->dc_stats[tbl] == NULL)
	entropy->dc_stats[tbl] = (unsigned char *) (*cinfo->mem->alloc_small)
	  ((j_common_ptr) cinfo, JPOOL_IMAGE, DC_STAT_BINS);
      MEMZERO(entropy->dc_stats[tbl], DC_STAT_BINS);
      /* Initialize DC predictions to 0 */
      entropy->last_dc_val[ci] = 0;
      entropy->dc_context[ci] = 0;
    }
    if (cinfo->progressive_mode == 0 || cinfo->Ss) {
      tbl = compptr->ac_tbl_no;
      if (tbl < 0 || tbl >= NUM_ARITH_TBLS)
	ERREXIT1(cinfo, JERR_NO_ARITH_TABLE, tbl);
      if (entropy->ac_stats[tbl] == NULL)
	entropy->ac_stats[tbl] = (unsigned char *) (*cinfo->mem->alloc_small)
	  ((j_common_ptr) cinfo, JPOOL_IMAGE, AC_STAT_BINS);
      MEMZERO(entropy->ac_stats[tbl], AC_STAT_BINS);
    }
  }

  /* Initialize arithmetic decoding variables */
  entropy->c = 0;
  entropy->a = 0;
  entropy->ct = -16;	/* force reading 2 initial bytes to fill C */

  /* Initialize restart counter */
  entropy->restarts_to_go = cinfo->restart_interval;
}


/*
 * Module initialization routine for arithmetic entropy decoding.
 */

GLOBAL(void)
jinit_arith_decoder (j_decompress_ptr cinfo)
{
  arith_entropy_ptr entropy;
  int i;

  entropy = (arith_entropy_ptr)
    (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
				SIZEOF(arith_entropy_decoder));
  cinfo->entropy = (struct jpeg_entropy_decoder *) entropy;
  entropy->pub.start_pass = start_pass;

  /* Mark tables unallocated */
  for (i = 0; i < NUM_ARITH_TBLS; i++) {
    entropy->dc_stats[i] = NULL;
    entropy->ac_stats[i] = NULL;
  }

  if (cinfo->progressive_mode) {
    /* Create progression status table */
    int *coef_bit_ptr, ci;
    cinfo->coef_bits = (int (*)[DCTSIZE2])
      (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
				  cinfo->num_components*DCTSIZE2*SIZEOF(int));
    coef_bit_ptr = & cinfo->coef_bits[0][0];
    for (ci = 0; ci < cinfo->num_components; ci++) 
      for (i = 0; i < DCTSIZE2; i++)
	*coef_bit_ptr++ = -1;
  }
}
