#include "baldr/rapidjson_utils.h"
#include <boost/program_options.hpp>
#include <boost/property_tree/ptree.hpp>
#include <cstdint>

#include "baldr/graphconstants.h"
#include "baldr/graphreader.h"
#include "baldr/tilehierarchy.h"
#include "midgard/encoded.h"
#include "midgard/logging.h"

#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <unordered_set>

#include "config.h"

using namespace valhalla::midgard;
using namespace valhalla::baldr;

namespace bpo = boost::program_options;

// global options instead of passing them around
std::string column_separator{'\0'};
std::string row_separator = "\n";
std::string config;
bool ferries;
bool unnamed;

// program entry point
int main(int argc, char* argv[]) {
  bpo::options_description options("valhalla_export_edges " VALHALLA_VERSION "\n"
                                   "\n"
                                   " Usage: valhalla_export_edges [options]\n"
                                   "\n"
                                   "valhalla_export_edges is a simple command line test tool which "
                                   "dumps information about each graph edge. "
                                   "\n"
                                   "\n");

  options.add_options()("help,h", "Print this help message.")("version,v",
                                                              "Print the version of this software.")(
      "column,c", bpo::value<std::string>(&column_separator),
      "What separator to use between columns [default=\\0].")(
      "row,r", bpo::value<std::string>(&column_separator),
      "What separator to use between row [default=\\n].")("ferries,f",
                                                          "Export ferries as well [default=false]")(
      "unnamed,u", "Export unnamed edges as well [default=false]")
      // positional arguments
      ("config", bpo::value<std::string>(&config), "Valhalla configuration file [required]");

  bpo::positional_options_description pos_options;
  pos_options.add("config", 1);
  bpo::variables_map vm;
  try {
    bpo::store(bpo::command_line_parser(argc, argv).options(options).positional(pos_options).run(),
               vm);
    bpo::notify(vm);
  } catch (std::exception& e) {
    std::cerr << "Unable to parse command line options because: " << e.what() << "\n"
              << "This is a bug, please report it at " PACKAGE_BUGREPORT << "\n";
    return EXIT_FAILURE;
  }

  if (vm.count("help") || !vm.count("config")) {
    std::cout << options << "\n";
    return EXIT_SUCCESS;
  }

  if (vm.count("version")) {
    std::cout << "valhalla_export_edges " << VALHALLA_VERSION << "\n";
    return EXIT_SUCCESS;
  }

  bool ferries = vm.count("ferries");
  bool unnamed = vm.count("unnamed");

  // parse the config
  boost::property_tree::ptree pt;
  rapidjson::read_json(config.c_str(), pt);

  // configure logging
  valhalla::midgard::logging::Configure({{"type", "std_err"}, {"color", "true"}});

  // get something we can use to fetch tiles
  valhalla::baldr::GraphReader reader(pt.get_child("mjolnir"));

  // keep the global number of edges encountered at the point we encounter each tile
  // this allows an edge to have a sequential global id and makes storing it very small

  std::unordered_set<uint64_t> edge_set;
  uint64_t edge_count = 0;
  for (const auto& level : TileHierarchy::levels()) {
    for (uint32_t tid = 0; tid < level.second.tiles.TileCount(); ++tid) {
      GraphId tile_id{tid, level.first, 0};
      if (reader.DoesTileExist(tile_id)) {
        const auto* tile = reader.GetGraphTile(tile_id);
        for (uint32_t eid = 0; eid < tile->header()->directededgecount(); ++eid) {
          auto edge_id = GraphId(tid, level.first, eid);

          if (edge_set.count(edge_id.value) > 0) {
            continue;
          }

          auto directededge = reader.directededge(edge_id);
          auto edgeinfo = reader.edgeinfo(edge_id);
          auto way_id = edgeinfo.wayid();

          if (way_id == 0) {
            continue;
          }

          edge_set.emplace(edge_id.value);

          std::cout << edge_id.value << column_separator;
          std::cout << way_id << row_separator;
          std::cout.flush();
        }
        reader.Clear();
      }
    }
  }

  return EXIT_SUCCESS;
}
