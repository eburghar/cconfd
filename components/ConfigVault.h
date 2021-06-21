#ifndef CCONFD_CONFIGVAULT_H
#define CCONFD_CONFIGVAULT_H

#include "VaultClientHttp.h" // VaultClientFactory
#include "VaultClientFile.h" // VaultClientFileFactory
#include "VaultClientEnv.h" // VaultClientEnvFactory
#include "common/Debug.h"
#include "common/Logger.h"
#include "interfaces/Config.h"
#include "interfaces/Engine.h"

#include <filesystem>

struct config
{
  std::string tmpl;
  std::string dir;
  std::string user;
  std::string role;
  std::string mode;
  std::string cmd;
  std::map<std::string, std::string> secrets;
  std::vector<std::string> paths;
};

class ConfigVault : public Config
{
public:
  INJECT(ConfigVault(VaultClientFactory vaultClientFactory,
                     Engine* engine,
                     ASSISTED(const nlohmann::json&) cfg,
                     ASSISTED(const std::string&) url,
                     ASSISTED(const std::string&) token,
                     ASSISTED(const std::string&) cacert,
                     ASSISTED(int) sleep));
  std::string dump() const override;
  bool generateConfigFiles() override;
  std::filesystem::perms fsPerms() const;

private:
  Engine* engine;
  config cfg;
  std::unique_ptr<VaultClient> vault;
  std::unique_ptr<VaultClient> file;
  std::unique_ptr<VaultClient> env;
};

using ConfigVaultFactory =
  std::function<std::unique_ptr<Config>(const nlohmann::json& cfg,
                                        const std::string& url,
                                        const std::string& token,
                                        const std::string& cacert,
                                        int sleep)>;

fruit::Component<ConfigVaultFactory>
getConfigVaultComponent();

#endif // CCONFD_CONFIGVAULT_H