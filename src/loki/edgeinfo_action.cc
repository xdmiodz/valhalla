#include <unordered_set>

#include "baldr/graphreader.h"
#include "baldr/rapidjson_utils.h"
#include "loki/worker.h"
#include "midgard/encoded.h"
#include "tyr/serializers.h"

using namespace valhalla;
using namespace valhalla::baldr;

namespace valhalla {
namespace loki {

std::string loki_worker_t::edgeinfo(valhalla_request_t& request) {
  auto id = rapidjson::get_optional<uint64_t>(request.document, "/graphid").get();
  auto edgeinfo = reader->edgeinfo(GraphId(id));
  auto json_map = json::map({{"shape", midgard::encode(edgeinfo.shape())},
                             {"way_id", static_cast<uint64_t>(edgeinfo.wayid())}});
  std::stringstream ss;
  ss << *json_map;
  return ss.str();
}

} // namespace loki
} // namespace valhalla
