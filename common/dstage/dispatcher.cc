
#include "common/dstage/dispatcher.h"

#include <memory>

#include "glog/logging.h"

namespace {}  // namespace

namespace dans {

template <typename T>
Dispatcher<T>::Dispatcher(unsigned max_priority)
    : _running(false), _max_priority(max_priority), _multi_q_p(nullptr) {
  VLOG(4) << __PRETTY_FUNCTION__ << " max_priority=" << max_priority;
}

// Duplicates job and inserts into MultiQueue
template <typename T>
void Dispatcher<T>::Dispatch(UniqConstJobPtr<T> job_p,
                             unsigned requested_duplication) {
  VLOG(4) << __PRETTY_FUNCTION__
          << ((job_p == nullptr) ? " job_p=nullptr," : " job_id=")
          << ((job_p == nullptr) ? ' ' : job_p->job_id)
          << ((job_p == nullptr) ? ' ' : ',')
          << " requested_duplication=" << requested_duplication;
  CHECK(_running);
  if (job_p == nullptr) {
    LOG(WARNING) << "Dispatcher sent nullptr instead of a job with "
                 << "requested_duplication=" << requested_duplication;
    return;
  }
  CHECK_LE(job_p->priority, _max_priority);

  // Get lowest priority (highest value) appropriate.
  Priority max_prio = job_p->priority + requested_duplication > _max_priority
                          ? _max_priority
                          : job_p->priority + requested_duplication;
  unsigned duplication = max_prio - job_p->priority;

  for (Priority prio = job_p->priority; prio <= max_prio; prio++) {
    auto duplicate_job_p = std::make_unique<const Job<T>>(
        job_p->job_data, job_p->job_id, prio, duplication);
    _multi_q_p->Enqueue(std::move(duplicate_job_p));
  }
}

template <typename T>
void Dispatcher<T>::LinkMultiQ(BaseMultiQueue<T>* multi_q_p) {
  VLOG(4) << __PRETTY_FUNCTION__;
  CHECK_NOTNULL(multi_q_p);
  _multi_q_p = multi_q_p;
  _running = true;
}

// As long as template implementation is in .cpp file, must explicitly tell
// compiler which types to compile...
template class Dispatcher<JData>;

}  // namespace dans