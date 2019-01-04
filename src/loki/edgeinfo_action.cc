#include <unordered_set>

#include "baldr/graphreader.h"
#include "baldr/rapidjson_utils.h"
#include "loki/worker.h"
#include "tyr/serializers.h"

using namespace valhalla;
using namespace valhalla::baldr;

namespace valhalla {
namespace loki {

std::string loki_worker_t::edgeinfo(valhalla_request_t &request) {
  auto id = rapidjson::get_optional<int>(request.document, "/graphid").get();
  auto edgeinfo = reader->edgeinfo(GraphId(id));
  return edgeinfo.encoded_shape();
}

} // namespace loki
} // namespace valhalla
