#pragma once
#include "../shared/types.hpp"

static constexpr auto MAX_ALLOCATION_EVENTS = 128;

struct HeapAllocationEvent {
  enum Type : u8 { ALLOCATED, FREED };
  Type type;
  size_t size;
};

class Events {

  private:

  int head = 0;
  int tail = 0;
  int count = 0;

   HeapAllocationEvent allocation_train[MAX_ALLOCATION_EVENTS];

  public:

  bool is_event_pending() { return count != 0; }

  HeapAllocationEvent consume_next_event() {
    if (count == 0)
      return HeapAllocationEvent(HeapAllocationEvent::FREED, 0);

    // EV1 EV2 EV3 EV4
    // tail    head
    HeapAllocationEvent ev;
    ev = allocation_train[tail];
    tail = (tail + 1) % MAX_ALLOCATION_EVENTS;
    count--;
    return ev;
  }

  bool push_event(HeapAllocationEvent ev) {
    if (count == MAX_ALLOCATION_EVENTS)
      return false;

    allocation_train[head] = ev;
    head = (head + 1) % MAX_ALLOCATION_EVENTS;
    count++;
    return true;
  }

  bool events_consumed() { return count == 0; }
};
