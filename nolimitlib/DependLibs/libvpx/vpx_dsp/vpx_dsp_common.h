/*
 *  Copyright (c) 2015 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef VPX_VPX_DSP_VPX_DSP_COMMON_H_
#define VPX_VPX_DSP_VPX_DSP_COMMON_H_

#include <limits.h>

#include "./vpx_config.h"
#include "vpx/vpx_integer.h"
#include "vpx_ports/mem.h"

#ifdef __cplusplus
extern "C" {
#endif

#define VPXMIN(x, y) (((x) < (y)) ? (x) : (y))
#define VPXMAX(x, y) (((x) > (y)) ? (x) : (y))

#define VPX_SWAP(type, a, b) \
  do {                       \
    type c = (b);            \
    (b) = a;                 \
    (a) = c;                 \
  } while (0)

#if CONFIG_VP9_HIGHBITDEPTH
// Note:
// tran_low_t  is the datatype used for final transform coefficients.
// tran_high_t is the datatype used for intermediate transform stages.
typedef int64_t tran_high_t;
typedef int32_t tran_low_t;
#else
// Note:
// tran_low_t  is the datatype used for final transform coefficients.
// tran_high_t is the datatype used for intermediate transform stages.
typedef int32_t tran_high_t;
typedef int16_t tran_low_t;
#endif  // CONFIG_VP9_HIGHBITDEPTH

typedef int16_t tran_coef_t;

// Visual Studio 2022 (cl.exe) targeting AArch64 with optimizations enabled
// produces invalid code for clip_pixel() when the return type is uint8_t.
// See:
// https://developercommunity.visualstudio.com/t/Misoptimization-for-ARM64-in-VS-2022-17/10363361
// TODO(jzern): check the compiler version after a fix for the issue is
// released.
#if defined(_MSC_VER) && defined(_M_ARM64) && !defined(__clang__)
static INLINE int clip_pixel(int val) {
  return (val > 255) ? 255 : (val < 0) ? 0 : val;
}
#else
static INLINE uint8_t clip_pixel(int val) {
  return (val > 255) ? 255 : (val < 0) ? 0 : val;
}
#endif

static INLINE int clamp(int value, int low, int high) {
  return value < low ? low : (value > high ? high : value);
}

static INLINE double fclamp(double value, double low, double high) {
  return value < low ? low : (value > high ? high : value);
}

static INLINE int64_t lclamp(int64_t value, int64_t low, int64_t high) {
  return value < low ? low : (value > high ? high : value);
}

static INLINE uint16_t clip_pixel_highbd(int val, int bd) {
  switch (bd) {
    case 8:
    default: return (uint16_t)clamp(val, 0, 255);
    case 10: return (uint16_t)clamp(val, 0, 1023);
    case 12: return (uint16_t)clamp(val, 0, 4095);
  }
}

// Returns the saturating cast of a double value to int.
static INLINE int saturate_cast_double_to_int(double d) {
  if (d > INT_MAX) return INT_MAX;
  return (int)d;
}

#if defined(TARGET_OS_ANDROID)
// declarations so will compile android with Qt 6.7.3
void vpx_fdct4x4_neon(const int16_t *input, tran_low_t *final_output, int stride);
void vpx_fdct8x8_neon(const int16_t *input, tran_low_t *final_output, int stride);
void vpx_fdct16x16_neon(const int16_t *input, tran_low_t *output, int stride);
uint64_t vpx_sum_squares_2d_i16(const int16_t *src, int stride, int size);
#include "vpx_scale/yv12config.h"
void vpx_extend_frame_borders(struct yv12_buffer_config *ybf);
#include "vp9/common/vp9_filter.h"
void vp9_scale_and_extend_frame_c(const YV12_BUFFER_CONFIG *src,
                                     YV12_BUFFER_CONFIG *dst,
                                     INTERP_FILTER filter_type,
                                     int phase_scaler);

#endif // defined(TARGET_OS_ANDROID)

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // VPX_VPX_DSP_VPX_DSP_COMMON_H_
