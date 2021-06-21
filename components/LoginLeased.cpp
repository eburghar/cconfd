#include "LoginLeased.h"
#include "LeaseSystemClock.h"

LoginLeased::LoginLeased(Lease* lease)
  : lease(lease)
{}

bool
LoginLeased::setToken(const std::string& token_,
                      const std::chrono::seconds& duration)
{
  token = token_;
  lease->start(duration);
  return true;
};

bool
LoginLeased::setToken(const std::string& token_,
         const std::chrono::system_clock::time_point& when,
         const std::chrono::system_clock::time_point& until)
{
  token = token;
  lease->start(when, until);
  return true;
}

bool
LoginLeased::getToken(std::string& token_) const
{
  if (token != "" && lease->isValid()) {
    token_ = token;
    return true;
  } else {
    return false;
  }
};

bool
LoginLeased::isValid() const
{
  return lease->isValid();
}

fruit::Component<Login>
getLoginLeasedComponent()
{
  return fruit::createComponent()
    .install(getLeaseSystemClockComponent)
    .bind<Login, LoginLeased>();
}