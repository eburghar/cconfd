#include "VaultClientHttp.h"
#include "HttpClientCurl.h"
#include "common/Exceptions.h"
#include "components/LoginLeased.h"

#include <curl/curl.h>
#include <date/date.h>
#include <filesystem>
#include <fruit/fruit.h>
#include <fstream>
#include <iostream>
#include <string>

namespace fs = std::filesystem;
using json = nlohmann::json;

static std::string
readFile(std::ifstream& in)
{
  return std::string(std::istreambuf_iterator<char>(in),
                     std::istreambuf_iterator<char>());
}

VaultClientHttp::VaultClientHttp(HttpClientFactory httpClientFactory,
                                 SecretFactory secretFactory,
                                 Login* login,
                                 const std::string& url,
                                 const std::string& role,
                                 const std::string& token_path,
                                 const std::string& cacert,
                                 int sleep)
  : httpClientFactory(httpClientFactory)
  , httpClient(httpClientFactory(cacert))
  , secretFactory(secretFactory)
  , login(login)
  , url(url)
  , role(role)
  , token_path(token_path)
  , sleep(sleep)
{}

bool
VaultClientHttp::logIn()
{
  if (login->isValid()) {
    SPDLOG_DEBUG("Already logged in");
    return true;
  }

  std::string readBuffer;
  long httpCode = 0;
  const std::string login_err{ "Login failed" };

  // Read kubernetes token and build http headers
  if (!fs::exists(fs::path(token_path)))
    throw file_exception("File \"" + token_path + "\" doesn't exist");
  std::ifstream jwt_file(token_path);
  if (jwt_file.fail())
    throw file_exception("Unable to open \"" + token_path + '"');
  std::string jwt = readFile(jwt_file);
  std::unordered_map<std::string, std::string> args(
    { { "role", role }, { "jwt", jwt } });

  // Authenticate and extract vault token
  json j(args);
  auto path = vaultUrl("/auth/kubernetes/login");
  auto res = httpClient->post(path, nullptr, j.dump(), httpCode, readBuffer);
  if (res == CURLE_OK && readBuffer.size() != 0) {
    auto reply = json::parse(readBuffer);
    SPDLOG_DEBUG("Login reply({1}/{2})\n{0}", reply.dump(2), res, httpCode);
    // test for errors messages
    std::string errors;
    if (hasErrors(reply, errors)) {
      throw exception(login_err + " with role \"" + role + "\": " + errors);
    } else {
      // get token and duration
      std::string tk = reply["/auth/client_token"_json_pointer];
      int duration = reply["/auth/lease_duration"_json_pointer];

      if (!login->setToken(tk, std::chrono::seconds(getSafeDuration(duration))))
        throw exception(login_err + ": client_token not found in login reply");

      SPDLOG_INFO("Logged in with role \"{}\"", role);
      return true;
    }
  } else {
    throw exception(login_err + "with role \"" + role + '"');
  }

  return false;
}

int
VaultClientHttp::getSafeDuration(int duration)
{
  if (duration == 0)
    return 0;
  auto safe = duration - 2 * sleep;
  if (safe >= 0)
    return safe;

  return duration;
}

bool
VaultClientHttp::hasErrors(const nlohmann::json& reply, std::string& errors)
{
  errors.clear();
  auto const it = reply.find("errors");
  if (it != reply.end()) {
    for (const auto& e : *it)
      errors += e;
    return true;
  }
  return false;
}

bool
VaultClientHttp::get(const std::string& name,
                     const std::string& path,
                     nlohmann::json& reply)
{

  // try to get a valid cached value
  json value;
  if (getSecret(name, value)) {
    SPDLOG_DEBUG("Vault GET (cached): {0}\n{1}", path, value.dump(2));
    reply = value;
    return true;
  }

  // try to get secret from vault
  logIn();

  if (login->isValid()) {
    std::string readBuffer;
    long httpCode = 0;

    std::string token;
    if (login->getToken(token)) {
      std::unordered_map<std::string, std::string> headers(
        { { "X-Vault-Token", token } });
      SPDLOG_DEBUG("Vault GET: {}", path);
      auto res =
        httpClient->get(vaultUrl(path), &headers, httpCode, readBuffer);
      if (res == 0 && readBuffer.length() != 0) {
        reply = json::parse(readBuffer);
        SPDLOG_DEBUG(
          "Vault Reply({1}/{2})\n{0}", reply.dump(2), res, httpCode);
        std::string errors;
        if (hasErrors(reply, errors)) {
          throw exception("Vault GET \"" + path + "\": " + errors);
        }
        int duration = reply["lease_duration"];
        std::string lease_id = reply["lease_id"];
        addSecret(name,
                  path,
                  lease_id,
                  std::chrono::seconds(getSafeDuration(duration)),
                  reply);
        return true;
      }
    }
  }
  return false;
}

bool
VaultClientHttp::getLease(const std::string& lease_id,
                          std::chrono::system_clock::time_point& when,
                          std::chrono::system_clock::time_point& until)
{
  if (login->isValid() && lease_id != "") {
    std::string readBuffer;
    long httpCode = 0;

    std::string token;
    if (login->getToken(token)) {
      std::unordered_map<std::string, std::string> headers(
        { { "X-Vault-Token", token } });
      json j{ { "lease_id", lease_id } };
      auto path = vaultUrl("/sys/leases/lookup");
      auto res =
        httpClient->post(path, &headers, j.dump(), httpCode, readBuffer);
      if (res == 0 && readBuffer.length() != 0) {
        auto reply = json::parse(readBuffer);
        SPDLOG_DEBUG(
          "Vault Reply({1}/{2})\n{0}", reply.dump(2), res, httpCode);
        std::string errors;
        if (!hasErrors(reply, errors)) {
          std::string when_s = reply["/data/issue_time"_json_pointer];
          std::string until_s = reply["/data/expire_time"_json_pointer];
          std::istringstream{ when_s } >> date::parse("%FT%TZ", when);
          std::istringstream{ until_s } >> date::parse("%FT%TZ", until);
          return true;
        }
      }
    }
  }
  return false;
}

bool
VaultClientHttp::put(const std::string& path,
                     const nlohmann::json& args,
                     nlohmann::json& reply)
{
  logIn();

  if (login->isValid()) {
    std::string readBuffer;
    long httpCode = 0;

    std::string token;
    if (login->getToken(token)) {
      std::unordered_map<std::string, std::string> headers(
        { { "X-Vault-Token", token }, { "Content-Type", "application/json" } });
      SPDLOG_DEBUG("Vault PUT {0}: {1}", path, args.dump(2));
      auto res = httpClient->post(
        vaultUrl(path), &headers, args.dump(), httpCode, readBuffer);
      reply = json::parse(readBuffer);
      SPDLOG_DEBUG("Vault reply({1}/{2})\n{0}", reply.dump(2), res, httpCode);
      std::string errors;
      if (hasErrors(reply, errors)) {
        throw exception("Vault PUT \"" + path + "\": " + errors);
      }
      return true;
    }
  }
  return false;
}

bool
VaultClientHttp::getSecret(const std::string& name, nlohmann::json& value) const
{
  const auto i = secrets.find(name);
  if (i != secrets.end()) {
    if (i->second->isValid())
      return i->second->get(value);
  }
  value.clear();
  return false;
}

bool
VaultClientHttp::addSecret(const std::string& name,
                           const std::string& path,
                           const std::string& lease_id,
                           const std::chrono::seconds& duration,
                           const nlohmann::json& value)
{
  std::chrono::system_clock::time_point when{
    std::chrono::system_clock::now()
  };
  std::chrono::system_clock::time_point until{ when + duration };
  // overwrite when and until with value from lease if any
  getLease(lease_id, when, until);
  secrets[name] = secretFactory(name, path, lease_id, when, until);
  // add secret value in cache
  secrets[name]->add(value);
  return true;
}

std::string
VaultClientHttp::vaultUrl(const std::string& path) const
{
  if (path.length() == 0)
    return path;
  else {
    if (path[0] == '/')
      return url + path.substr(1);
    else
      return url + path;
  }
}

fruit::Component<VaultClientFactory>
getVaultClientHttpComponent()
{
  return fruit::createComponent()
    .install(getHttpClientCurlComponent)
    .install(getSecretLeasedComponent)
    .install(getLoginLeasedComponent)
    .bind<VaultClient, VaultClientHttp>();
}