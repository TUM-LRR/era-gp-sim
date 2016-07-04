#include "core/scheduler.hpp"

#include <thread>

Scheduler::Scheduler(int sleepMillis)
: stopRequested_(false), sleepMilliseconds_(sleepMillis), stopped_(false) {
  std::thread schedulerThread(&Scheduler::run_, this);
  schedulerThread.detach();
}

Scheduler::~Scheduler() {
  stopRequested_ = true;
  while (!stopped_) {
    /*wait*/
    stopped_.load();
  }
}

void Scheduler::run_() {
  stopped_ = false;
  stopRequested_.load();
  while (true) {
    std::function<void()> task;
    if (taskQueue_.pop(task)) {
      // there was something in the queue, ececute the task
      try {
        task();
      } catch (std::exception& e) {
        // TODO?
      }
    } else {
      if (stopRequested_) {
        break;
      }
      // the queue is empty, sleep for a bit
      std::this_thread::sleep_for(sleepMilliseconds_);
    }
  }
  stopped_ = true;
}

void Scheduler::push(std::function<void()> task) {
  taskQueue_.push(std::move(task));
}
