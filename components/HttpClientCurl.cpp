#include "HttpClientCurl.h"
#include "LoginLeased.h"
#include "common/Logger.h"
#include "common/Utils.h"

#include <curl/curl.h>

static size_t
writeCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
  ((std::string*)userp)->append((char*)contents, size * nmemb);
  return size * nmemb;
}

HttpClientCurl::HttpClientCurl(Login* login, const std::string& cacert)
  : login(login)
  , cacert(cacert)
{}

int
HttpClientCurl::get(const std::string& url,
                    const std::unordered_map<std::string, std::string>* headers,
                    long& httpCode,
                    std::string& readBuffer)
{
  auto res = CURLE_FAILED_INIT;
  auto curl = curl_easy_init();

  if (curl) {
    struct curl_slist* chunk = nullptr;
    if (headers != nullptr) {
      for (auto const& hdr : *headers)
        chunk =
          curl_slist_append(chunk, (hdr.first + ": " + hdr.second).c_str());
      curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
    }

    if (startsWith(url, "https://"))
    {
      curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER , 0);
      curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST , 1);
      if (cacert != "")
      {
        SPDLOG_DEBUG("using https and cacert {}", cacert);
        curl_easy_setopt(curl, CURLOPT_CAPATH, cacert);
      }
    }
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    if (debug) {
      curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    }

    res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, httpCode);
    if (res != CURLE_OK) {
      SPDLOG_ERROR("GET \"{0}\" failed\n{1}", url, curl_easy_strerror(res));
    }

    curl_easy_cleanup(curl);
    curl_slist_free_all(chunk);
  }

  return res;
}

int
HttpClientCurl::post(
  const std::string& url,
  const std::unordered_map<std::string, std::string>* headers,
  const std::string& value,
  long& httpCode,
  std::string& readBuffer)
{
  auto res = CURLE_FAILED_INIT;
  auto curl = curl_easy_init();

  if (curl) {
    struct curl_slist* chunk = nullptr;
    if (headers != nullptr) {
      for (auto const& hdr : *headers)
        chunk =
          curl_slist_append(chunk, (hdr.first + ": " + hdr.second).c_str());
      curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
    }

    if (startsWith(url, "https://"))
    {
      curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER , 0);
      curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST , 1);
      if (cacert != "")
      {
        SPDLOG_DEBUG("using https and cacert {}", cacert);
        curl_easy_setopt(curl, CURLOPT_CAPATH, cacert);
      }
    }
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, value.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    if (debug) {
      curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    }

    res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, httpCode);

    if (res != CURLE_OK) {
      SPDLOG_ERROR("POST {0} failed\n{1}", url, curl_easy_strerror(res));
    }

    curl_easy_cleanup(curl);
    curl_slist_free_all(chunk);
  }

  return res;
}

fruit::Component<HttpClientFactory>
getHttpClientCurlComponent()
{
  return fruit::createComponent()
    .install(getLoginLeasedComponent)
    .bind<HttpClient, HttpClientCurl>();
}