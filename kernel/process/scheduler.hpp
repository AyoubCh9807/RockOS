#pragma once
#include "process_manager.hpp"
#include "ps_types.hpp"

class Scheduler {
private:
  Process *current_process = nullptr;
  ProcessManager &process_manager;
  size_t quantum = 10;
  size_t ticks = 0;

public:
  Scheduler(ProcessManager &pm) : process_manager(pm) {}

  void tick() {
    if (current_process == nullptr) {
      current_process = pick_first_process();
      if (current_process)
        process_manager.set_process_state(current_process,
                                          ProcessState::RUNNING);
      return;
    }
    Vector<Process *> &ptrain = process_manager.get_process_train();
    if (ptrain.size() == 0)
      return;
    ticks++;
    if (ticks % quantum != 0)
      return;
    Process *next_process = pick_next();
    ticks %= quantum;
    if (next_process != current_process) {
      // switch context
    }
  };
  Process *pick_first_process() {
    Vector<Process *> &ptrain = process_manager.get_process_train();
    if (ptrain.size() == 0)
      return nullptr;
    for (int i = 0; i < ptrain.size(); i++) {
      if (ptrain[i]->is_ready())
        return ptrain[i];
    }
    return nullptr;
  }

  void set_current(Process *process) {
    if (process != nullptr)
      current_process = process;
  };
  Process *get_current() const { return current_process; };

  Process *pick_next() {
    if (current_process == nullptr)
      return nullptr;
    Vector<Process *> &ptrain = process_manager.get_process_train();
    if (ptrain.size() == 0)
      return nullptr;

    int index = -1;
    for (int i = 0; i < ptrain.size(); i++) {
      if (ptrain[i] == current_process)
        index = i;
    }

    if (index == -1)
      return current_process;

    int i = (index + 1) % ptrain.size();
    int check_count = 0;
    do {
      if (ptrain[i]->is_ready())
        return ptrain[i];
      check_count++;
      i = (i + 1) % ptrain.size();
    } while (check_count < ptrain.size());

    return current_process;
  }

  void on_timer(CpuContext *ctx) {
    if (!ctx)
      return;

    // First timer tick: choose the first process.
    if (current_process == nullptr) {
      current_process = pick_first_process();

      if (current_process)
        process_manager.set_process_state(current_process,
                                          ProcessState::RUNNING);

      return;
    }

    ticks++;

    if (ticks < quantum)
      return;

    ticks = 0;

    Process *next = pick_next();

    if (!next || next == current_process)
      return;

    // Save the currently running CPU state.
    current_process->get_context() = *ctx;

    // Switch which process is considered running.
    process_manager.set_process_state(current_process, ProcessState::READY);

    process_manager.set_process_state(next, ProcessState::RUNNING);

    current_process = next;

    // Put the next process's saved CPU state
    // into the interrupt frame.
    *ctx = next->get_context();
  }
};
