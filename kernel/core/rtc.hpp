#pragma once

#include "../containers/string.hpp"
#include "../core/asm.hpp"
#include "../shared/types.hpp"
#include "../utils/math_utils.hpp"
#include "../utils/string_utils.hpp"

namespace RTC {

constexpr u8 CMOS_INDEX = 0x70;
constexpr u8 CMOS_DATA = 0x71;

constexpr u8 SECONDS_REGISTER = 0x00;
constexpr u8 MINUTES_REGISTER = 0x02;
constexpr u8 HOURS_REGISTER = 0x04;
constexpr u8 DAY_REGISTER = 0x07;
constexpr u8 MONTH_REGISTER = 0x08;
constexpr u8 YEAR_REGISTER = 0x09;

static u8 read_seconds() {
  Asm::outb(CMOS_INDEX, SECONDS_REGISTER);
  return Asm::inb(CMOS_DATA);
}

static u8 read_minutes() {
  Asm::outb(CMOS_INDEX, MINUTES_REGISTER);
  return Asm::inb(CMOS_DATA);
}

static u8 read_hours() {
  Asm::outb(CMOS_INDEX, HOURS_REGISTER);
  return Asm::inb(CMOS_DATA);
}

static u8 read_day() {
  Asm::outb(CMOS_INDEX, DAY_REGISTER);
  return Asm::inb(CMOS_DATA);
}

static u8 read_month() {
  Asm::outb(CMOS_INDEX, MONTH_REGISTER);
  return Asm::inb(CMOS_DATA);
}

static u8 read_year() {
  Asm::outb(CMOS_INDEX, YEAR_REGISTER);
  return Asm::inb(CMOS_DATA);
}

static u8 to_utc1(u8 hours) { return (hours + 1) % 24; }

static String get_full_time() {
  u8 seconds = Math::bcd_to_binary(read_seconds());
  u8 minutes = Math::bcd_to_binary(read_minutes());
  u8 hours = to_utc1(Math::bcd_to_binary(read_hours()));
  u8 day = Math::bcd_to_binary(read_day());
  u8 month = Math::bcd_to_binary(read_month());
  u8 year = Math::bcd_to_binary(read_year());

  return StringUtils::format("%d-%d-%d %d:%d:%d", 2000 + year, month, day,
                             hours, minutes, seconds);
}

} // namespace RTC
