#include "SecretFile.h"
#include "common/Exceptions.h"
#include "common/Utils.h"

#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;
using json = nlohmann::json;

static std::string
readFile(std::ifstream& in)
{
  return std::string(std::istreambuf_iterator<char>(in),
                     std::istreambuf_iterator<char>());
}

SecretFile::SecretFile(const std::string& name,
                       const std::string& path)
  : name(name)
  , path(path)
{
  this->read();
}

void
SecretFile::add(const nlohmann::json& value)
{
  this->value = value;
}

bool
SecretFile::read()
{
  // Read secret file
  if (!fs::exists(fs::path(path)))
    throw file_exception("File \"" + path + "\" doesn't exist");
  std::ifstream secret_file(path);
  if (secret_file.fail())
    throw file_exception("Unable to open \"" + path + '"');
  std::string value = readFile(secret_file);
  trim(value);
  SPDLOG_DEBUG("File content: {0}", value);
  this->add(json(value));
  return true;
}

bool
SecretFile::get(nlohmann::json& value) const
{
  // return value if secret is valid
  if (isValid()) {
    SPDLOG_DEBUG("Get File: {0}: ", this->value.dump());
    value = this->value;
    return true;
  }
  value.clear();
  return false;
}

void
SecretFile::stamp(const std::chrono::seconds& duration)
{
}

void
SecretFile::stamp(const std::chrono::system_clock::time_point& when,
                    const std::chrono::system_clock::time_point& until)
{
}

bool
SecretFile::isValid() const
{
  // TODO: return valid if filestamp didn't change
  return true;
}