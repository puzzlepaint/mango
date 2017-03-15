/*
    MANGO Multimedia Development Platform
    Copyright (C) 2012-2017 Twilight Finland 3D Oy Ltd. All rights reserved.
*/
#pragma once

#include "simd.hpp"
#include "common.hpp"

namespace mango {
namespace simd {

    // -----------------------------------------------------------------
    // reinterpret
    // -----------------------------------------------------------------

    static inline int32x4 int32x4_reinterpret(uint32x4 s)
    {
        return uint32x4::type(s);
    }

    static inline int32x4 int32x4_reinterpret(float32x4 s)
    {
        return _mm_castps_si128(s);
    }

    static inline uint32x4 uint32x4_reinterpret(int32x4 s)
    {
        return int32x4::type(s);
    }

    static inline uint32x4 uint32x4_reinterpret(float32x4 s)
    {
        return _mm_castps_si128(s);
    }

    static inline float32x4 float32x4_reinterpret(int32x4 s)
    {
        return _mm_castsi128_ps(s);
    }

    static inline float32x4 float32x4_reinterpret(uint32x4 s)
    {
        return _mm_castsi128_ps(s);
    }

    // -----------------------------------------------------------------
    // zero extend
    // -----------------------------------------------------------------

#if defined(MANGO_ENABLE_SSE4_1)

    static inline uint16x8 uint16x8_extend(uint8x16 s)
    {
        return _mm_cvtepu8_epi16(s);
    }

    static inline uint32x4 uint32x4_extend(uint8x16 s)
    {
        return _mm_cvtepu8_epi32(s);
    }

    static inline uint32x4 uint32x4_extend(uint16x8 s)
    {
        return _mm_cvtepu16_epi32(s);
    }

#else

    static inline uint16x8 uint16x8_extend(uint8x16 s)
    {
        return _mm_unpacklo_epi8(s, _mm_setzero_si128());
    }

    static inline uint32x4 uint32x4_extend(uint8x16 s)
    {
        const __m128i temp = _mm_unpacklo_epi8(s, _mm_setzero_si128());
        return _mm_unpacklo_epi16(temp, _mm_setzero_si128());
    }

    static inline uint32x4 uint32x4_extend(uint16x8 s)
    {
        return _mm_unpacklo_epi16(s, _mm_setzero_si128());
    }

#endif

    // -----------------------------------------------------------------
    // sign extend
    // -----------------------------------------------------------------

#if defined(MANGO_ENABLE_SSE4_1)

    static inline int16x8 int16x8_extend(int8x16 s)
    {
        return _mm_cvtepi8_epi16(s);
    }

    static inline int32x4 int32x4_extend(int8x16 s)
    {
        return _mm_cvtepi8_epi32(s);
    }

    static inline int32x4 int32x4_extend(int16x8 s)
    {
        return _mm_cvtepi16_epi32(s);
    }

#else

    static inline int16x8 int16x8_extend(int8x16 s)
    {
        const __m128i sign = _mm_cmpgt_epi8(_mm_setzero_si128(), s);
        return _mm_unpacklo_epi8(s, sign);
    }

    static inline int32x4 int32x4_extend(int8x16 s)
    {
        const __m128i temp = _mm_unpacklo_epi8(s, _mm_cmpgt_epi8(_mm_setzero_si128(), s));
        return _mm_unpacklo_epi16(temp, _mm_cmpgt_epi16(_mm_setzero_si128(), temp));
    }

    static inline int32x4 int32x4_extend(int16x8 s)
    {
        const __m128i sign = _mm_cmpgt_epi16(_mm_setzero_si128(), s);
        return _mm_unpacklo_epi16(s, sign);
    }

#endif

    // -----------------------------------------------------------------
    // pack
    // -----------------------------------------------------------------

    static inline uint8x16 uint8x16_pack(uint16x8 a, uint16x8 b)
    {
        return _mm_packus_epi16(a, b);
    }

    static inline uint16x8 uint16x8_pack(uint32x4 a, uint32x4 b)
    {
        return simd_packus_epi32(a, b);
    }

    static inline int8x16 int8x16_pack(int16x8 a, int16x8 b)
    {
        return _mm_packs_epi16(a, b);
    }

    static inline int16x8 int16x8_pack(int32x4 a, int32x4 b)
    {
        return _mm_packs_epi32(a, b);
    }

    // -----------------------------------------------------------------
    // float32
    // -----------------------------------------------------------------

    static inline float32x2 get_low(float32x4 a)
    {
        float x = get_x(a);
        float y = get_y(a);
        return float32x2_set2(x, y);
    }

    static inline float32x2 get_high(float32x4 a)
    {
        float z = get_z(a);
        float w = get_w(a);
        return float32x2_set2(z, w);
    }

    static inline float32x4 combine(float32x2 a, float32x2 b)
    {
        return float32x4_set4(a[0], a[1], b[0], b[1]);
    }

    static inline float32x4 float32x4_convert(uint32x4 s)
    {
        // conversion could be done by subtracting 0x80000000 from the value before signed conversion and
        // adding float(0x80000000) to the result after conversion but this would reduce precision on the LSBs.
        const __m128i mask = _mm_set1_epi32(0x0000ffff);
        const __m128i onep39 = _mm_set1_epi32(0x53000000);
        const __m128i x0 = _mm_or_si128(_mm_srli_epi32(s, 16), onep39);
        const __m128i x1 = _mm_and_si128(s, mask);
        const __m128 f1 = _mm_cvtepi32_ps(x1);
        const __m128 f0 = _mm_sub_ps(_mm_castsi128_ps(x0), _mm_castsi128_ps(onep39));
        return _mm_add_ps(f0, f1);
    }

    static inline float32x4 float32x4_convert(int32x4 s)
    {
        return _mm_cvtepi32_ps(s);
    }

    static inline uint32x4 uint32x4_convert(float32x4 s)
    {
        // conversion could be done by subtracting float(0x80000000) from the value before signed conversion and
        // adding 0x80000000 to the result after conversion but this would reduce precision on the LSBs.
	    __m128 x2 = _mm_castsi128_ps(_mm_set1_epi32(0x4f000000));
	    __m128 x1 = _mm_cmple_ps(x2, s);
  	    __m128i x0 = _mm_cvtps_epi32(_mm_sub_ps(s, _mm_and_ps(x2, x1)));
  	    return _mm_or_si128(x0, _mm_slli_epi32(_mm_castps_si128(x1), 31));
    }

    static inline int32x4 int32x4_convert(float32x4 s)
    {
        return _mm_cvtps_epi32(s);
    }

    static inline int32x4 int32x4_truncate(float32x4 s)
    {
        return _mm_cvttps_epi32(s);
    }

    // -----------------------------------------------------------------
    // float64
    // -----------------------------------------------------------------

    static inline float64x2 get_low(float64x4 a)
    {
        return a.xy;
    }

    static inline float64x2 get_high(float64x4 a)
    {
        return a.zw;
    }

    static inline float64x4 combine(float64x2 a, float64x2 b)
    {
        float64x4 result;
        result.xy = a;
        result.zw = b;
        return result;
    }

    static inline float64x4 float64x4_convert(int32x4 s)
    {
        float64x4 result;
        result.xy = _mm_cvtepi32_pd(s);
        result.zw = _mm_cvtepi32_pd(_mm_shuffle_epi32(s, 0xee));
        return result;
    }

    static inline float64x4 float64x4_convert(float32x4 s)
    {
        float64x4 result;
        result.xy = _mm_cvtps_pd(s);
        result.zw = _mm_cvtps_pd(_mm_shuffle_ps(s, s, 0xee));
        return result;
    }

    static inline int32x4 int32x4_convert(float64x4 s)
    {
        __m128i xy = _mm_cvtpd_epi32(s.xy);
        __m128i zw = _mm_cvtpd_epi32(s.zw);
        __m128i xzyw = _mm_unpacklo_epi32(xy, zw);
        return _mm_shuffle_epi32(xzyw, 0xd8);
    }

    static inline float32x4 float32x4_convert(float64x4 s)
    {
        __m128 xy00 = _mm_cvtpd_ps(s.xy);
        __m128 zw00 = _mm_cvtpd_ps(s.zw);
        return _mm_shuffle_ps(xy00, zw00, 0x44);
    }

    static inline float64x4 float64x4_convert(uint32x4 ui)
    {
        const __m128d bias = _mm_set1_pd((1ll << 52) * 1.5);
        const __m128i mask = _mm_set1_epi32(0x43380000);
        __m128i xy = _mm_unpacklo_epi32(ui, mask);
        __m128i zw = _mm_unpackhi_epi32(ui, mask);
        float64x4 result;
        result.xy = _mm_sub_pd(_mm_castsi128_pd(xy), bias);
        result.zw = _mm_sub_pd(_mm_castsi128_pd(zw), bias);
        return result;
    }

    static inline uint32x4 uint32x4_convert(float64x4 d)
    {
        const __m128d bias = _mm_set1_pd((1ll << 52) * 1.5);
        __m128 xy = _mm_castpd_ps(_mm_add_pd(d.xy, bias));
        __m128 zw = _mm_castpd_ps(_mm_add_pd(d.zw, bias));
        __m128 u = _mm_shuffle_ps(xy, zw, 0x88);
        return _mm_castps_si128(u);
    }

    static inline int32x4 int32x4_truncate(float64x4 s)
    {
        __m128i xy = _mm_cvttpd_epi32(s.xy);
        __m128i zw = _mm_cvttpd_epi32(s.zw);
        __m128i xzyw = _mm_unpacklo_epi32(xy, zw);
        return _mm_shuffle_epi32(xzyw, 0xd8);
    }

    // -----------------------------------------------------------------
    // float16
    // -----------------------------------------------------------------

#ifdef MANGO_ENABLE_F16C

    static inline float32x4 float32x4_convert(float16x4 h)
    {
        const __m128i* p = reinterpret_cast<const __m128i *>(&h);
        return _mm_cvtph_ps(_mm_loadl_epi64(p));
    }

    static inline float16x4 float16x4_convert(float32x4 f)
    {
        float16x4 h;
        __m128i* p = reinterpret_cast<__m128i *>(&h);
        _mm_storel_epi64(p, _mm_cvtps_ph(f, 0));
        return h;
    }

#else

    static inline float32x4 float32x4_convert(float16x4 h)
    {
        const __m128i* p = reinterpret_cast<const __m128i *>(&h);
        const int32x4 u = _mm_unpacklo_epi16(_mm_loadl_epi64(p), _mm_setzero_si128());

        int32x4 no_sign  = int32x4_and(u, int32x4_set1(0x7fff));
        int32x4 sign     = int32x4_and(u, int32x4_set1(0x8000));
        int32x4 exponent = int32x4_and(u, int32x4_set1(0x7c00));
        int32x4 mantissa = int32x4_and(u, int32x4_set1(0x03ff));

        // NaN or Inf
        int32x4 a = int32x4_or(int32x4_set1(0x7f800000), sll(mantissa, 13));

        // Zero or Denormal
        const int32x4 magic = int32x4_set1(0x3f000000);
        int32x4 b;
        b = add(magic, mantissa);
        b = int32x4_reinterpret(sub(float32x4_reinterpret(b), float32x4_reinterpret(magic)));

        // Numeric Value
        int32x4 c = add(int32x4_set1(0x38000000), sll(no_sign, 13));

        // Select a, b, or c based on exponent
        int32x4 mask;
        int32x4 result;

        mask = compare_eq(exponent, int32x4_zero());
        result = select(mask, b, c);

        mask = compare_eq(exponent, int32x4_set1(0x7c00));
        result = select(mask, a, result);

        // Sign
        result = int32x4_or(result, sll(sign, 16));

        return float32x4_reinterpret(result);
    }

    static inline float16x4 float16x4_convert(float32x4 f)
    {
        const float32x4 magic = float32x4_set1(Float(0, 15, 0).f);
        const int32x4 vinf = int32x4_set1(31 << 23);

        const int32x4 u = int32x4_reinterpret(f);
        const int32x4 sign = srl(int32x4_and(u, int32x4_set1(0x80000000)), 16);

        const int32x4 vexponent = int32x4_set1(0x7f800000);

        // Inf / NaN
        const int32x4 s0 = compare_eq(int32x4_and(u, vexponent), vexponent);
        int32x4 mantissa = int32x4_and(u, int32x4_set1(0x007fffff));
        int32x4 x0 = compare_eq(mantissa, int32x4_zero());
        mantissa = select(x0, int32x4_zero(), sra(mantissa, 13));
        const int32x4 v0 = int32x4_or(int32x4_set1(0x7c00), mantissa);

        int32x4 v1 = int32x4_and(u, int32x4_set1(0x7ffff000));
        v1 = int32x4_reinterpret(mul(float32x4_reinterpret(v1), magic));
        v1 = add(v1, int32x4_set1(0x1000));

#if defined(MANGO_ENABLE_SSE4_1)
        v1 = _mm_min_epi32(v1, vinf);
        v1 = sra(v1, 13);

        int32x4 v = select(s0, v0, v1);
        v = int32x4_or(v, sign);
        v = _mm_packus_epi32(v, v);
#else
        v1 = select(compare_gt(v1, vinf), vinf, v1);
        v1 = sra(v1, 13);

        int32x4 v = select(s0, v0, v1);
        v = int32x4_or(v, sign);
        v = _mm_slli_epi32 (v, 16);
        v = _mm_srai_epi32 (v, 16);
        v = _mm_packs_epi32 (v, v);
#endif

        float16x4 h;
        _mm_storel_epi64(reinterpret_cast<__m128i *>(&h), v);
        return h;
    }

#endif // MANGO_ENABLE_F16C

} // namespace simd
} // namespace mango