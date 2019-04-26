#pragma once

#include "storage/storage_manager.hpp"
#include "utils/abstract_plugin.hpp"
#include "utils/singleton.hpp"

namespace opossum {

class PcmPlugin : public AbstractPlugin, public Singleton<PcmPlugin> {
 public:
  PcmPlugin() : sm(StorageManager::get()) {}

  const std::string description() const final;

  void start() final;

  void stop() final;

  StorageManager& sm;
};

}  // namespace opossum
