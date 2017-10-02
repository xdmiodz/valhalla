#ifndef VALHALLA_MJOLNIR_DATAQUALITY_H
#define VALHALLA_MJOLNIR_DATAQUALITY_H

#include <cstdint>
#include <algorithm>
#include <unordered_set>
#include <map>

#include <valhalla/midgard/logging.h>
#include <valhalla/baldr/graphid.h>
#include <valhalla/baldr/directededge.h>

namespace valhalla {
namespace mjolnir {

// Simple struct for holding duplicate ways to allow sorting by edgecount
struct DuplicateWay {
  uint64_t wayid1;
  uint64_t wayid2;
  uint32_t edgecount;

  DuplicateWay(const uint64_t id1, const uint64_t id2, const uint32_t n)
      : wayid1(id1),
        wayid2(id2),
        edgecount(n) {
  }

  // For sorting by number of duplicate edges
  bool operator < (const DuplicateWay& other) const {
      return edgecount > other.edgecount;
  }
};

/**
 * Class used to generate statistics and gather data quality issues.
 * Also forms per tile data quality metrics.
 */
class DataQuality {
 public:
  /**
   * Constructor
   */
  DataQuality();

  /**
   * Add statistics (accumulate from several DataQuality objects)
   * @param  stats  Data quality object to add to stats
   */
  void AddStatistics(const DataQuality& stats);

  /**
   * Adds a duplicte edge report (gives the 2 way ids that overlap).
   * @param  wayid1  Way Id of one edge.
   * @param  wayid2  Way Id of the other edge.
   */
  void AddDuplicate(const uint64_t wayid1, const uint64_t wayid2);

  /**
   * Log simple statistics.
   */
  void LogStatistics() const;

  /**
   * Log duplicates to a separate file.
   */
  void LogDuplicates() const;

  // Public - simple stats
  uint32_t nodecount;
  uint32_t directededge_count;
  uint32_t simplerestrictions;
  uint32_t timedrestrictions;
  uint32_t culdesaccount;
  uint32_t forward_restrictions_count;
  uint32_t reverse_restrictions_count;
  uint32_t node_counts[128];


 protected:
  // Duplicate edges (overlapping ways)
  std::map<std::pair<uint64_t, uint64_t>, uint32_t> duplicateways_;
};

}
}

#endif  // VALHALLA_MJOLNIR_DATAQUALITY_H
