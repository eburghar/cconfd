#ifndef CCONFD_VAULTCLIENT_H
#define CCONFD_VAULTCLIENT_H

#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>

class VaultClient
{
public:
  virtual bool logIn() = 0;
  virtual bool get(const std::string& name,
                   const std::string& path,
                   nlohmann::json& reply) = 0;
  virtual bool put(const std::string& path,
                   const nlohmann::json& args,
                   nlohmann::json& reply) = 0;
  virtual ~VaultClient() = default;
};

#endif // CCONFD_VAULTCLIENT_H