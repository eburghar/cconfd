#include "SecretLeased.h"
#include "LeaseSystemClock.h"

#include <unordered_map>

SecretLeased::SecretLeased(Lease* lease,
                           const std::string& name,
                           const std::string& path,
                           const std::string& lease_id,
                           const std::chrono::system_clock::time_point& when,
                           const std::chrono::system_clock::time_point& until)
  : lease(lease)
  , name(name)
  , path(path)
  , lease_id(lease_id)
{
  lease->start(when, until);
}

void
SecretLeased::add(const nlohmann::json& value)
{
  this->value = value;
}

bool
SecretLeased::get(nlohmann::json& value) const
{
  // return value if secret is valid
  if (isValid()) {
    value = this->value;
    return true;
  }
  value.clear();
  return false;
}

void
SecretLeased::stamp(const std::chrono::seconds& duration)
{
  lease->start(duration);
}

void
SecretLeased::stamp(const std::chrono::system_clock::time_point& when,
                    const std::chrono::system_clock::time_point& until)
{
  lease->start(when, until);
}

bool
SecretLeased::isValid() const
{
  return lease->isValid();
}

fruit::Component<SecretFactory>
getSecretLeasedComponent()
{
  return fruit::createComponent()
    .install(getLeaseSystemClockComponent)
    .bind<Secret, SecretLeased>();
}