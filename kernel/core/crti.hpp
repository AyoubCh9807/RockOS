#pragma once

extern "C" void (*__init_array_start[])();
extern "C" void (*__init_array_end[])();

void call_constructors() {
    for (auto* it = __init_array_start; it < __init_array_end; it++) {
        (*it)();
    }
}
