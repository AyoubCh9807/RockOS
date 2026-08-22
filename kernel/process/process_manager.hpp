#pragma once
#include "page_table.hpp"
#include "process.hpp"
#include "ps_types.hpp"

struct ProcessCreationEvent {
  Process *p;
  u32 pid;
  bool success;
  ProcessCreationEvent() : p(nullptr), pid(INVALID_PID), success(false) {}
  ProcessCreationEvent(Process *p, u32 pid, bool s)
      : p(p), pid(pid), success(s) {}
};

struct ProcessRetreivalEvent {
  bool success;
  Process *p;
  ProcessRetreivalEvent() : p(nullptr), success(false) {}
  ProcessRetreivalEvent(Process *p, bool s) : p(p), success(s) {}
};

class ProcessManager {
private:
  static constexpr int MAX_PROCESSES = 1024;
  Vector<Process *> process_train;

  FrameAllocator &frame_allocator;

  ProcessId id_pool[MAX_PROCESSES];

public:
  ProcessManager(FrameAllocator &falloc) : frame_allocator(falloc) {
    id_pool[0].is_free = false;
    id_pool[0].id = 0;
    for (int i = 1; i < MAX_PROCESSES; i++) {
      ProcessId pid;
      id_pool[i].id = i;
      id_pool[i].is_free = true;
    }
  }

  ~ProcessManager() {
    for (int i = 0; i < process_train.size(); i++) {
      delete process_train[i];
    }
  }

  ProcessCreationEvent create_process(size_t size, ProcessEntry entry) {

    PageTable *pt = new PageTable(frame_allocator);

    if (!pt->init()) {
      delete pt;
      return ProcessCreationEvent();
    }

    Process *p = new Process(pt, size, entry);

    u32 pid = get_free_id();

    if (pid == INVALID_PID) {
      delete p;
      return ProcessCreationEvent();
    }

    p->set_id(pid);

    if (!p->setup()) {
      delete p;
      return ProcessCreationEvent();
    }

    process_train.push_back(p);

    id_pool[pid].is_free = false;

    return ProcessCreationEvent(p, pid, true);
  }

  u32 get_free_id() {
    for (int i = 1; i < MAX_PROCESSES; i++) {
      if (id_pool[i].is_free)
        return id_pool[i].id;
    }
    return INVALID_PID;
  }

  ProcessRetreivalEvent get_process(u32 pid) {
    ProcessRetreivalEvent ev;
    for (int i = 0; i < process_train.size(); i++) {
      if (pid == process_train[i]->get_pid()) {
        ev.p = process_train[i];
        ev.success = true;
        return ev;
      }
    }
    return ev;
  }

  bool delete_process(u32 pid) {
    if (pid >= MAX_PROCESSES)
      return false;
    for (int i = 0; i < process_train.size(); i++) {
      if (process_train[i]->get_pid() == pid) {
        delete process_train[i];
        process_train.erase(i);
        free_pid(pid);
        return true;
      }
    }
    return false;
  }

  bool free_pid(u32 pid) {
    if (pid >= MAX_PROCESSES)
      return false;
    id_pool[pid].is_free = true;
    return true;
  }

  void set_process_state(Process *p, ProcessState s) {
    if (p)
      p->set_state(s);
  }

  Vector<Process *> &get_process_train() { return process_train; }
};
