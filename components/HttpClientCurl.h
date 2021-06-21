#ifndef CCONFD_HTTPCLIENTCURL_H
#define CCONFD_HTTPCLIENTCURL_H

#include "common/Debug.h"
#include "common/Logger.h"
#include "interfaces/HttpClient.h"
#include "interfaces/Login.h"

#include <fruit/fruit.h>

class HttpClientCurl : public HttpClient
{
public:
  INJECT(HttpClientCurl(Login* login, ASSISTED(const std::string&) cacert));
  virtual int get(const std::string& url,
                  const std::unordered_map<std::string, std::string>* headers,
                  long& httpCode,
                  std::string& readBuffer) override;
  virtual int post(const std::string& url,
                   const std::unordered_map<std::string, std::string>* headers,
                   const std::string& value,
                   long& httpCode,
                   std::string& readBuffer) override;

private:
  Login* login;
  std::string cacert;
  bool debug = false;
};

using HttpClientFactory =
  std::function<std::unique_ptr<HttpClient>(const std::string& cacert)>;

fruit::Component<HttpClientFactory>
getHttpClientCurlComponent();

#endif // CCONFD_HTTPCLIENTCURL_H