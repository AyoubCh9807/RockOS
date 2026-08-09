#pragma once

#include "../core/timer.hpp"
#include "../crypto/sha256.hpp"

class Random {
private:
  static inline u8 pool[32]; // Declaration only
  static inline u64 counter = 0;
  static inline bool initialized = false;

public:
  static void init() {
    for (int i = 0; i < 32; i++) {
      pool[i] = 0;
    }
    counter = 0;
    add_entropy(Timer::get_ticks());
    add_entropy(Asm::rdtsc());
    initialized = true;
  }

  static void add_entropy(u64 value) {
    u8 input[40];
    for (int i = 0; i < 32; i++) {
      input[i] = pool[i];
    }
    for (int i = 0; i < 8; i++) {
      input[32 + i] = (value >> (i * 8)) & 0xff;
    }
    SHA256::hash(input, 40, pool);
  }

  static u32 next() {
    if (!initialized) {
      init();
    }
    u8 input[40];
    for (int i = 0; i < 32; i++) {
      input[i] = pool[i];
    }
    for (int i = 0; i < 8; i++) {
      input[32 + i] = (counter >> (i * 8)) & 0xff;
    }
    u8 output[32];
    SHA256::hash(input, 40, output);
    counter++;
    add_entropy(counter);

    u32 result = 0;
    result |= output[0];
    result |= output[1] << 8;
    result |= output[2] << 16;
    result |= output[3] << 24;
    return result;
  }
};
