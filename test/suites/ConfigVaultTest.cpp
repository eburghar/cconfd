#include "components/ConfigVault.h"

#include <gtest/gtest.h>

using json = nlohmann::json;

class ConfigVaultTest : public ::testing::Test
{
protected:
  ConfigVaultTest()
    : injector(getConfigVaultComponent)
    , factory(injector)
  {}

  void SetUp() override{};

  // injector delete all injected object, so it must be bound to Fixture
  // lifetime hence declared as member and initialized in constructor
  fruit::Injector<ConfigVaultFactory> injector;
  ConfigVaultFactory factory;
};

TEST_F(ConfigVaultTest, EmptyConfig)
{
  json cfg;
  auto conf = factory(cfg, "", "test/token", "", 60);
  ASSERT_EQ(conf->dump(),
            "{\"cmd\":\"\",\"dir\":\"\",\"mode\":\"\",\"paths\":[],\"role\":\"\",\"secrets\":{},\"tmpl\":\"\",\"user\":\"\"}"
           );
}

TEST_F(ConfigVaultTest, FullConfig)
{
  json cfg{ { "dir", "test" },
            { "tmpl", "service.jsonnet" },
            { "mode", "0644" },
            { "role", "demo" },
            { "secrets", { { "account", "vault:secret/data/demo/account" } } },
            { "cmd", "/bin/echo refresh" } };

  auto conf = factory(cfg, "", "test/token", "", 60);
  ASSERT_EQ(conf->dump(),
            "{\"cmd\":\"/bin/echo refresh\",\"dir\":\"test/\",\"mode\":\"0644\",\"paths\":[],\"role\":\"demo\",\"secrets\":{\"account\":\"vault:secret/data/demo/account\"},\"tmpl\":\"service.jsonnet\",\"user\":\"\"}"
            );
}

TEST_F(ConfigVaultTest, PartialConfig)
{
  json cfg{ { "dir", "test" } };

  auto conf = factory(cfg, "", "test/token", "", 60);
  ASSERT_EQ(conf->dump(),
            "{\"cmd\":\"\",\"dir\":\"test/\",\"mode\":\"\",\"paths\":[],\"role\":\"\",\"secrets\":{},\"tmpl\":\"\",\"user\":\"\"}"
            );
}