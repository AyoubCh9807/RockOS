#include "compiler_string_utils.hpp"
#include <bit>
#include <cstdint>
#include <iostream>

class RodataManager {
private:
  std::string output;
  size_t str_counter = 0;
  size_t f64_counter = 0;
  size_t f32_counter = 0;

public:
  std::string add_string(const std::string &value) {
    std::string label = StringUtils::format("l_str_%d", str_counter++);

    output.append(StringUtils::format("%s:\n", label.c_str()));

    output.append("db ");

    for (size_t i = 0; i < value.size(); ++i) {
      output.append(
          StringUtils::format("0x%02X", static_cast<unsigned char>(value[i])));

      if (i + 1 < value.size())
        output.append(", ");
    }

    if (!value.empty())
      output.append(", ");

    output.append("0\n");

    return label;
  }

  std::string add_f32(float value) {
    std::string label = StringUtils::format("l_f32_%d", f32_counter++);

    uint32_t bits = std::bit_cast<uint32_t>(value);

    std::string hex = StringUtils::format("0x%08X", bits);

    output.append(StringUtils::format("%s:\n", label.c_str()));

    output.append(StringUtils::format("dd %s\n", hex.c_str()));

    return label;
  }

  std::string add_f64(double value) {
    std::string label = StringUtils::format("l_f64_%d", f64_counter++);

    uint64_t bits = std::bit_cast<uint64_t>(value);

    std::string hex =
        StringUtils::format("0x%016llX", static_cast<unsigned long long>(bits));

    output.append(StringUtils::format("%s:\n", label.c_str()));

    output.append(StringUtils::format("dq %s\n", hex.c_str()));

    return label;
  }
  const std::string &get_output() const { return output; }

  void clear() {
    output.clear();
    str_counter = 0;
    f64_counter = 0;
    f32_counter = 0;
  }
};
