#ifndef CCONFD_LEASE_H
#define CCONFD_LEASE_H

#include <chrono>

class Lease
{
public:
  virtual void start(const std::chrono::system_clock::time_point& when,
                     const std::chrono::seconds& duration) = 0;
  virtual void start(const std::chrono::system_clock::time_point& when,
                     const std::chrono::system_clock::time_point& until) = 0;
  virtual void start(const std::string& when, const std::string& until) = 0;
  virtual void start(const std::chrono::seconds& duration) = 0;
  virtual void extend(const std::chrono::seconds& duration) = 0;
  virtual void end(const std::chrono::system_clock::time_point& until) = 0;
  virtual bool isValid() = 0;
};

#endif // CCONFD_LEASE_H