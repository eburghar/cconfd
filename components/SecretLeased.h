#ifndef CCONFD_SECRETSLEASED_H
#define CCONFD_SECRETSLEASED_H

#include "common/Debug.h"
#include "common/Logger.h"
#include "interfaces/Lease.h"
#include "interfaces/Secret.h"

#include <fruit/fruit.h>
#include <string>

class SecretLeased : public Secret
{
public:
  INJECT(SecretLeased(Lease* lease,
                      ASSISTED(const std::string&) name,
                      ASSISTED(const std::string&) path,
                      ASSISTED(const std::string&) lease_id,
                      ASSISTED(const std::chrono::system_clock::time_point&)
                        when,
                      ASSISTED(const std::chrono::system_clock::time_point&)
                        until));
  virtual void add(const nlohmann::json& value) override;
  virtual bool get(nlohmann::json& value) const override;
  virtual void stamp(const std::chrono::seconds& duration) override;
  virtual void stamp(
    const std::chrono::system_clock::time_point& when,
    const std::chrono::system_clock::time_point& until) override;
  virtual bool isValid() const override;

private:
  Lease* lease;
  std::string name;
  std::string path;
  std::string lease_id;
  nlohmann::json value;
};

using SecretFactory = std::function<std::unique_ptr<Secret>(
  const std::string& name,
  const std::string& path,
  const std::string& lease_id,
  const std::chrono::system_clock::time_point& when,
  const std::chrono::system_clock::time_point& until)>;

fruit::Component<SecretFactory>
getSecretLeasedComponent();

#endif // CCONFD_SECRETSLEASED_H