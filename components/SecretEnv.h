#ifndef CCONFD_SECRETENV_H
#define CCONFD_SECRETENV_H

#include "common/Debug.h"
#include "common/Logger.h"
#include "interfaces/Secret.h"

#include <string>

class SecretEnv : public Secret
{
public:
  SecretEnv(const std::string& name,
            const std::string& path);

  virtual void add(const nlohmann::json& value) override;
  bool read();
  virtual bool get(nlohmann::json& value) const override;
  virtual void stamp(const std::chrono::seconds& duration) override;
  virtual void stamp(
    const std::chrono::system_clock::time_point& when,
    const std::chrono::system_clock::time_point& until) override;
  virtual bool isValid() const override;

private:
  std::string name;
  std::string path;
  nlohmann::json value;
};

#endif // CCONFD_SECRETENV_H