#ifndef CCONFD_CONFIGDIRJSON_H
#define CCONFD_CONFIGDIRJSON_H

#include "common/Debug.h"
#include "common/Logger.h"
#include "components/ConfigVault.h"
#include "interfaces/ConfigDir.h"

#include <fruit/fruit.h>
#include <nlohmann/json.hpp>
#include <unordered_map>

class ConfigDirJson : public ConfigDir
{
public:
  INJECT(ConfigDirJson(ConfigVaultFactory configFactory));
  bool generateConfigFiles(const std::string& dir,
                           const std::string& url,
                           const std::string& token,
                           const std::string& cacert,
                           int sleep,
                           int readiness) override;

  bool generateCachedConfigFiles(const nlohmann::json& j,
                                 const std::string& key,
                                 const std::string url,
                                 const std::string& token,
                                 const std::string& cacert,
                                 int sleep);

private:
  ConfigVaultFactory configFactory;
  std::unordered_map<std::string, std::unique_ptr<Config>> configs;
};

fruit::Component<ConfigDir>
getConfigDirJsonComponent();

#endif // CCONFD_CONFIGDIRJSON_H