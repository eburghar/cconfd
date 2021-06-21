#include "SecretEnv.h"
#include "common/Exceptions.h"

using json = nlohmann::json;

SecretEnv::SecretEnv(const std::string& name,
                     const std::string& path)
  : name(name)
  , path(path)
{
  this->read();
}

void
SecretEnv::add(const nlohmann::json& value)
{
  this->value = value;
}

bool
SecretEnv::read()
{
  // Read environment variable
  auto value = std::getenv(path.c_str());
  if (value == NULL)
    throw exception("Environment variable \"" + path + "\" not defined");
  SPDLOG_DEBUG("Variable {0}={0}", name, value);
  this->add(json(value));
  return true;
}

bool
SecretEnv::get(nlohmann::json& value) const
{
  // return value if secret is valid
  if (isValid()) {
    SPDLOG_DEBUG("Get variable: {0}: ", this->value.dump());
    value = this->value;
    return true;
  }
  value.clear();
  return false;
}

void
SecretEnv::stamp(const std::chrono::seconds& duration)
{
}

void
SecretEnv::stamp(const std::chrono::system_clock::time_point& when,
                    const std::chrono::system_clock::time_point& until)
{
}

bool
SecretEnv::isValid() const
{
  // TODO: return valid if filestamp didn't change
  return true;
}