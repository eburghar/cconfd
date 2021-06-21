#include "components/LeaseSystemClock.h"

#include <gtest/gtest.h>
#include <date/date.h>

class LeaseSystemClockTest : public ::testing::Test
{
protected:
  LeaseSystemClockTest()
    : injector(getLeaseSystemClockComponent)
    , lease(injector)
  {}

  void SetUp() override{};

  fruit::Injector<Lease> injector;
  Lease* lease;
};

// Lease creation
TEST_F(LeaseSystemClockTest, Create)
{
  // A lease not started is not valid
  ASSERT_FALSE(lease->isValid());
}

// Lease start
TEST_F(LeaseSystemClockTest, Start)
{
  lease->start(std::chrono::minutes(1));
  // A started lease is valid
  ASSERT_TRUE(lease->isValid());
}

// Lease expired
TEST_F(LeaseSystemClockTest, Expired)
{
  lease->start(std::chrono::minutes(-1));
  // expired lease is not valid anymore
  ASSERT_FALSE(lease->isValid());
}

// Start Lease from RFC stamp
TEST_F(LeaseSystemClockTest, RFCStart)
{

  auto today{std::chrono::system_clock::now()};
  auto tomorrow{today + std::chrono::hours(24)};
  auto yesterday{today - std::chrono::hours(24)};
  auto before_yesterday{yesterday - std::chrono::hours(24)};
  const std::string fmt{"%FT%TZ"};

  // lease one day in future is valid
  lease->start(date::format(fmt, today), date::format(fmt, tomorrow));
  ASSERT_TRUE(lease->isValid());

  // lease one day in past is invalid
  lease->start(date::format(fmt, before_yesterday), date::format(fmt, yesterday));
  ASSERT_FALSE(lease->isValid());

  // lease with no duration even in past is valid
  lease->start(date::format(fmt, yesterday), date::format(fmt, yesterday));
  ASSERT_TRUE(lease->isValid());
}