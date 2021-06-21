#include "components/LoginLeased.h"

#include <gtest/gtest.h>

// TODO test reply error
// {
//   "errors": [
//     "{\"kind\":\"Status\",\"apiVersion\":\"v1\",\"metadata\":{},\"status\":\"Failure\",\"message\":\"tokenreviews.authentication.k8s.io is forbidden: User \\\"system:serviceaccount:default:default\\\" cannot create resource \\\"tokenreviews\\\" in API group \\\"authentication.k8s.io\\\" at the cluster scope\",\"reason\":\"Forbidden\",\"details\":{\"group\":\"authentication.k8s.io\",\"kind\":\"tokenreviews\"},\"code\":403}"
//   ]
// }

class LoginLeasedTest : public ::testing::Test
{
protected:
  LoginLeasedTest()
    : injector(getLoginLeasedComponent)
    , login(injector)
  {}

  void SetUp() override{};

  // injector delete all injected object, so it must be bound to Fixture
  // lifetime hence declared as member and initialized in constructor
  fruit::Injector<Login> injector;
  Login* login;
};

// Login creation
TEST_F(LoginLeasedTest, Create)
{
  std::string token;
  // no token has been set
  ASSERT_FALSE(login->getToken(token));
  // login is not valid without token
  ASSERT_FALSE(login->isValid());
}

// Login validity
TEST_F(LoginLeasedTest, Validity)
{
  const std::string token = "test";
  ASSERT_TRUE(login->setToken(token, std::chrono::minutes(1)));
  // login with token is valid
  ASSERT_TRUE(login->isValid());

  std::string token2;
  // token has been set
  ASSERT_TRUE(login->getToken(token2));
  // an values equals
  ASSERT_EQ(token, token2);
}

// Login expired
TEST_F(LoginLeasedTest, Expired)
{
  const std::string token = "test";
  ASSERT_TRUE(login->setToken(token, std::chrono::minutes(-1)));
  // login has expired
  ASSERT_FALSE(login->isValid());
}