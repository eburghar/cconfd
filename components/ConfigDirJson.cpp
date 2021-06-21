#include "ConfigDirJson.h"
#include "common/Utils.h"
#include "components/ConfigVault.h"

#include <filesystem>
#include <fstream>
#include <cerrno>

namespace fs = std::filesystem;
using json = nlohmann::json;

ConfigDirJson::ConfigDirJson(ConfigVaultFactory configFactory)
  : configFactory(configFactory)
{}

bool
ConfigDirJson::generateConfigFiles(const std::string& dir,
                                   const std::string& url,
                                   const std::string& token,
                                   const std::string& cacert,
                                   int sleep,
                                   int readiness)
{
  for (auto& p : fs::directory_iterator(dir)) {
    auto path = p.path().string();
    if (endsWith(path, ".json")) {
      // TODO: test if file has changed on disk and invalidate cache
      SPDLOG_INFO("Reading \"{}\"", path);
      std::ifstream i(path);
      json j;
      i >> j;
      for (json::iterator it = j.begin(); it != j.end(); ++it) {
        SPDLOG_INFO("  Generating \"{}\" configuration files", it.key());
        generateCachedConfigFiles(j, it.key(), url, token, cacert, sleep);
      }
    }
  }

  // s6-readiness
  if (readiness > 0) {
    if (fcntl(readiness, F_GETFL) != -1 || errno != EBADF)
      write(readiness, "\n", 1);
    else
      SPDLOG_ERROR("Error writing to \"{}\"", readiness);
  }

  return true;
}

bool
ConfigDirJson::generateCachedConfigFiles(const json& j,
                                         const std::string& key,
                                         const std::string url,
                                         const std::string& token,
                                         const std::string& cacert,
                                         int sleep)
{
  const auto i = configs.find(key);
  if (i != configs.end()) {
    return i->second->generateConfigFiles();
  } else {
    configs[key] = configFactory(j[key], url, token, cacert, sleep);
    return configs[key]->generateConfigFiles();
  }
}

fruit::Component<ConfigDir>
getConfigDirJsonComponent()
{
  return fruit::createComponent()
    .install(getConfigVaultComponent)
    .bind<ConfigDir, ConfigDirJson>();
}