#pragma once
#include <stdint.h>

/**
 * @brief  Update and return the multi-turn angle.
 * @param  raw  Current single-turn angle [0…360°].
 * @return Continuous angle over multiple revolutions.
 */
float position_update_multiturn(float raw);