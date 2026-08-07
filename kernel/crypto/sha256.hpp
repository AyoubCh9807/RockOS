#pragma once

#include "../shared/types.hpp"

class SHA256 {
private:
  static inline constexpr u32 K[64] = {0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
                                0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,

                                0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
                                0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,

                                0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
                                0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,

                                0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
                                0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,

                                0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
                                0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,

                                0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
                                0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,

                                0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
                                0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,

                                0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
                                0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

public:
  static u32 rotr(u32 x, u32 n) { return (x >> n) | (x << (32 - n)); }

  static u32 choose(u32 e, u32 f, u32 g) { return (e & f) ^ (~e & g); }

  static u32 majority(u32 a, u32 b, u32 c) {
    return (a & b) ^ (a & c) ^ (b & c);
  }

  static u32 sigma0(u32 x) { return rotr(x, 2) ^ rotr(x, 13) ^ rotr(x, 22); }

  static u32 sigma1(u32 x) { return rotr(x, 6) ^ rotr(x, 11) ^ rotr(x, 25); }

  static u32 gamma0(u32 x) { return rotr(x, 7) ^ rotr(x, 18) ^ (x >> 3); }

  static u32 gamma1(u32 x) { return rotr(x, 17) ^ rotr(x, 19) ^ (x >> 10); }

  static void hash(const u8 *data, u32 length, u8 output[32]) {

    u32 h[8] = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,

                0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};

    u32 padded_length = length + 1;

    while ((padded_length % 64) != 56) {
      padded_length++;
    }

    u8 block[64];

    u32 offset = 0;

    while (offset < padded_length) {

      for (int i = 0; i < 64; i++) {
        if (offset + i < length) {
          block[i] = data[offset + i];
        }

        else if (offset + i == length) {
          block[i] = 0x80;
        }

        else {
          block[i] = 0;
        }
      }

      offset += 64;

      if (offset == padded_length) {
        u64 bit_length = (u64)length * 8;

        for (int i = 0; i < 8; i++) {
          block[63 - i] = (bit_length >> (i * 8)) & 0xff;
        }
      }

      u32 w[64];

      for (int i = 0; i < 16; i++) {
        w[i] = ((u32)block[i * 4] << 24) | ((u32)block[i * 4 + 1] << 16) |
               ((u32)block[i * 4 + 2] << 8) | ((u32)block[i * 4 + 3]);
      }

      for (int i = 16; i < 64; i++) {
        w[i] = gamma1(w[i - 2]) + w[i - 7] + gamma0(w[i - 15]) + w[i - 16];
      }

      u32 a = h[0];
      u32 b = h[1];
      u32 c = h[2];
      u32 d = h[3];

      u32 e = h[4];
      u32 f = h[5];
      u32 g = h[6];
      u32 hh = h[7];

      for (int i = 0; i < 64; i++) {

        u32 t1 = hh + sigma1(e) + choose(e, f, g) + K[i] + w[i];

        u32 t2 = sigma0(a) + majority(a, b, c);

        hh = g;
        g = f;
        f = e;

        e = d + t1;

        d = c;
        c = b;
        b = a;

        a = t1 + t2;
      }

      h[0] += a;
      h[1] += b;
      h[2] += c;
      h[3] += d;

      h[4] += e;
      h[5] += f;
      h[6] += g;
      h[7] += hh;
    }

    for (int i = 0; i < 8; i++) {
      output[i * 4] = (h[i] >> 24) & 0xff;
      output[i * 4 + 1] = (h[i] >> 16) & 0xff;
      output[i * 4 + 2] = (h[i] >> 8) & 0xff;
      output[i * 4 + 3] = h[i] & 0xff;
    }
  }
};

