#pragma once
#include "../core/timer.hpp"
#include "../utils/debugger.hpp"
#include "process_manager.hpp"
#include "ps_types.hpp"

class Scheduler {
private:
  Process *current_process = nullptr;
  ProcessManager &process_manager;
  size_t quantum = 10;
  size_t ticks = 0;

  inline static Scheduler *instance;

public:
  Scheduler(ProcessManager &pm) : process_manager(pm) { instance = this; }

  static Scheduler *get_scheduler() { return instance; }

  Process *pick_first_process() {
    Vector<Process *> &ptrain = process_manager.get_process_train();
    for (int i = 0; i < ptrain.size(); i++)
      if (ptrain[i]->is_ready())
        return ptrain[i];
    return nullptr;
  }

  Process *pick_next() {
    if (current_process == nullptr)
      return nullptr;
    Vector<Process *> &ptrain = process_manager.get_process_train();
    if (ptrain.size() == 0)
      return nullptr;

    int index = -1;
    for (int i = 0; i < ptrain.size(); i++)
      if (ptrain[i] == current_process)
        index = i;

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

    if (current_process)
      Debugger::logf("TICK pid=%d ticks=%d\n", (int)current_process->get_pid(),
                     (int)ticks);

    // Default: keep resuming right where we currently are.
    next_resume_rsp = reinterpret_cast<u64>(ctx);

    if (current_process == nullptr) {
      current_process = pick_first_process();
      if (!current_process)
        return;

      process_manager.set_process_state(current_process, ProcessState::RUNNING);
      Asm::write_cr3(current_process->get_page_table()->get_pml4());

      // Resume from this process's own private stack (its pre-built
      // synthetic frame — it's never actually run before), not the
      // shared kernel stack `ctx` currently points at.
      next_resume_rsp = current_process->get_context().rsp;
      Debugger::logf("SCHED: next_resume_rsp set to %d\n",
                     (unsigned)next_resume_rsp);
      return;
    }

    ticks++;
    if (ticks < quantum)
      return;
    ticks = 0;

    Process *next = pick_next();
    if (!next || next == current_process)
      return;

    // `ctx` already points into current_process's OWN private stack
    // (since it was switched in via next_resume_rsp last time) — just
    // remember where, so we can resume it here later.
    current_process->get_context().rsp = reinterpret_cast<u64>(ctx);

    process_manager.set_process_state(current_process, ProcessState::READY);
    process_manager.set_process_state(next, ProcessState::RUNNING);
    current_process = next;

    Asm::write_cr3(next->get_page_table()->get_pml4());
    next_resume_rsp = next->get_context().rsp;
  }
};
