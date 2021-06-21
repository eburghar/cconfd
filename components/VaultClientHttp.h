#ifndef CCONFD_VAULTCLIENTHTTP_H
#define CCONFD_VAULTCLIENTHTTP_H

#include "SecretLeased.h" // SecretFactory
#include "common/Debug.h"
#include "common/Logger.h"
#include "components/HttpClientCurl.h"
#include "interfaces/Login.h"
#include "interfaces/VaultClient.h"

#include <fruit/fruit.h>
#include <unordered_map>

class VaultClientHttp : public VaultClient
{
public:
  INJECT(VaultClientHttp(HttpClientFactory httpClientFactory,
                         SecretFactory secretFactory,
                         Login* login,
                         ASSISTED(const std::string&) url,
                         ASSISTED(const std::string&) role,
                         ASSISTED(const std::string&) token_path,
                         ASSISTED(const std::string&) cacert,
                         ASSISTED(int) sleep));
  virtual bool logIn() override;
  virtual bool get(const std::string& name,
                   const std::string& path,
                   nlohmann::json& reply) override;
  virtual bool put(const std::string& path,
                   const nlohmann::json& args,
                   nlohmann::json& reply) override;

  bool addSecret(const std::string& name,
                 const std::string& path,
                 const std::string& lease_id,
                 const std::chrono::seconds& duration,
                 const nlohmann::json& value);
  bool getSecret(const std::string& name, nlohmann::json& value) const;
  bool getLease(const std::string& lease_id,
                std::chrono::system_clock::time_point& when,
                std::chrono::system_clock::time_point& until);
  bool hasErrors(const nlohmann::json& j, std::string& errors);
  int getSafeDuration(int duration);

private:
  std::string vaultUrl(const std::string& path) const;

  HttpClientFactory httpClientFactory;
  std::unique_ptr<HttpClient> httpClient;
  SecretFactory secretFactory;
  Login* login;
  std::string url;
  std::string role;
  std::string token_path;
  int sleep;
  std::unordered_map<std::string, std::unique_ptr<Secret>> secrets;
};

using VaultClientFactory =
  std::function<std::unique_ptr<VaultClient>(const std::string& url,
                                             const std::string& role,
                                             const std::string& token_path,
                                             const std::string& cacert,
                                             int sleep)>;

fruit::Component<VaultClientFactory>
getVaultClientHttpComponent();

#endif // CCONFD_VAULTCLIENTHTTP_H