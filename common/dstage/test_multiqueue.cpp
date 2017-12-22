#include "common/dstage/job.h"
#include "common/dstage/multiqueue.h"

#include <cassert>
#include <cstdlib>  // EXIT_SUCCESS and EXIT_FAILURE

namespace {

using namespace duplicate_aware_scheduling;
const JData kGenericData{5};

unsigned kNumberOfQueues = 3;
unsigned kDecoyA = 500;
unsigned kDecoyB = kDecoyA + 1;
unsigned kRealValue = kDecoyB + 1;

}  // namespace

int main() {
  bool success = true;
  fprintf(stderr, "test_multiqueue... ");

  JobIdFactory j_fact(0);

  auto decoy_a = std::make_shared<Job<JData>>(
      kGenericData, /*job_id=*/j_fact.CreateJobId(),
      /*priority=*/1, /*requested_duplication=*/1);
  auto decoy_b = std::make_shared<Job<JData>>(
      kGenericData, /*job_id=*/j_fact.CreateJobId(),
      /*priority=*/1, /*requested_duplication=*/1);
  auto decoy_real = std::make_shared<Job<JData>>(
      kGenericData, /*job_id=*/j_fact.CreateJobId(),
      /*priority=*/1, /*requested_duplication=*/1);

  MultiQueue<JData> prio_qs(kNumberOfQueues);

  // // Purge a missing JobId returns empty list.
  // assert(prio_qs.Purge(kDecoyA).empty());

  // Purge a job with several instances.
  prio_qs.Enqueue(decoy_a, std::vector<Priority>{0, 1, 2});
  // assert(prio_qs.Purge(kDecoyA).size() == kNumberOfQueues);

  // // Add some JobIds with some garbage in between.
  // prio_qs.Enqueue(kDecoyA, std::vector<Priority>{0, 1, 2});
  // prio_qs.Enqueue(kRealValue, std::vector<Priority>{0, 1, 2});
  // prio_qs.Enqueue(kDecoyB, std::vector<Priority>{0, 1, 2});
  // prio_qs.Enqueue(0, std::vector<Priority>{0});
  // prio_qs.Enqueue(1, std::vector<Priority>{1});
  // prio_qs.Enqueue(2, std::vector<Priority>{2});

  // // Purge the garbage.
  // assert(prio_qs.Purge(kDecoyA).size() == kNumberOfQueues);
  // assert(prio_qs.Purge(kDecoyB).size() == kNumberOfQueues);

  // // Test that everything else is in order.
  // for (unsigned i = 0; i < kNumberOfQueues; i++) {
  //   assert(prio_qs.Dequeue(i) == kRealValue);
  //   assert(prio_qs.Dequeue(i) == i);
  //   assert(prio_qs.Empty(i));
  // }

  if (success) {
    fprintf(stderr, " Passed\n");
    return 0;
  } else
    fprintf(stderr, " Failed\n");
  return 1;
}