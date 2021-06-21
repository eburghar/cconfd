#ifndef CCONFD_LOGIN_H
#define CCONFD_LOGIN_H

#include <chrono>
#include <string>

class Login
{
public:
  virtual bool setToken(const std::string& token,
                        const std::chrono::seconds& duration) = 0;
  virtual bool setToken(const std::string& token,
                        const std::chrono::system_clock::time_point& when,
                        const std::chrono::system_clock::time_point& until) = 0;
  virtual bool getToken(std::string& token) const = 0;
  virtual bool isValid() const = 0;
};

#endif // CCONFD_LOGIN_H