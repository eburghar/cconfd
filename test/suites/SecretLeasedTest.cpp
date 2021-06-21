#include "components/SecretLeased.h"

#include <gtest/gtest.h>

using json = nlohmann::json;

class SecretLeasedTest : public ::testing::Test
{
protected:
  SecretLeasedTest()
    : injector(getSecretLeasedComponent)
    , factory(injector)
    , when(std::chrono::system_clock::now())
    , until(when - std::chrono::hours(24)) // the secret is created as invalid
    , secret(factory("account", "secret/data/demo/account", "", when, until))
  {}

  void SetUp() override{};

  // injector delete all injected object, so it must be bound to Fixture
  // lifetime hence declared as member and initialized in constructor
  fruit::Injector<SecretFactory> injector;
  SecretFactory factory;
  std::chrono::system_clock::time_point when;
  std::chrono::system_clock::time_point until;
  std::unique_ptr<Secret> secret;
};

static const json data {
  { "data", { { "user", "username" }, { "password", "foo" } } }
};

// Secret creation
TEST_F(SecretLeasedTest, Create)
{
  // secret not stamped is not valid
  EXPECT_FALSE(secret->isValid());
}

// Secret creation
TEST_F(SecretLeasedTest, Start)
{
  secret->stamp(std::chrono::seconds(1));
  // stamped secret is valid
  EXPECT_TRUE(secret->isValid());
}

// Secret expired
TEST_F(SecretLeasedTest, Expired)
{
  secret->stamp(std::chrono::seconds(-1));
  // expired secret is not valid
  EXPECT_FALSE(secret->isValid());
}

// Test secret
TEST_F(SecretLeasedTest, Get)
{
  secret->stamp(std::chrono::seconds(1));
  secret->add(data);
  json j;

  EXPECT_TRUE(secret->get(j));
  EXPECT_EQ(j["/data/user"_json_pointer], "username");

  // expired secret should return {}
  secret->stamp(std::chrono::seconds(-1));
  ASSERT_FALSE(secret->get(j));
  EXPECT_TRUE(j.empty());
}

// Overwrite Secret
TEST_F(SecretLeasedTest, Overwrite)
{
  secret->stamp(std::chrono::seconds(1));
  secret->add(data);
  json j;

  ASSERT_TRUE(secret->get(j));
  EXPECT_EQ(j["/data/user"_json_pointer], "username");
  j["data"]["user"] = "john";
  secret->add(j);

  // test that original json didn't changed
  EXPECT_EQ(data["/data/user"_json_pointer], "username");

  // test that value have been overwritten in secret
  ASSERT_TRUE(secret->get(j));
  EXPECT_EQ(j["/data/user"_json_pointer], "john");
}