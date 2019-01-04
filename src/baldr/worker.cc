#include <valhalla/baldr/graphreader.h>
#include <valhalla/baldr/worker.h>

namespace valhalla {
namespace baldr {

baldr_worker_t::baldr_worker_t(
    const boost::property_tree::ptree &config,
    const std::shared_ptr<baldr::GraphReader> &graph_reader)
    : config(config), reader(graph_reader) {
  // If we weren't provided with a graph reader make our own
  if (!reader)
    reader.reset(new GraphReader(config.get_child("mjolnir")));
}

#ifdef HAVE_HTTP
void run_service(const boost::property_tree::ptree &config) {
  // gets requests from baldr proxy
  auto upstream_endpoint =
      config.get<std::string>("baldr.service.proxy") + "_out";
  // or returns just location information back to the server
  auto loopback_endpoint = config.get<std::string>("httpd.service.loopback");
  auto interrupt_endpoint = config.get<std::string>("httpd.service.interrupt");

  // listen for requests
  zmq::context_t context;
  prime_server::worker_t worker(
      context, upstream_endpoint, "ipc:///dev/null", loopback_endpoint,
      interrupt_endpoint,
      std::bind(&baldr_worker_t::work, baldr_worker_t(config),
                std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3));
  worker.work();

  // TODO: should we listen for SIGINT and terminate gracefully/exit(0)?
}
#endif

} // namespace baldr
} // namespace valhalla
