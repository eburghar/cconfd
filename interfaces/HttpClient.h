#ifndef CCONFD_HTTPCLIENT_H
#define CCONFD_HTTPCLIENT_H

#include <string>
#include <unordered_map>

class HttpClient
{
public:
  virtual int get(const std::string& url,
                  const std::unordered_map<std::string, std::string>* headers,
                  long& httpCode,
                  std::string& readBuffer) = 0;
  virtual int post(const std::string& url,
                   const std::unordered_map<std::string, std::string>* headers,
                   const std::string& value,
                   long& httpCode,
                   std::string& readBuffer) = 0;
  virtual ~HttpClient() = default;
};

#endif // CCONFD_HTTPCLIENT_H