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

  Process *get_current_process() const { return current_process; }

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

    static u32 dbg_tick = 0;
    if (current_process && (dbg_tick++ % 30 == 0))
      Debugger::logf("RSP pid=%d val=%d\n", (int)current_process->get_pid(),
                     (unsigned)reinterpret_cast<u64>(ctx));

    next_resume_rsp = reinterpret_cast<u64>(ctx);

    if (current_process == nullptr) {
      current_process = pick_first_process();
      if (!current_process)
        return;

      static bool logged_once = false;
      if (!logged_once) {
        Debugger::logf("FIRST SWITCH: pid=%d\n",
                       (int)current_process->get_pid());
        logged_once = true;
      }

      process_manager.set_process_state(current_process, ProcessState::RUNNING);
      Asm::write_cr3(current_process->get_page_table()->get_pml4());
      next_resume_rsp = current_process->get_context().rsp;
      return;
    }

    ticks++;
    if (ticks < quantum)
      return;
    ticks = 0;

    Process *next = pick_next();
    if (!next || next == current_process)
      return;

    static bool logged_switch_once = false;
    if (!logged_switch_once) {
      Debugger::logf("FIRST REAL SWITCH: from pid=%d to pid=%d\n",
                     (int)current_process->get_pid(), (int)next->get_pid());
      logged_switch_once = true;
    }

    current_process->get_context().rsp = reinterpret_cast<u64>(ctx);
    process_manager.set_process_state(current_process, ProcessState::READY);
    process_manager.set_process_state(next, ProcessState::RUNNING);
    current_process = next;
    Asm::write_cr3(next->get_page_table()->get_pml4());
    Debugger::logf("CR3 SWITCH OK pid=%d\n", (int)next->get_pid());

    next_resume_rsp = next->get_context().rsp;
  }
};
