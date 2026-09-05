#pragma once

#include "../containers/string.hpp"
#include "../core/asm.hpp"
#include "../shared/types.hpp"
#include "../utils/math_utils.hpp"
#include "../utils/string_utils.hpp"

enum class Timezone { UTC1, UTC2 };

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

inline u8 get_seconds() { return MathUtils::bcd_to_binary(read_seconds()); }

inline u8 get_minutes() { return MathUtils::bcd_to_binary(read_minutes()); }

inline u8 get_hours() { return MathUtils::bcd_to_binary(read_hours()); }

inline u8 get_day() { return MathUtils::bcd_to_binary(read_day()); }

inline u8 get_month() { return MathUtils::bcd_to_binary(read_month()); }

inline u8 get_year() { return MathUtils::bcd_to_binary(read_year()); }

static void get_full_time_into(char *buf, size_t max_len) {
  u8 seconds = get_seconds();
  u8 minutes = get_minutes();
  u8 hours = get_hours();
  u8 day = get_day();
  u8 month = get_month();
  u8 year = get_year();

  StringUtils::snprintf(buf, max_len, "%d-%02d-%02d %02d:%02d:%02d",
                        2000 + year, month, day, hours, minutes, seconds);
}
static void get_full_time_into(char *buf, size_t max_len, Timezone tz) {
  u8 seconds = get_seconds();
  u8 minutes = get_minutes();
  u8 hours = get_hours();
  u8 day = get_day();
  u8 month = get_month();
  u8 year = get_year();

  switch(tz) {
    case Timezone::UTC1: hours = (hours + 1) % 24; break;
    case Timezone::UTC2: hours = (hours + 2) % 24; break;
  }

  StringUtils::snprintf(buf, max_len, "%d-%02d-%02d %02d:%02d:%02d",
                        2000 + year, month, day, hours, minutes, seconds);
}

} // namespace RTC
