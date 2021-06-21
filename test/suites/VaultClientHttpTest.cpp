#include "common/Utils.h"
#include "components/HttpClientFake.h"
#include "components/VaultClientFake.h"

#include <date/date.h>
#include <gtest/gtest.h>
#include <memory>

using json = nlohmann::json;

class VaultClientHttpTest : public ::testing::Test
{
protected:
  VaultClientHttpTest()
    : injector(getVaultClientFakeComponent)
    , factory(injector)
    , vault(
        static_unique_ptr_cast<VaultClientHttp>(factory("", "demo", "test/token", "", 60)))
  {}

  void SetUp() override{};

  // injector delete all injected object, so it must be bound to Fixture
  // lifetime hence declared as member and initialized in constructor
  fruit::Injector<VaultClientFactory> injector;
  VaultClientFactory factory;
  std::unique_ptr<VaultClientHttp> vault;
};

static const std::string date_fmt{ "%FT%TZ" };
static auto today{std::chrono::system_clock::now()};
static auto tomorrow{today + std::chrono::hours(24)};

// reply to login
static const json login_reply{
  { "auth",
    { { "accessor", "3jorFNuojJC4E6worwcsS1k6" },
      { "client_token", "5OACd4f5KZJflTo68sW5nFqg" },
      { "entity_id", "95a39261-eca6-4cce-aac4-72ce2c8086c4" },
      { "lease_duration", 3600 },
      { "metadata",
        { { "role", "demo" },
          { "service_account_name", "default" },
          { "service_account_namespace", "default" },
          { "service_account_secret_name", "default-token-sjxjw" },
          { "service_account_uid", "12dd0773-dd54-11e8-80ce-c48e8ff32dcb" } } },
      { "policies", { { "default", "demo" } } },
      { "renewable", true },
      { "token_policies", { { "default", "demo" } } } } },
  { "data", nullptr },
  { "lease_duration", 0 },
  { "lease_id", "" },
  { "renewable", false },
  { "request_id", "ec4df82f-36e2-aad6-7c08-7a8cf6fe7a86" },
  { "warnings", nullptr },
  { "wrap_info", nullptr }
};

// reply to get kv secret
static const json secret_reply{
  { "auth", nullptr },
  { "data",
    { { "data", { { "password", "foo" }, { "user", "username" } } },
      { "metadata",
        { { "created_time", "2018-11-08T18:30:29.193863566Z" },
          { "deletion_time", "" },
          { "destroyed", false },
          { "version", 1 } } } } },
  { "lease_duration", 0 },
  { "lease_id", "" },
  { "renewable", false },
  { "request_id", "f1baa9df-4ccc-d236-dff1-d51a4b2830df" },
  { "warnings", nullptr },
  { "wrap_info", nullptr }
};

// reply to get database secret (which has a lease)
static const json secret_leased_reply{
  { "auth", nullptr },
  { "data",
    { { "password", "A1a-2O8TwmDFdWCsSbS7" },
      { "username", "v-kubernet-demo-2R7ehjkCXAEDqy3kkvbY-1542194330" } } },
  { "lease_duration", 3600 },
  { "lease_id", "database/creds/demo/6joNxpvIH5UDzTdCPIyZ7zfV" },
  { "renewable", true },
  { "request_id", "eb8966a3-b63a-323a-b098-b277d077b48a" },
  { "warnings", nullptr },
  { "wrap_info", nullptr }
};

// reply to get lease from lease_id
static const json lease_reply{
  { "request_id", "052d5437-4785-bc8f-45dc-4ccd8674e54a" },
  { "lease_id", "" },
  { "renewable", false },
  { "lease_duration", 0 },
  { "data",
    { { "expire_time", date::format(date_fmt, tomorrow) },
      { "id", "database/creds/demo/6joNxpvIH5UDzTdCPIyZ7zfV" },
      { "issue_time", date::format(date_fmt, today) },
      { "last_renewal", nullptr },
      { "renewable", true },
      { "ttl", 3066 } } },
  { "wrap_info", nullptr },
  { "warnings", nullptr },
  { "auth", nullptr }
};

TEST_F(VaultClientHttpTest, Login)
{
  httpClientFake->addPostReply("auth/kubernetes/login", login_reply);
  ASSERT_TRUE(vault->logIn());
}

TEST_F(VaultClientHttpTest, GetSecret)
{
  json j;

  // get a secret without login
  httpClientFake->addPostReply("auth/kubernetes/login", login_reply);
  httpClientFake->addGetReply("secret/data/demo/account", secret_reply);
  ASSERT_TRUE(vault->get("account", "secret/data/demo/account", j));
}

TEST_F(VaultClientHttpTest, AddSecret)
{
  json j;

  vault->addSecret("account",
                   "secret/data/demo/account",
                   "",
                   std::chrono::seconds(-1),
                   secret_reply);
  // secret lease has expired, so lease is invalid
  ASSERT_FALSE(vault->getSecret("account", j));
  ASSERT_TRUE(j.empty());
}

TEST_F(VaultClientHttpTest, GetSecretCached)
{
  json j;

  // get a secret without login
  httpClientFake->addPostReply("auth/kubernetes/login", login_reply);
  httpClientFake->addGetReply("secret/data/demo/account", secret_reply);
  ASSERT_TRUE(vault->get("account", "secret/data/demo/account", j));
  ASSERT_EQ(j["/data/data/user"_json_pointer], secret_reply["/data/data/user"_json_pointer]);
  // second call should take the value from cache
  httpClientFake->removeGetReply("secret/data/demo/account");
  j = {};
  ASSERT_TRUE(vault->get("account", "secret/data/demo/account", j));
  ASSERT_EQ(j["/data/data/user"_json_pointer], secret_reply["/data/data/user"_json_pointer]);
}

TEST_F(VaultClientHttpTest, GetLease)
{
  json j;
  std::chrono::system_clock::time_point when;
  std::chrono::system_clock::time_point until;

  httpClientFake->addPostReply("auth/kubernetes/login", login_reply);
  httpClientFake->addPostReply("sys/leases/lookup", lease_reply);
  ASSERT_TRUE(vault->logIn());
  ASSERT_TRUE(vault->getLease(secret_leased_reply["lease_id"], when, until));

  ASSERT_EQ(date::format(date_fmt, when), lease_reply["/data/issue_time"_json_pointer]);
  ASSERT_EQ(date::format(date_fmt, until), lease_reply["/data/expire_time"_json_pointer]);
}