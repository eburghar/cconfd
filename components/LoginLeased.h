#ifndef CCONFD_LOGINLEASED_H
#define CCONFD_LOGINLEASED_H

#include "common/Debug.h"
#include "common/Logger.h"
#include "interfaces/Lease.h"
#include "interfaces/Login.h"

#include <fruit/fruit.h>
#include <string>

class LoginLeased : public Login
{
public:
  INJECT(LoginLeased(Lease* lease));
  virtual bool setToken(const std::string& token,
                        const std::chrono::seconds& duration) override;
  virtual bool setToken(const std::string& token,
                        const std::chrono::system_clock::time_point& when,
                        const std::chrono::system_clock::time_point& until) override;
  virtual bool getToken(std::string& token) const override;
  virtual bool isValid() const override;

private:
  Lease* lease;
  std::string token;
};

fruit::Component<Login>
getLoginLeasedComponent();

#endif // SVCONFD_LOGINLEASED_H