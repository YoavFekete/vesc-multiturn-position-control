#include "position_multiturn.h"
#include <stdint.h>
#include <stdbool.h>

//  Configure these as needed:

#define ANGLE_WRAP_THRESHOLD    180.0f  // degrees

static int32_t  round_count  = 0;
static float    prev_angle   = 0.0f;
static bool     inited       = false;

/**
 * @brief  Call this every control cycle to update cumulative angle.
 * @param  raw       The current 0–360° reading from your encoder (in degrees).
 * @return The continuous (multi-turn) angle, in degrees.
 */
float position_update_multiturn(float raw) {
    
    if (!inited) {
        prev_angle = raw;
        round_count = 0;
        inited = true;
        return raw;
    }

    float delta = raw - prev_angle;

    // detect wrap-around crossing
    if (delta > ANGLE_WRAP_THRESHOLD) {
        // jumped from high->low (e.g. 359 → 1) ⇒ decrement turns
        round_count--;
    } else if (delta < -ANGLE_WRAP_THRESHOLD) {
        // jumped from low->high (e.g. 1 → 359) ⇒ increment turns
        round_count++;
    }

    prev_angle = raw;
    // cumulative angle = current + (# turns × 360°)
    return raw + (float)round_count * 360.0f;
}

