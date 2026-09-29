#pragma once
#include "../containers/string.hpp"
#include "../shared/types.hpp"

enum class BatteryStatus { Unknown, Charging, Discharging, Full, NotCharging };

struct BatteryInfo {
  bool present;

  u8 percentage;
  BatteryStatus status;

  u32 voltage;
  u32 current;
  u32 remaining_capacity;

  u32 design_capacity;
  u32 full_charge_capacity;
  u32 cycle_count;

  u32 temperature;

  String manufacturer;
  String model;
  String serial;
};

class Battery {
public:
  bool initialize() {};
  bool update() {};

  const BatteryInfo &get_info() const {};

private:
  BatteryInfo info;
};
