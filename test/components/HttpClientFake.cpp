#include "HttpClientFake.h"

HttpClientFake* httpClientFake;

HttpClientFake::HttpClientFake(Login* login, const std::string& cacert)
{
  // be carefull of dangling pointer
  httpClientFake = this;
}

int
HttpClientFake::getReply(
  std::unordered_map<std::string, nlohmann::json>& replies,
  const std::string& url,
  std::string& readBuffer)
{
  auto reply = replies.find(url);
  if (reply != replies.end()) {
    readBuffer = reply->second.dump();
    return 0;
  }
  return 1;
}

int
HttpClientFake::get(const std::string& url,
                    const std::unordered_map<std::string, std::string>* headers,
                    long& httpCode,
                    std::string& readBuffer)
{
  return getReply(get_replies, url, readBuffer);
}

int
HttpClientFake::post(
  const std::string& url,
  const std::unordered_map<std::string, std::string>* headers,
  const std::string& value,
  long& httpCode,
  std::string& readBuffer)
{
  return getReply(post_replies, url, readBuffer);
}

void
HttpClientFake::addReply(
  std::unordered_map<std::string, nlohmann::json>& replies,
  const std::string& path,
  const nlohmann::json& reply)
{
  SPDLOG_DEBUG("addReply {}", path);
  replies.emplace(path, reply);
}

void
HttpClientFake::addGetReply(const std::string& path,
                            const nlohmann::json& reply)
{
  addReply(get_replies, path, reply);
}

void
HttpClientFake::addPostReply(const std::string& path,
                             const nlohmann::json& reply)
{
  addReply(post_replies, path, reply);
}

void
HttpClientFake::removeReply(
  std::unordered_map<std::string, nlohmann::json>& replies,
  const std::string& path)
{
  SPDLOG_DEBUG("removeReply {}", path);
  replies.erase(path);
}

void
HttpClientFake::removeGetReply(const std::string& path)
{
  removeReply(get_replies, path);
}

void
HttpClientFake::removePostReply(const std::string& path)
{
  removeReply(post_replies, path);
}

fruit::Component<HttpClientFactory>
getHttpClientFakeComponent()
{
  return fruit::createComponent()
    .install(getLoginLeasedComponent)
    .bind<HttpClient, HttpClientFake>();
}