#ifndef CCONFD_HTTPCLIENTFAKE_H
#define CCONFD_HTTPCLIENTFAKE_H

#include "common/Logger.h"
#include "components/LoginLeased.h"
#include "components/HttpClientCurl.h"
#include "interfaces/Login.h"

#include <fruit/fruit.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <unordered_map>

// HACK to have direct access to injected HttpClientFake
class HttpClientFake;
extern HttpClientFake* httpClientFake;

class HttpClientFake : public HttpClient
{
public:
  INJECT(HttpClientFake(Login* login, ASSISTED(const std::string&) cacert));
  int getReply(std::unordered_map<std::string, nlohmann::json>& replies,
               const std::string& url,
               std::string& readBuffer);
  int get(const std::string& url,
          const std::unordered_map<std::string, std::string>* headers,
          long& httpCode,
          std::string& readBuffer) override;
  int post(const std::string& url,
           const std::unordered_map<std::string, std::string>* headers,
           const std::string& value,
           long& httpCode,
           std::string& readBuffer) override;

  void addReply(std::unordered_map<std::string, nlohmann::json>& replies,
                const std::string& path,
                const nlohmann::json& reply);
  void addGetReply(const std::string& path, const nlohmann::json& reply);
  void addPostReply(const std::string& path, const nlohmann::json& reply);
  void removeReply(std::unordered_map<std::string, nlohmann::json>& replies,
    const std::string& path);
  void removeGetReply(const std::string& path);
  void removePostReply(const std::string& path);

private:
  std::unordered_map<std::string, nlohmann::json> get_replies;
  std::unordered_map<std::string, nlohmann::json> post_replies;
};

fruit::Component<HttpClientFactory>
getHttpClientFakeComponent();

#endif // CCONFD_HTTPCLIENTFAKE_H