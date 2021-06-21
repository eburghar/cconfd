#include "VaultClientEnv.h"

using json = nlohmann::json;

VaultClientEnv::VaultClientEnv()
{
}

bool
VaultClientEnv::logIn()
{
  return true;
}

bool
VaultClientEnv::get(const std::string& name,
                     const std::string& path,
                     nlohmann::json& reply)
{
  // try to get a valid cached value
  json value;
  if (getSecret(name, value)) {
    SPDLOG_DEBUG("Vault Env GET (cached): {0}\n{1}", name, value.dump(2));
    reply = value;
    return true;
  } else {
    secrets[name] = std::make_unique<SecretEnv>(name, path);
    return secrets[name]->get(reply);
  }

  return addSecret(name, path, reply);
}

bool
VaultClientEnv::put(const std::string& path,
                     const nlohmann::json& args,
                     nlohmann::json& reply)
{
  return false;
}

bool
VaultClientEnv::addSecret(const std::string& name,
                           const std::string& path,
                           const nlohmann::json& value)
{
  secrets[name] = std::make_unique<SecretEnv>(name, path);
  // add secret value in cache
  secrets[name]->add(value);
  return true;
}

bool
VaultClientEnv::getSecret(const std::string& name, nlohmann::json& value) const
{
  const auto i = secrets.find(name);
  if (i != secrets.end()) {
    if (i->second->isValid())
      return i->second->get(value);
  }
  value.clear();
  return false;
}

fruit::Component<VaultClient>
getVaultClientEnvComponent()
{
  return fruit::createComponent()
    .bind<VaultClient, VaultClientEnv>();
}