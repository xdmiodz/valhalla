#include "mjolnir/dataquality.h"
#include <fstream>
#include <vector>

using namespace valhalla::midgard;
using namespace valhalla::baldr;

namespace valhalla {
namespace mjolnir {

// Constructor
DataQuality::DataQuality()
    : nodecount(0),
      directededge_count(0),
      simplerestrictions(0),
      timedrestrictions(0),
      culdesaccount(0),
      forward_restrictions_count(0),
      reverse_restrictions_count(0),
      node_counts{} {
  // Clear out the duplicates file
  std::ofstream dupfile;
  dupfile.open("duplicateways.txt", std::ofstream::out | std::ofstream::app);
}

// Add statistics (accumulate from several DataQuality objects)
void DataQuality::AddStatistics(const DataQuality& stats) {
  nodecount          += stats.nodecount;
  directededge_count += stats.directededge_count;
  simplerestrictions += stats.simplerestrictions;
  timedrestrictions  += stats.timedrestrictions;
  culdesaccount      += stats.culdesaccount;
  for (uint32_t i = 0; i < 128; i++) {
    node_counts[i] += stats.node_counts[i];
  }
}

// Adds an issue.
void DataQuality::AddDuplicate(const uint64_t wayid1, const uint64_t wayid2) {
  std::pair<uint64_t, uint64_t> wayids = std::make_pair(wayid1, wayid2);
  auto it = duplicateways_.find(wayids);
  if (it == duplicateways_.end()) {
    duplicateways_.emplace(wayids, 1);
  } else {
    it->second++;
  }
}

// Logs statistics and issues
void DataQuality::LogStatistics() const {
  LOG_DEBUG("Node Count = " + std::to_string(nodecount));
  LOG_DEBUG("Directed Edge Count = " + std::to_string(directededge_count));
  LOG_DEBUG("Simple Restriction Count = " + std::to_string(simplerestrictions));
  LOG_DEBUG("Timed  Restriction Count = " + std::to_string(timedrestrictions));
  LOG_DEBUG("Cul-de-Sac Count = " + std::to_string(culdesaccount));
  LOG_DEBUG("Node edge count histogram:");
  for (uint32_t i = 0; i < 128; i++) {
    if (node_counts[i] > 0) {
      LOG_DEBUG(std::to_string(i) + ": " + std::to_string(node_counts[i]));
    }
  }
}

// Logs issues
void DataQuality::LogDuplicates() const {
  // Log the duplicate ways - sort by number of duplicate edges
  std::vector<DuplicateWay> dups;
  if (duplicateways_.size() > 0) {
    for (const auto& dup : duplicateways_) {
      dups.emplace_back(DuplicateWay(dup.first.first, dup.first.second,
                                      dup.second));
    }
  }

  // Sort by edgecount and write to separate file
  std::ofstream dupfile;
  std::sort(dups.begin(), dups.end());
  dupfile.open("duplicateways.txt", std::ofstream::out | std::ofstream::app);
  dupfile << "WayID1   WayID2    DuplicateEdges" << std::endl;
  for (const auto& dupway : dups) {
    dupfile << dupway.wayid1 << "," << dupway.wayid2 << ","
            << dupway.edgecount << std::endl;
  }
  dupfile.close();
}

}
}
