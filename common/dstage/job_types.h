#ifndef DANS02_DSTAGE_JOB_TYPES_H
#define DANS02_DSTAGE_JOB_TYPES_H

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "common/dstage/synchronization.h"
#include "glog/logging.h"

namespace dans {

// ***********************************************************************
// Used for interaction with server.

#define REQUEST_GETFILE 1
#define REQUEST_PURGE 2
#define REQUEST_ACCEPT 11
#define REQUEST_REJECT 12
#define REQUEST_NOTFOUND 13

struct Protocol {
  int type;
  int priority;
  int object_id;
  int start_idx;
  int size_bytes;
};

// ***********************************************************************

using ClientCallback3 =
    std::function<void(unsigned priority, int object_id,
                       std::unique_ptr<std::vector<char>> object)>;

struct ConnectData {
  std::vector<std::string> ip_addresses;
  int object_id;
  std::shared_ptr<ClientCallback3> done;
};

struct ConnectDataInternal {
  std::string ip;
  int object_id;
  std::shared_ptr<ClientCallback3> done;
  std::shared_ptr<PurgeState> purge_state;
};

struct RequestData {
  std::shared_ptr<Connection> connection;
  int object_id;
  std::shared_ptr<ClientCallback3> done;
  std::shared_ptr<PurgeState> purge_state;
};

struct ResponseData {
  std::shared_ptr<Connection> connection;
  int object_id;
  unsigned index;
  std::unique_ptr<std::vector<char>> object;
  std::shared_ptr<ClientCallback3> done;
  std::shared_ptr<PurgeState> purge_state;
};

}  // namespace dans

#endif  // DANS02_DSTAGE_JOB_TYPES_H
