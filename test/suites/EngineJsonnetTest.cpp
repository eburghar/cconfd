#include "components/EngineJsonnet.h"

#include <gtest/gtest.h>

class EngineJsonnetTest : public ::testing::Test
{
protected:
  EngineJsonnetTest()
    : injector(getEngineJsonnetComponent)
    , engine(injector)
  {}

  void SetUp() override{};

  // injector delete all injected object, so it must be bound to Fixture
  // lifetime hence declared as member and initialized in constructor
  fruit::Injector<Engine> injector;
  Engine* engine;
};

TEST_F(EngineJsonnetTest, TestEvaluateSnippet)
{
  std::vector<std::string> paths;
  ASSERT_TRUE(engine->init(paths));
}