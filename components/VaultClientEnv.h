#ifndef CCONFD_VAULTCLIENTENV_H
#define CCONFD_VAULTCLIENTENV_H

#include "SecretEnv.h" // SecretEnvFactory
#include "common/Debug.h"
#include "common/Logger.h"
#include "interfaces/VaultClient.h"

#include <fruit/fruit.h>
#include <unordered_map>

class VaultClientEnv : public VaultClient
{
public:
  INJECT(VaultClientEnv());
  virtual bool logIn() override;
  virtual bool get(const std::string& name,
                   const std::string& path,
                   nlohmann::json& reply) override;
  virtual bool put(const std::string& path,
                   const nlohmann::json& args,
                   nlohmann::json& reply) override;
  bool addSecret(const std::string& name,
                 const std::string& path,
                 const nlohmann::json& value);
  bool getSecret(const std::string& name, nlohmann::json& value) const;
  //virtual ~VaultClientEnv() = default;

private:
  std::unordered_map<std::string, std::unique_ptr<Secret>> secrets;
};

fruit::Component<VaultClient>
getVaultClientEnvComponent();

#endif //CCONFD_VAULTCLIENTENV_H