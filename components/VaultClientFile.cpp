#include "VaultClientFile.h"

using json = nlohmann::json;

VaultClientFile::VaultClientFile()
{
}

bool
VaultClientFile::logIn()
{
	return true;
}

bool
VaultClientFile::get(const std::string& name,
                     const std::string& path,
                     nlohmann::json& reply)
{
  // try to get a valid cached value
  json value;
  if (getSecret(name, value)) {
    SPDLOG_DEBUG("Vault File GET (cached): {0}\n{1}", path, value.dump(2));
    reply = value;
    return true;
  } else {
    secrets[name] = std::make_unique<SecretFile>(name, path);
    return secrets[name]->get(reply);
  }

  return addSecret(name, path, reply);
}

bool
VaultClientFile::put(const std::string& path,
                     const nlohmann::json& args,
                     nlohmann::json& reply)
{
	return false;
}

bool
VaultClientFile::addSecret(const std::string& name,
                           const std::string& path,
                           const nlohmann::json& value)
{
	secrets[name] = std::make_unique<SecretFile>(name, path);
  // add secret value in cache
  secrets[name]->add(value);
  return true;
}

bool
VaultClientFile::getSecret(const std::string& name, nlohmann::json& value) const
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
getVaultClientFileComponent()
{
  return fruit::createComponent()
    .bind<VaultClient, VaultClientFile>();
}