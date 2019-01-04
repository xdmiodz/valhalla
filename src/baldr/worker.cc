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

baldr_worker_t::~baldr_worker_t() {}

void baldr_worker_t::cleanup() {}

EdgeInfo
baldr_worker_t::edgeinfo(const valhalla_request_t valhalla_request) const {
  return reader->edgeinfo(GraphId(10));
}

#ifdef HAVE_HTTP
worker_t::result_t
baldr_worker_t::work(const std::list<zmq::message_t> &job, void *request_info,
                     const std::function<void()> &interrupt_function) {
  auto &info = *static_cast<http_request_info_t *>(request_info);
  LOG_INFO("Got Baldr Request " + std::to_string(info.id));
  valhalla_request_t request;
  try {
    // crack open the original request
    std::string request_str(static_cast<const char *>(job.front().data()),
                            job.front().size());
    std::string serialized_options(
        static_cast<const char *>((++job.cbegin())->data()),
        (++job.cbegin())->size());
    request.parse(request_str, serialized_options);

    // Set the interrupt function
    service_worker_t::set_interrupt(interrupt_function);
  } catch (const std::exception &e) {
    return jsonify_error({299, std::string(e.what())}, info, request);
  }

  auto edge_id = request.document["edge_id"].GetInt();
}

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
