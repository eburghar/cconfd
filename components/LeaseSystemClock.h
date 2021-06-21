#ifndef CCONFD_LEASESYSTEMCLOCK_H
#define CCONFD_LEASESYSTEMCLOCK_H

#include "common/Debug.h"
#include "common/Logger.h"
#include "interfaces/Lease.h"

#include <fruit/fruit.h>

class LeaseSystemClock : public Lease
{
public:
  INJECT(LeaseSystemClock());

  virtual void start(const std::chrono::system_clock::time_point& when,
                     const std::chrono::seconds& duration) override;
  virtual void start(
    const std::chrono::system_clock::time_point& when,
    const std::chrono::system_clock::time_point& until) override;
  virtual void start(
    const std::string& when,
    const std::string& until) override;
  virtual void start(const std::chrono::seconds& duration) override;
  virtual void extend(const std::chrono::seconds& duration) override;
  virtual void end(const std::chrono::system_clock::time_point& until) override;
  virtual bool isValid() override;

private:
  std::chrono::system_clock::time_point when;
  std::chrono::system_clock::time_point until;
  bool started;
};

fruit::Component<Lease>
getLeaseSystemClockComponent();

#endif // CCONFD_LEASESYSTEMCLOCK_H