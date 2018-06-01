#ifndef VALHALLA_BALDR_PREDICTEDTRAFFIC_H_
#define VALHALLA_BALDR_PREDICTEDTRAFFIC_H_

#include <cstdint>
#include <valhalla/baldr/graphconstants.h>

namespace valhalla {
namespace baldr {

/**
 * Structure to store the predicted traffic information for an edge.
 */
class PredictedTraffic {
public:
  /**
   * Constructor with arguments.
   * @param  idx  Directed edge index to which this Predicted Traffic record applies to.
   * @param  compressed_offset  Offset to compressed data
   */
  PredictedTraffic(const uint32_t idx, const uint32_t compressed_offset);

  /**
   * Get the index of the directed edge this Predicted Traffic record applies to.
   * @return  Returns the directed edge index (within the same tile
   *          as the Predicted Traffic information).
   */
  uint32_t edgeindex() const {
    return edgeindex_;
  }

  /**
   * Set the directed edge index.
   * @param  idx  Directed edge index.
   */
  void set_edgeindex(const uint32_t idx);

  /**
   * Offset to the compressed traffic data. The offset is from the start
   * of the compressed traffic data within a tile.
   * @return  Returns offset from the start of the edge info within a tile.
   */
  uint64_t compressed_offset() const {
    return compressed_offset_;
  }

  /**
   * Set the offset to the compressed traffic data. The offset is from the start
   * of the compressed traffic data within a tile.
   * @param  compressed_offset  Offset from the start of the edge info within a tile.
   */
  void set_compressed_offset(const uint32_t compressed_offset);

protected:
  uint32_t edgeindex_ : 22; // kMaxTileEdgeCount in nodeinfo.h: 22 bits
  uint32_t compressed_offset_ : 24; // Offset to compressed data.
  uint32_t spare_ : 18;
};

} // namespace baldr
} // namespace valhalla

#endif // VALHALLA_BALDR_PREDICTEDTRAFFIC_H_
