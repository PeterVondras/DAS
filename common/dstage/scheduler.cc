#include "common/dstage/scheduler.h"

#include <memory>

#include "common/thread/thread_utility.h"
#include "glog/logging.h"

DEFINE_bool(set_thread_priority, false,
            "Set thread priority with Linux OS, "
            "which requires running with `sudo`.");

namespace {
const int kSchedule = SCHED_RR;
}  // namespace

namespace dans {

template <typename T>
Scheduler<T>::Scheduler(std::vector<unsigned> threads_per_prio)
    : _set_thread_priority(FLAGS_set_thread_priority),
      _running(false),
      _max_priority(threads_per_prio.size() - 1),
      _multi_q_p(nullptr),
      _destructing(false),
      _threads_per_prio(threads_per_prio) {
  VLOG(4) << __PRETTY_FUNCTION__
          << " threads_per_prio vector size=" << threads_per_prio.size();
  CHECK(!_threads_per_prio.empty());
}

template <typename T>
Scheduler<T>::~Scheduler() {
  VLOG(4) << __PRETTY_FUNCTION__;
  {
    std::unique_lock<std::shared_timed_mutex> lock(_destructing_lock);
    _destructing = true;
  }
  if (_running) {
    _multi_q_p->ReleaseQueues();
    for (auto&& threads_the_this_prio : _workers) {
      for (auto&& worker : threads_the_this_prio) {
        worker.join();
      }
    }
  }
}

template <typename T>
std::list<UniqConstJobPtr<T>> Scheduler<T>::Purge(JobId job_id) {
  VLOG(4) << __PRETTY_FUNCTION__ << " job_id=" << job_id;
  static_cast<void>(job_id);
  std::list<UniqConstJobPtr<T>> purged;
  return purged;
}

template <typename T>
void Scheduler<T>::LinkMultiQ(BaseMultiQueue<T>* multi_q_p) {
  VLOG(4) << __PRETTY_FUNCTION__;
  CHECK_NOTNULL(multi_q_p);
  _multi_q_p = multi_q_p;
}

template <typename T>
void Scheduler<T>::Run() {
  VLOG(4) << __PRETTY_FUNCTION__;
  CHECK_NOTNULL(_multi_q_p);
  CHECK(!_running);

  _running = true;
  Priority p = 0;
  // Creating number_threads_at_this_prio threads for each priority, where
  // number_threads_at_this_prio can be different at each level.
  for (auto&& number_threads_at_this_prio : _threads_per_prio) {
    LOG(INFO) << "Creating threadpool of size " << number_threads_at_this_prio
              << " for scheduler at priority " << p;
    std::vector<std::thread> threads_at_this_prio;
    _workers.push_back(std::vector<std::thread>());
    for (unsigned i = 0; i < number_threads_at_this_prio; ++i) {
      _workers.back().push_back(
          std::thread(&Scheduler<T>::StartScheduling, this, p));
      if (_set_thread_priority) {
        // TODO(pvondras) Thred policy and priority mapping should be
        // set from the options in scheduler constructor.
        int new_prio = 90 - (p * 30) < 1 ? 1 : 90 - (p * 30);
        threadutility::ThreadUtility::SetPriority(_workers.back().back(),
                                                  kSchedule, new_prio);
      }
    }
    p++;
  }
}

template <typename T>
void Scheduler<T>::StartScheduling(Priority prio) {
  VLOG(4) << __PRETTY_FUNCTION__ << " prio=" << prio;
  while (true) {
    {
      std::shared_lock<std::shared_timed_mutex> lock(_destructing_lock);
      if (_destructing) return;
    }

    UniqConstJobPtr<T> job = _multi_q_p->Dequeue(prio);
    if (job == nullptr) continue;
    // A likely spot to add functionality for a more advanced scheduler.
  }
}

// As long as template implementation is in .cpp file, must explicitly tell
// compiler which types to compile...
template class Scheduler<JData>;

}  // namespace dans