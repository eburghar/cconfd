#ifndef CCONFD_SECRETS_H
#define CCONFD_SECRETS_H

#include <chrono>
#include <nlohmann/json.hpp>
#include <string>

class Secret
{
public:
  virtual void add(const nlohmann::json& value) = 0;
  virtual bool get(nlohmann::json& value) const = 0;
  virtual void stamp(const std::chrono::seconds& duration) = 0;
  virtual void stamp(const std::chrono::system_clock::time_point& when,
                     const std::chrono::system_clock::time_point& until) = 0;
  virtual bool isValid() const = 0;
  virtual ~Secret() = default;
};

#endif // SVCONFD_SECRETS_H