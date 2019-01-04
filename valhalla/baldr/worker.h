#pragma once

#include <boost/property_tree/ptree.hpp>
#include <valhalla/baldr/edgeinfo.h>
#include <valhalla/baldr/graphreader.h>
#include <valhalla/worker.h>

namespace valhalla {
namespace baldr {

#ifdef HAVE_HTTP
void run_service(const boost::property_tree::ptree &config);
#endif

class baldr_worker_t : public service_worker_t {
public:
  baldr_worker_t(const boost::property_tree::ptree &config,
                 const std::shared_ptr<GraphReader> &graph_reader = {});
  virtual ~baldr_worker_t();
#ifdef HAVE_HTTP
  virtual worker_t::result_t
  work(const std::list<zmq::message_t> &job, void *request_info,
       const std::function<void()> &interrupt) override;
#endif
  virtual void cleanup() override;

  EdgeInfo edgeinfo(const valhalla_request_t valhalla_request) const;

protected:
  boost::property_tree::ptree config;
  std::shared_ptr<GraphReader> reader;
};

} // namespace baldr
} // namespace valhalla
