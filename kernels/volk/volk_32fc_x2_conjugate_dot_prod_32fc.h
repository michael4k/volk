/* -*- c++ -*- */
/*
 * Copyright 2012, 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

/*!
 * \page volk_32fc_x2_conjugate_dot_prod_32fc
 *
 * \b Overview
 *
 * This block computes the conjugate dot product (or inner product)
 * between two vectors, the \p input and \p taps vectors. Given a set
 * of \p num_points taps, the result is the sum of products between
 * the input vector and the conjugate of the taps. The result is a
 * single value stored in the \p result address and is returned as a
 * complex float.
 *
 * <b>Dispatcher Prototype</b>
 * \code
 * void volk_32fc_x2_conjugate_dot_prod_32fc(lv_32fc_t* result, const lv_32fc_t* input, const lv_32fc_t* taps, unsigned int num_points)
 * \endcode
 *
 * \b Inputs
 * \li input: vector of complex floats.
 * \li taps:  complex float taps.
 * \li num_points: number of samples in both \p input and \p taps.
 *
 * \b Outputs
 * \li result: pointer to a complex float value to hold the dot product result.
 *
 * \b Example
 * \code
 * int N = 10000;
 *
 * <FIXME>
 *
 * volk_32fc_x2_conjugate_dot_prod_32fc();
 *
 * \endcode
 */

#ifndef INCLUDED_volk_32fc_x2_conjugate_dot_prod_32fc_u_H
#define INCLUDED_volk_32fc_x2_conjugate_dot_prod_32fc_u_H


#include<volk/volk_complex.h>


#ifdef LV_HAVE_GENERIC

static inline void volk_32fc_x2_conjugate_dot_prod_32fc_generic(lv_32fc_t* result, const lv_32fc_t* input, const lv_32fc_t* taps, unsigned int num_points) {

  const unsigned int num_bytes = num_points*8;

  float * res = (float*) result;
  float * in = (float*) input;
  float * tp = (float*) taps;
  unsigned int n_2_ccomplex_blocks = num_bytes >> 4;
  unsigned int isodd = (num_bytes >> 3) &1;

  float sum0[2] = {0,0};
  float sum1[2] = {0,0};
  unsigned int i = 0;

  for(i = 0; i < n_2_ccomplex_blocks; ++i) {
    sum0[0] += in[0] * tp[0] + in[1] * tp[1];
    sum0[1] += (-in[0] * tp[1]) + in[1] * tp[0];
    sum1[0] += in[2] * tp[2] + in[3] * tp[3];
    sum1[1] += (-in[2] * tp[3]) + in[3] * tp[2];

    in += 4;
    tp += 4;
  }

  res[0] = sum0[0] + sum1[0];
  res[1] = sum0[1] + sum1[1];

  for(i = 0; i < isodd; ++i) {
    *result += input[(num_bytes >> 3) - 1] * lv_conj(taps[(num_bytes >> 3) - 1]);
  }
}

#endif /*LV_HAVE_GENERIC*/

#ifdef LV_HAVE_AVX
#include <immintrin.h>
#include "volk/volk_avx_intrinsics.h"
static inline void
volk_32fc_x2_conjugate_dot_prod_32fc_u_avx(lv_32fc_t* result, const lv_32fc_t* input, const lv_32fc_t* taps, unsigned int num_points) {

  int quarter_points = num_points / 4;
  __m256 avec, bvec, resultvec, sumvec;
  sumvec = _mm256_set1_ps(0.f);
  const float *a_p = (const float*) input;
  const float *b_p = (const float*) taps;

  for (int qpoint = 0; qpoint < quarter_points; ++qpoint) {
    avec = _mm256_loadu_ps(a_p);
    bvec = _mm256_loadu_ps(b_p);
    resultvec = _mm256_complexconjugatemul_ps(avec, bvec);
    sumvec = _mm256_add_ps(sumvec, resultvec);

    a_p += 8;
    b_p += 8;
  }

  __VOLK_ATTR_ALIGNED(32) lv_32fc_t tmp_result[4];
  _mm256_store_ps((float*)tmp_result, sumvec);
  *result = tmp_result[0] + tmp_result[1];
  *result += tmp_result[2] + tmp_result[3];

  for (int point=quarter_points*4; point < num_points; ++point) {
    float a_r = *a_p++;
    float a_i = *a_p++;
    float b_r = *a_p++;
    float b_i = *b_p++;
    *result += lv_cmake(a_r*b_r + a_i*b_i, a_r*-b_i + a_i*b_r);
  }
}
#endif /* LV_HAVE_AVX */

#ifdef LV_HAVE_AVX
#include <immintrin.h>
#include "volk/volk_avx_intrinsics.h"
static inline void
volk_32fc_x2_conjugate_dot_prod_32fc_a_avx(lv_32fc_t* result, const lv_32fc_t* input, const lv_32fc_t* taps, unsigned int num_points) {

  int quarter_points = num_points / 4;
  __m256 avec, bvec, resultvec, sumvec;
  sumvec = _mm256_set1_ps(0.f);
  const float *a_p = (const float*) input;
  const float *b_p = (const float*) taps;

  for (int qpoint = 0; qpoint < quarter_points; ++qpoint) {
    avec = _mm256_load_ps(a_p);
    bvec = _mm256_load_ps(b_p);
    resultvec = _mm256_complexconjugatemul_ps(avec, bvec);
    sumvec = _mm256_add_ps(sumvec, resultvec);

    a_p += 8;
    b_p += 8;
  }

  __VOLK_ATTR_ALIGNED(32) lv_32fc_t tmp_result[4];
  _mm256_store_ps((float*)tmp_result, sumvec);
  *result = tmp_result[0] + tmp_result[1];
  *result += tmp_result[2] + tmp_result[3];

  for (int point=quarter_points*4; point < num_points; ++point) {
    float a_r = *a_p++;
    float a_i = *a_p++;
    float b_r = *a_p++;
    float b_i = *b_p++;
    *result += lv_cmake(a_r*b_r + a_i*b_i, a_r*-b_i + a_i*b_r);
  }
}
#endif /* LV_HAVE_AVX */

#ifdef LV_HAVE_SSE3

#include <xmmintrin.h>
#include <pmmintrin.h>
#include <mmintrin.h>

static inline void volk_32fc_x2_conjugate_dot_prod_32fc_u_sse3(lv_32fc_t* result, const lv_32fc_t* input, const lv_32fc_t* taps, unsigned int num_points) {

  unsigned int num_bytes = num_points*8;

  // Variable never used?
  //__VOLK_ATTR_ALIGNED(16) static const uint32_t conjugator[4]= {0x00000000, 0x80000000, 0x00000000, 0x80000000};

  union HalfMask {
    uint32_t intRep[4];
    __m128 vec;
    } halfMask;

  union NegMask {
    int intRep[4];
    __m128 vec;
  } negMask;

  unsigned int offset = 0;
  float Rsum=0, Isum=0;
  float Im,Re;

  __m128 in1, in2, Rv, fehg, Iv, Rs, Ivm, Is;
  __m128 zv = {0,0,0,0};

  halfMask.intRep[0] = halfMask.intRep[1] = 0xFFFFFFFF;
  halfMask.intRep[2] = halfMask.intRep[3] = 0x00000000;

  negMask.intRep[0] = negMask.intRep[2] = 0x80000000;
  negMask.intRep[1] = negMask.intRep[3] = 0;

  // main loop
  while(num_bytes >= 4*sizeof(float)){

    in1 = _mm_loadu_ps( (float*) (input+offset) );
    in2 = _mm_loadu_ps( (float*) (taps+offset) );
    Rv = _mm_mul_ps(in1, in2);
    fehg = _mm_shuffle_ps(in2, in2, _MM_SHUFFLE(2,3,0,1));
    Iv = _mm_mul_ps(in1, fehg);
    Rs = _mm_hadd_ps( _mm_hadd_ps(Rv, zv) ,zv);
    Ivm = _mm_xor_ps( negMask.vec, Iv );
    Is = _mm_hadd_ps( _mm_hadd_ps(Ivm, zv) ,zv);
    _mm_store_ss( &Im, Is );
    _mm_store_ss( &Re, Rs );
    num_bytes -= 4*sizeof(float);
    offset += 2;
    Rsum += Re;
    Isum += Im;
  }

  // handle the last complex case ...
  if(num_bytes > 0){

    if(num_bytes != 4){
      // bad things are happening
    }

    in1 = _mm_loadu_ps( (float*) (input+offset) );
    in2 = _mm_loadu_ps( (float*) (taps+offset) );
    Rv = _mm_and_ps(_mm_mul_ps(in1, in2), halfMask.vec);
    fehg = _mm_shuffle_ps(in2, in2, _MM_SHUFFLE(2,3,0,1));
    Iv = _mm_and_ps(_mm_mul_ps(in1, fehg), halfMask.vec);
    Rs = _mm_hadd_ps(_mm_hadd_ps(Rv, zv),zv);
    Ivm = _mm_xor_ps( negMask.vec, Iv );
    Is = _mm_hadd_ps(_mm_hadd_ps(Ivm, zv),zv);
    _mm_store_ss( &Im, Is );
    _mm_store_ss( &Re, Rs );
    Rsum += Re;
    Isum += Im;
  }

  result[0] = lv_cmake(Rsum,Isum);
  return;
}

#endif /*LV_HAVE_SSE3*/

#ifdef LV_HAVE_NEON
#include <arm_neon.h>
static inline void volk_32fc_x2_conjugate_dot_prod_32fc_neon(lv_32fc_t* result, const lv_32fc_t* input, const lv_32fc_t* taps, unsigned int num_points) {

    unsigned int quarter_points = num_points / 4;
    unsigned int number;

    lv_32fc_t* a_ptr = (lv_32fc_t*) taps;
    lv_32fc_t* b_ptr = (lv_32fc_t*) input;
    // for 2-lane vectors, 1st lane holds the real part,
    // 2nd lane holds the imaginary part
    float32x4x2_t a_val, b_val, accumulator;
    float32x4x2_t tmp_imag;
    accumulator.val[0] = vdupq_n_f32(0);
    accumulator.val[1] = vdupq_n_f32(0);

    for(number = 0; number < quarter_points; ++number) {
        a_val = vld2q_f32((float*)a_ptr); // a0r|a1r|a2r|a3r || a0i|a1i|a2i|a3i
        b_val = vld2q_f32((float*)b_ptr); // b0r|b1r|b2r|b3r || b0i|b1i|b2i|b3i
        __VOLK_PREFETCH(a_ptr+8);
        __VOLK_PREFETCH(b_ptr+8);

        // do the first multiply
        tmp_imag.val[1] = vmulq_f32(a_val.val[1], b_val.val[0]);
        tmp_imag.val[0] = vmulq_f32(a_val.val[0], b_val.val[0]);

        // use multiply accumulate/subtract to get result
        tmp_imag.val[1] = vmlsq_f32(tmp_imag.val[1], a_val.val[0], b_val.val[1]);
        tmp_imag.val[0] = vmlaq_f32(tmp_imag.val[0], a_val.val[1], b_val.val[1]);

        accumulator.val[0] = vaddq_f32(accumulator.val[0], tmp_imag.val[0]);
        accumulator.val[1] = vaddq_f32(accumulator.val[1], tmp_imag.val[1]);

        // increment pointers
        a_ptr += 4;
        b_ptr += 4;
    }
    lv_32fc_t accum_result[4];
    vst2q_f32((float*)accum_result, accumulator);
    *result = accum_result[0] + accum_result[1] + accum_result[2] + accum_result[3];

    // tail case
    for(number = quarter_points*4; number < num_points; ++number) {
      *result += (*a_ptr++) * lv_conj(*b_ptr++);
    }
    *result = lv_conj(*result);

}
#endif /*LV_HAVE_NEON*/

#endif /*INCLUDED_volk_32fc_x2_conjugate_dot_prod_32fc_u_H*/

#ifndef INCLUDED_volk_32fc_x2_conjugate_dot_prod_32fc_a_H
#define INCLUDED_volk_32fc_x2_conjugate_dot_prod_32fc_a_H

#include <volk/volk_common.h>
#include<volk/volk_complex.h>
#include<stdio.h>


#ifdef LV_HAVE_GENERIC


static inline void volk_32fc_x2_conjugate_dot_prod_32fc_a_generic(lv_32fc_t* result, const lv_32fc_t* input, const lv_32fc_t* taps, unsigned int num_points) {

  const unsigned int num_bytes = num_points*8;

  float * res = (float*) result;
  float * in = (float*) input;
  float * tp = (float*) taps;
  unsigned int n_2_ccomplex_blocks = num_bytes >> 4;
  unsigned int isodd = (num_bytes >> 3) &1;

  float sum0[2] = {0,0};
  float sum1[2] = {0,0};
  unsigned int i = 0;

  for(i = 0; i < n_2_ccomplex_blocks; ++i) {
    sum0[0] += in[0] * tp[0] + in[1] * tp[1];
    sum0[1] += (-in[0] * tp[1]) + in[1] * tp[0];
    sum1[0] += in[2] * tp[2] + in[3] * tp[3];
    sum1[1] += (-in[2] * tp[3]) + in[3] * tp[2];

    in += 4;
    tp += 4;
  }

  res[0] = sum0[0] + sum1[0];
  res[1] = sum0[1] + sum1[1];

  for(i = 0; i < isodd; ++i) {
    *result += input[(num_bytes >> 3) - 1] * lv_conj(taps[(num_bytes >> 3) - 1]);
  }
}

#endif /*LV_HAVE_GENERIC*/


#if LV_HAVE_SSE && LV_HAVE_64

static inline void volk_32fc_x2_conjugate_dot_prod_32fc_a_sse(lv_32fc_t* result, const lv_32fc_t* input, const lv_32fc_t* taps, unsigned int num_points) {

  const unsigned int num_bytes = num_points*8;

  __VOLK_ATTR_ALIGNED(16) static const uint32_t conjugator[4]= {0x00000000, 0x80000000, 0x00000000, 0x80000000};

  __VOLK_ASM __VOLK_VOLATILE
    (
     "#  ccomplex_conjugate_dotprod_generic (float* result, const float *input,\n\t"
     "#                         const float *taps, unsigned num_bytes)\n\t"
     "#    float sum0 = 0;\n\t"
     "#    float sum1 = 0;\n\t"
     "#    float sum2 = 0;\n\t"
     "#    float sum3 = 0;\n\t"
     "#    do {\n\t"
     "#      sum0 += input[0] * taps[0] - input[1] * taps[1];\n\t"
     "#      sum1 += input[0] * taps[1] + input[1] * taps[0];\n\t"
     "#      sum2 += input[2] * taps[2] - input[3] * taps[3];\n\t"
     "#      sum3 += input[2] * taps[3] + input[3] * taps[2];\n\t"
     "#      input += 4;\n\t"
     "#      taps += 4;  \n\t"
     "#    } while (--n_2_ccomplex_blocks != 0);\n\t"
     "#    result[0] = sum0 + sum2;\n\t"
     "#    result[1] = sum1 + sum3;\n\t"
     "# TODO: prefetch and better scheduling\n\t"
     "  xor    %%r9,  %%r9\n\t"
     "  xor    %%r10, %%r10\n\t"
     "  movq   %[conjugator], %%r9\n\t"
     "  movq   %%rcx, %%rax\n\t"
     "  movaps 0(%%r9), %%xmm8\n\t"
     "  movq   %%rcx, %%r8\n\t"
     "  movq   %[rsi],  %%r9\n\t"
     "  movq   %[rdx], %%r10\n\t"
     "	xorps	%%xmm6, %%xmm6		# zero accumulators\n\t"
     "	movaps	0(%%r9), %%xmm0\n\t"
     "	xorps	%%xmm7, %%xmm7		# zero accumulators\n\t"
     "	movups	0(%%r10), %%xmm2\n\t"
     "	shr	$5, %%rax		# rax = n_2_ccomplex_blocks / 2\n\t"
     "  shr     $4, %%r8\n\t"
     "  xorps  %%xmm8, %%xmm2\n\t"
     "	jmp	.%=L1_test\n\t"
     "	# 4 taps / loop\n\t"
     "	# something like ?? cycles / loop\n\t"
     ".%=Loop1:	\n\t"
     "# complex prod: C += A * B,  w/ temp Z & Y (or B), xmmPN=$0x8000000080000000\n\t"
     "#	movaps	(%%r9), %%xmmA\n\t"
     "#	movaps	(%%r10), %%xmmB\n\t"
     "#	movaps	%%xmmA, %%xmmZ\n\t"
     "#	shufps	$0xb1, %%xmmZ, %%xmmZ	# swap internals\n\t"
     "#	mulps	%%xmmB, %%xmmA\n\t"
     "#	mulps	%%xmmZ, %%xmmB\n\t"
     "#	# SSE replacement for: pfpnacc %%xmmB, %%xmmA\n\t"
     "#	xorps	%%xmmPN, %%xmmA\n\t"
     "#	movaps	%%xmmA, %%xmmZ\n\t"
     "#	unpcklps %%xmmB, %%xmmA\n\t"
     "#	unpckhps %%xmmB, %%xmmZ\n\t"
     "#	movaps	%%xmmZ, %%xmmY\n\t"
     "#	shufps	$0x44, %%xmmA, %%xmmZ	# b01000100\n\t"
     "#	shufps	$0xee, %%xmmY, %%xmmA	# b11101110\n\t"
     "#	addps	%%xmmZ, %%xmmA\n\t"
     "#	addps	%%xmmA, %%xmmC\n\t"
     "# A=xmm0, B=xmm2, Z=xmm4\n\t"
     "# A'=xmm1, B'=xmm3, Z'=xmm5\n\t"
     "	movaps	16(%%r9), %%xmm1\n\t"
     "	movaps	%%xmm0, %%xmm4\n\t"
     "	mulps	%%xmm2, %%xmm0\n\t"
     "	shufps	$0xb1, %%xmm4, %%xmm4	# swap internals\n\t"
     "	movaps	16(%%r10), %%xmm3\n\t"
     "	movaps	%%xmm1, %%xmm5\n\t"
     "  xorps   %%xmm8, %%xmm3\n\t"
     "	addps	%%xmm0, %%xmm6\n\t"
     "	mulps	%%xmm3, %%xmm1\n\t"
     "	shufps	$0xb1, %%xmm5, %%xmm5	# swap internals\n\t"
     "	addps	%%xmm1, %%xmm6\n\t"
     "	mulps	%%xmm4, %%xmm2\n\t"
     "	movaps	32(%%r9), %%xmm0\n\t"
     "	addps	%%xmm2, %%xmm7\n\t"
     "	mulps	%%xmm5, %%xmm3\n\t"
     "	add	$32, %%r9\n\t"
     "	movaps	32(%%r10), %%xmm2\n\t"
     "	addps	%%xmm3, %%xmm7\n\t"
     "	add	$32, %%r10\n\t"
     "  xorps   %%xmm8, %%xmm2\n\t"
     ".%=L1_test:\n\t"
     "	dec	%%rax\n\t"
     "	jge	.%=Loop1\n\t"
     "	# We've handled the bulk of multiplies up to here.\n\t"
     "	# Let's sse if original n_2_ccomplex_blocks was odd.\n\t"
     "	# If so, we've got 2 more taps to do.\n\t"
     "	and	$1, %%r8\n\t"
     "	je	.%=Leven\n\t"
     "	# The count was odd, do 2 more taps.\n\t"
     "	# Note that we've already got mm0/mm2 preloaded\n\t"
     "	# from the main loop.\n\t"
     "	movaps	%%xmm0, %%xmm4\n\t"
     "	mulps	%%xmm2, %%xmm0\n\t"
     "	shufps	$0xb1, %%xmm4, %%xmm4	# swap internals\n\t"
     "	addps	%%xmm0, %%xmm6\n\t"
     "	mulps	%%xmm4, %%xmm2\n\t"
     "	addps	%%xmm2, %%xmm7\n\t"
     ".%=Leven:\n\t"
     "	# neg inversor\n\t"
     "	xorps	%%xmm1, %%xmm1\n\t"
     "	mov	$0x80000000, %%r9\n\t"
     "	movd	%%r9, %%xmm1\n\t"
     "	shufps	$0x11, %%xmm1, %%xmm1	# b00010001 # 0 -0 0 -0\n\t"
     "	# pfpnacc\n\t"
     "	xorps	%%xmm1, %%xmm6\n\t"
     "	movaps	%%xmm6, %%xmm2\n\t"
     "	unpcklps %%xmm7, %%xmm6\n\t"
     "	unpckhps %%xmm7, %%xmm2\n\t"
     "	movaps	%%xmm2, %%xmm3\n\t"
     "	shufps	$0x44, %%xmm6, %%xmm2	# b01000100\n\t"
     "	shufps	$0xee, %%xmm3, %%xmm6	# b11101110\n\t"
     "	addps	%%xmm2, %%xmm6\n\t"
     "					# xmm6 = r1 i2 r3 i4\n\t"
     "	movhlps	%%xmm6, %%xmm4		# xmm4 = r3 i4 ?? ??\n\t"
     "	addps	%%xmm4, %%xmm6		# xmm6 = r1+r3 i2+i4 ?? ??\n\t"
     "	movlps	%%xmm6, (%[rdi])		# store low 2x32 bits (complex) to memory\n\t"
     :
     :[rsi] "r" (input), [rdx] "r" (taps), "c" (num_bytes), [rdi] "r" (result), [conjugator] "r" (conjugator)
     :"rax", "r8", "r9", "r10"
     );

  int getem = num_bytes % 16;

  for(; getem > 0; getem -= 8) {
    *result += (input[(num_bytes >> 3) - 1] * lv_conj(taps[(num_bytes >> 3) - 1]));
  }
}
#endif

#if LV_HAVE_SSE && LV_HAVE_32
static inline void volk_32fc_x2_conjugate_dot_prod_32fc_a_sse_32(lv_32fc_t* result, const lv_32fc_t* input, const lv_32fc_t* taps, unsigned int num_points) {

  const unsigned int num_bytes = num_points*8;

  __VOLK_ATTR_ALIGNED(16) static const uint32_t conjugator[4]= {0x00000000, 0x80000000, 0x00000000, 0x80000000};

  int bound = num_bytes >> 4;
  int leftovers = num_bytes % 16;

  __VOLK_ASM __VOLK_VOLATILE
    (
     "	#pushl	%%ebp\n\t"
     "	#movl	%%esp, %%ebp\n\t"
     "	#movl	12(%%ebp), %%eax		# input\n\t"
     "	#movl	16(%%ebp), %%edx		# taps\n\t"
     "	#movl	20(%%ebp), %%ecx                # n_bytes\n\t"
     "  movaps  0(%[conjugator]), %%xmm1\n\t"
     "	xorps	%%xmm6, %%xmm6		# zero accumulators\n\t"
     "	movaps	0(%[eax]), %%xmm0\n\t"
     "	xorps	%%xmm7, %%xmm7		# zero accumulators\n\t"
     "	movaps	0(%[edx]), %%xmm2\n\t"
     "  movl    %[ecx], (%[out])\n\t"
     "	shrl	$5, %[ecx]		# ecx = n_2_ccomplex_blocks / 2\n\t"

     "  xorps   %%xmm1, %%xmm2\n\t"
     "	jmp	.%=L1_test\n\t"
     "	# 4 taps / loop\n\t"
     "	# something like ?? cycles / loop\n\t"
     ".%=Loop1:	\n\t"
     "# complex prod: C += A * B,  w/ temp Z & Y (or B), xmmPN=$0x8000000080000000\n\t"
     "#	movaps	(%[eax]), %%xmmA\n\t"
     "#	movaps	(%[edx]), %%xmmB\n\t"
     "#	movaps	%%xmmA, %%xmmZ\n\t"
     "#	shufps	$0xb1, %%xmmZ, %%xmmZ	# swap internals\n\t"
     "#	mulps	%%xmmB, %%xmmA\n\t"
     "#	mulps	%%xmmZ, %%xmmB\n\t"
     "#	# SSE replacement for: pfpnacc %%xmmB, %%xmmA\n\t"
     "#	xorps	%%xmmPN, %%xmmA\n\t"
     "#	movaps	%%xmmA, %%xmmZ\n\t"
     "#	unpcklps %%xmmB, %%xmmA\n\t"
     "#	unpckhps %%xmmB, %%xmmZ\n\t"
     "#	movaps	%%xmmZ, %%xmmY\n\t"
     "#	shufps	$0x44, %%xmmA, %%xmmZ	# b01000100\n\t"
     "#	shufps	$0xee, %%xmmY, %%xmmA	# b11101110\n\t"
     "#	addps	%%xmmZ, %%xmmA\n\t"
     "#	addps	%%xmmA, %%xmmC\n\t"
     "# A=xmm0, B=xmm2, Z=xmm4\n\t"
     "# A'=xmm1, B'=xmm3, Z'=xmm5\n\t"
     "	movaps	16(%[edx]), %%xmm3\n\t"
     "	movaps	%%xmm0, %%xmm4\n\t"
     "  xorps   %%xmm1, %%xmm3\n\t"
     "	mulps	%%xmm2, %%xmm0\n\t"
     "	movaps	16(%[eax]), %%xmm1\n\t"
     "	shufps	$0xb1, %%xmm4, %%xmm4	# swap internals\n\t"
     "	movaps	%%xmm1, %%xmm5\n\t"
     "	addps	%%xmm0, %%xmm6\n\t"
     "	mulps	%%xmm3, %%xmm1\n\t"
     "	shufps	$0xb1, %%xmm5, %%xmm5	# swap internals\n\t"
     "	addps	%%xmm1, %%xmm6\n\t"
     "  movaps  0(%[conjugator]), %%xmm1\n\t"
     "	mulps	%%xmm4, %%xmm2\n\t"
     "	movaps	32(%[eax]), %%xmm0\n\t"
     "	addps	%%xmm2, %%xmm7\n\t"
     "	mulps	%%xmm5, %%xmm3\n\t"
     "	addl	$32, %[eax]\n\t"
     "	movaps	32(%[edx]), %%xmm2\n\t"
     "	addps	%%xmm3, %%xmm7\n\t"
     "  xorps   %%xmm1, %%xmm2\n\t"
     "	addl	$32, %[edx]\n\t"
     ".%=L1_test:\n\t"
     "	decl	%[ecx]\n\t"
     "	jge	.%=Loop1\n\t"
     "	# We've handled the bulk of multiplies up to here.\n\t"
     "	# Let's sse if original n_2_ccomplex_blocks was odd.\n\t"
     "	# If so, we've got 2 more taps to do.\n\t"
     "	movl	0(%[out]), %[ecx]		# n_2_ccomplex_blocks\n\t"
     "  shrl    $4, %[ecx]\n\t"
     "	andl	$1, %[ecx]\n\t"
     "	je	.%=Leven\n\t"
     "	# The count was odd, do 2 more taps.\n\t"
     "	# Note that we've already got mm0/mm2 preloaded\n\t"
     "	# from the main loop.\n\t"
     "	movaps	%%xmm0, %%xmm4\n\t"
     "	mulps	%%xmm2, %%xmm0\n\t"
     "	shufps	$0xb1, %%xmm4, %%xmm4	# swap internals\n\t"
     "	addps	%%xmm0, %%xmm6\n\t"
     "	mulps	%%xmm4, %%xmm2\n\t"
     "	addps	%%xmm2, %%xmm7\n\t"
     ".%=Leven:\n\t"
     "	# neg inversor\n\t"
     "  #movl 8(%%ebp), %[eax] \n\t"
     "	xorps	%%xmm1, %%xmm1\n\t"
     "  movl	$0x80000000, (%[out])\n\t"
     "	movss	(%[out]), %%xmm1\n\t"
     "	shufps	$0x11, %%xmm1, %%xmm1	# b00010001 # 0 -0 0 -0\n\t"
     "	# pfpnacc\n\t"
     "	xorps	%%xmm1, %%xmm6\n\t"
     "	movaps	%%xmm6, %%xmm2\n\t"
     "	unpcklps %%xmm7, %%xmm6\n\t"
     "	unpckhps %%xmm7, %%xmm2\n\t"
     "	movaps	%%xmm2, %%xmm3\n\t"
     "	shufps	$0x44, %%xmm6, %%xmm2	# b01000100\n\t"
     "	shufps	$0xee, %%xmm3, %%xmm6	# b11101110\n\t"
     "	addps	%%xmm2, %%xmm6\n\t"
     "					# xmm6 = r1 i2 r3 i4\n\t"
     "	#movl	8(%%ebp), %[eax]		# @result\n\t"
     "	movhlps	%%xmm6, %%xmm4		# xmm4 = r3 i4 ?? ??\n\t"
     "	addps	%%xmm4, %%xmm6		# xmm6 = r1+r3 i2+i4 ?? ??\n\t"
     "	movlps	%%xmm6, (%[out])		# store low 2x32 bits (complex) to memory\n\t"
     "	#popl	%%ebp\n\t"
     :
     : [eax] "r" (input), [edx] "r" (taps), [ecx] "r" (num_bytes), [out] "r" (result), [conjugator] "r" (conjugator)
     );

  for(; leftovers > 0; leftovers -= 8) {
    *result += (input[(bound << 1)] * lv_conj(taps[(bound << 1)]));
  }
}
#endif /*LV_HAVE_SSE*/


#endif /*INCLUDED_volk_32fc_x2_conjugate_dot_prod_32fc_a_H*/
