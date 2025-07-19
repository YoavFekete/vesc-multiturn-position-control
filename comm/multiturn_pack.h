#pragma once

#include <stdint.h>
#include <math.h>

/**
 * @brief  Pack an absolute multi-turn position into one unsigned 32-bit word:
 *         [ bits 31…14 = signed 18-bit revolutions ]
 *         [ bits 23… 0 = unsigned 14-bit intra-turn angle ]
 * @param  pos_deg  Absolute angle in degrees (can be <0 or >360).
 * @return          Packed uint32_t word.
 */
static inline uint32_t pack_multiturn_pos32(float pos_deg) {
    // Compute signed 8-bit full turns
    int32_t revs = (int32_t)floorf(pos_deg / 360.0f);
    if (revs < -(1<<17)) revs = -(1<<17);
    else if (revs >  (1<<17)-1) revs = (1<<17)-1;

    // Compute fractional angle in [0,360)
    float frac = pos_deg - (float)revs * 360.0f;
    if (frac < 0.0f)        frac += 360.0f;
    else if (frac >= 360.0f) frac -= 360.0f;
    
    // Quantize to 14 bits
    uint32_t angle14 = (uint32_t)roundf((frac / 360.0f) * ((1U<<14)-1));
    
    // Pack into MSB=rev, LSB=angle
    return ((uint32_t)(revs & ((1<<18)-1)) << 14) | (angle14 & 0x3FFF);
}

/**
 * @brief  Unpack a 32-bit packed multi-turn position (18-bit revs + 14-bit count)
 *         back into a floating-point degree value.
 * @param  raw_packed  The 32-bit word from pack_pos14().
 * @return             Absolute angle in degrees.
 */
static inline float unpack_multiturn_pos32(uint32_t raw_packed) {
    // 1) Pull down the top 18 bits into a signed 32-bit slot
    int32_t revs18 = (int32_t)(raw_packed >> 14);

    // 2) Sign-extend from 18 bits to 32 bits
    if (revs18 & (1 << 17)) {
        revs18 |= ~((1 << 18) - 1);
    }

    // 3) Extract the lower 14-bit encoder count
    uint32_t angle14 = raw_packed & 0x3FFFU;  // mask bits [13:0]

    // 4) Convert counts → fractional degrees
    float frac = ((float)angle14 / (float)((1U << 14) - 1)) * 360.0f;

    // 5) Combine full turns + fraction
    return (float)revs18 * 360.0f + frac;
}