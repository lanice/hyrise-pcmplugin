#pragma once

#include <json.hpp>

#include "utils/abstract_plugin.hpp"
#include "utils/singleton.hpp"

#include "cpucounters.h"

namespace opossum {

struct PcmResult {
  double qpi_to_mc_traffic_ratio = 0.0;
  uint64_t bytes_read_from_mc = 0;
  std::vector<std::vector<double>> qpi_link_utilization_in = {};
  std::vector<std::vector<double>> qpi_link_utilization_out = {};
};

class PcmPlugin : public AbstractPlugin, public Singleton<PcmPlugin> {
 public:
  PcmPlugin();

  const std::string description() const final;

  void start() final;

  void stop() final;

  void register_listenable(std::shared_ptr<Listenable> listenable);

 private:

  void _start_counting(const nlohmann::json &payload);

  void _stop_counting(const nlohmann::json &payload);

  PcmResult _get_result(SystemCounterState before, SystemCounterState after);

 private:
  PCM * _pcm;
  uint32_t _number_of_sockets;
  uint64_t _links_per_socket;
  SystemCounterState _system_counter_state_before;
  SystemCounterState _system_counter_state_after;
  nlohmann::json _results;
  size_t _current_item_id;
};

}  // namespace opossum
