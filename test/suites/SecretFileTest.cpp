#include "components/SecretFile.h"

#include <gtest/gtest.h>

using json = nlohmann::json;

class SecretFileTest : public ::testing::Test
{
protected:
  SecretFileTest()
    : secret(std::make_unique<SecretFile>("secret", "test/secret.txt"))
  {}

  void SetUp() override{};
  std::unique_ptr<Secret> secret;
};

// Secret validity
TEST_F(SecretFileTest, Validity)
{
  // secret file is valid
  EXPECT_TRUE(secret->isValid());
}

// Secret content
TEST_F(SecretFileTest, Content)
{
	json j;

  	EXPECT_TRUE(secret->get(j));
  	EXPECT_EQ(j.dump(), "\"foo\"");
}