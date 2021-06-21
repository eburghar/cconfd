#ifndef CCONFD_CONFIG_H
#define CCONFD_CONFIG_H

#include <string>

class Config
{
public:
  virtual std::string dump() const = 0;
  virtual bool generateConfigFiles() = 0;
  virtual ~Config() = default;
};

#endif // CCONFD_CONFIG_H