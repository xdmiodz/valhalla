#include "baldr/predicted_traffic.h"
#include <cmath>
#include <valhalla/midgard/logging.h>

namespace valhalla {
namespace baldr {

// Constructor with arguments.
PredictedTraffic::PredictedTraffic(const uint32_t idx, const uint32_t compressed_offset) {
  set_edgeindex(idx);
  set_compressed_offset(compressed_offset_);
}

// Set the directed edge index.
void PredictedTraffic::set_edgeindex(const uint32_t idx) {
  edgeindex_ = idx;
}

// Get the offset to the common edge data.
void PredictedTraffic::set_compressed_offset(const uint32_t compressed_offset) {
  if (compressed_offset > kMaxCompressedOffset) {
    // Consider this a catastrophic error
    LOG_ERROR("Exceeded maximum compressed traffic offset: " + std::to_string(compressed_offset));
    throw std::runtime_error("PredictedTraffic: exceeded maximum compressed traffic offset");
  } else {
    compressed_offset_ = compressed_offset;
  }
}

} // namespace baldr
} // namespace valhalla
