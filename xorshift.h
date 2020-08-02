#pragma once

// -------------------------------------------------------------------------------------------------

static uint16_t prng = 1;
inline uint16_t xorShift() {
  prng ^= prng << 7;
  prng ^= prng >> 9;
  prng ^= prng << 8;
  return prng;
}

// -------------------------------------------------------------------------------------------------