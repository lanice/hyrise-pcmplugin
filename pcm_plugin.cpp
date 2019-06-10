#include "pcm_plugin.hpp"

#include "benchmark_runner.hpp"

namespace opossum {

PcmPlugin::PcmPlugin() :
  _pcm(nullptr),
  _number_of_sockets(1),
  _links_per_socket(1),
  _system_counter_state_before(),
  _system_counter_state_after(),
  _results(nlohmann::json{{"benchmarks", nlohmann::json::array()}}){}

const std::string PcmPlugin::description() const { return "This is the Hyrise PcmPlugin"; }

void PcmPlugin::start() {
  std::cout << "Initializing PCM lib" << std::endl;
  _pcm = PCM::getInstance();
  PCM::ErrorCode returnResult = _pcm->program();
  if (returnResult != PCM::Success){
    std::cerr << "PCM couldn't start" << std::endl;
    std::cerr << "Error code: " << returnResult << std::endl;
    /*_pcm->resetPMU();*/
    exit(1);
  }
  _number_of_sockets = _pcm->getNumSockets();
  _links_per_socket = _pcm->getQPILinksPerSocket();
}

void PcmPlugin::stop() {
  std::cout << "Cleaning up PCM lib" << std::endl;
  _pcm->cleanup();
}

void PcmPlugin::register_listenable(std::shared_ptr<Listenable> listenable) {
  listenable->add_listener(Event::ItemRunStarted, [&](const nlohmann::json &payload){this->_start_counting(payload);});
  listenable->add_listener(Event::ItemRunFinished, [&](const nlohmann::json &payload){this->_stop_counting(payload);});

  Assert(std::dynamic_pointer_cast<BenchmarkRunner>(listenable), "Can register PcmPlugin only with BenchmarkRunner as Listenable.");
  listenable->add_listener(Event::CreateReport, [&](const nlohmann::json &payload){
    std::dynamic_pointer_cast<BenchmarkRunner>(listenable)->add_to_json_report("PcmPlugin", _results);
  });
}

void PcmPlugin::_start_counting(const nlohmann::json &payload) {
  _current_item_id = payload["item_id"];
  _system_counter_state_before = getSystemCounterState();
}

void PcmPlugin::_stop_counting(const nlohmann::json &payload) {
  _system_counter_state_after = getSystemCounterState();

  Assert(payload["item_id"] == _current_item_id, "Whoopsie");

  auto result = _get_result(_system_counter_state_before, _system_counter_state_after);

  auto result_json = nlohmann::json{{"name", payload["item_name"]}};
  result_json.push_back({"qpi_to_mc_traffic_ratio", result.qpi_to_mc_traffic_ratio});
  result_json.push_back({"bytes_read_from_mc", result.bytes_read_from_mc});
  result_json.push_back({"qpi_link_utilization_in", result.qpi_link_utilization_in});
  result_json.push_back({"qpi_link_utilization_out", result.qpi_link_utilization_out});
  _results["benchmarks"].push_back(result_json);
}

PcmResult PcmPlugin::_get_result(SystemCounterState before, SystemCounterState after) {
  auto result = PcmResult();
  for (auto socket = uint32_t{0}; socket < _number_of_sockets; ++socket) {
    auto qpi_link_utilization_in = std::vector<double>{};
    auto qpi_link_utilization_out = std::vector<double>{};

    for (auto link = uint32_t{0}; link < _links_per_socket; ++link) {
      qpi_link_utilization_in.push_back(getIncomingQPILinkUtilization(socket, link, before, after));
      qpi_link_utilization_out.push_back(getOutgoingQPILinkUtilization(socket, link, before, after));
    }

    result.qpi_link_utilization_in.push_back(qpi_link_utilization_in);
    result.qpi_link_utilization_out.push_back(qpi_link_utilization_out);
  }

  result.qpi_to_mc_traffic_ratio = getQPItoMCTrafficRatio(before, after);
  result.bytes_read_from_mc = getBytesReadFromMC(before, after);
  return result;
}

EXPORT_PLUGIN(PcmPlugin)

}  // namespace opossum
