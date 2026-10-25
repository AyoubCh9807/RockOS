#pragma once

#include "../../data/system.hpp"
#include "icommand.hpp"

class Base64Command : public ICommand {
private:
  static constexpr char map[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                "abcdefghijklmnopqrstuvwxyz"
                                "0123456789+/";

public:
  const char *name() const override { return "base64"; }

  CommandResult execute(int argc, char **argv) override {
    if (argc < 2 || !argv[1])
      return CommandResult(Generator::random_phrase(foolish_phrases));

    char buf[1024];
    StringUtils::join(buf, sizeof(buf), argc, argv, " ");

    size_t len = StringUtils::strlen(buf);
    char out[2048];

    size_t j = 0;

    for (size_t i = 0; i < len; i += 3) {
      u32 a = (u8)buf[i];
      u32 b = i + 1 < len ? (u8)buf[i + 1] : 0;
      u32 c = i + 2 < len ? (u8)buf[i + 2] : 0;

      u32 combined = (a << 16) | (b << 8) | c;

      out[j++] = map[(combined >> 18) & 0x3F];
      out[j++] = map[(combined >> 12) & 0x3F];
      out[j++] = i + 1 < len ? map[(combined >> 6) & 0x3F] : '=';
      out[j++] = i + 2 < len ? map[combined & 0x3F] : '=';
    }

    out[j] = '\0';

    return CommandResult(out);
  }
};
