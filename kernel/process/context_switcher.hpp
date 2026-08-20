#pragma once

#include "cpu_context.hpp"

extern "C" void switch_context(CpuContext *current,
                               CpuContext *next);
