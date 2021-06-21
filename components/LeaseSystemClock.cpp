#include "LeaseSystemClock.h"

#include <date/date.h>
#include <sstream>

LeaseSystemClock::LeaseSystemClock()
  : started(false)
{}

void
LeaseSystemClock::start(const std::chrono::system_clock::time_point& when_, const std::chrono::seconds& duration)
{
  when = when_;
  extend(duration);
}

void LeaseSystemClock::start(const std::chrono::system_clock::time_point& when_, const std::chrono::system_clock::time_point& until_)
{
  when = when_;
  until = until_;
  started = true;
}

void LeaseSystemClock::start(const std::string& when_s, const std::string& until_s)
{
  std::istringstream{when_s} >> date::parse("%FT%TZ", when);
  std::istringstream{until_s} >> date::parse("%FT%TZ", until);
  started = true;
}

void
LeaseSystemClock::start(const std::chrono::seconds& duration)
{
  std::chrono::system_clock::time_point when_ = std::chrono::system_clock::now();
  start(when_, duration);
}

void
LeaseSystemClock::extend(const std::chrono::seconds& duration)
{
  until = when + duration;
  started = true;
}

void
LeaseSystemClock::end(const std::chrono::system_clock::time_point& until_)
{
  until = until_;
  started = true;
}

bool
LeaseSystemClock::isValid()
{
  // invalid if not started
  if (not started)
    return false;
  // always valid when there is no duration (when == until)
  if (when == until)
    return true;
  // otherwise test against present
  return until > std::chrono::system_clock::now();
}

fruit::Component<Lease>
getLeaseSystemClockComponent()
{
  return fruit::createComponent().bind<Lease, LeaseSystemClock>();
}