#include "common/MainDebug.h"
#include "common/Logger.h"

#include <args.hxx>
#include <gtest/gtest.h>
#include <spdlog/sinks/stdout_color_sinks.h>

int
main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);

  auto logger = spdlog::stderr_color_mt("cconfd_test");
  spdlog::set_default_logger(logger);

  args::ArgumentParser parser("");
  args::CounterFlag verbose_(
    parser, "verbose", "Level of verbosity", { 'v', "verbose" });
  try {
    parser.ParseCLI(argc, argv);
  } catch (const args::Help&) {
    std::cout << parser;
    return 0;
  } catch (const args::ParseError& e) {
    std::cerr << e.what() << parser;
    return 1;
  } catch (const exit_exception& e) {
    SPDLOG_CRITICAL(e.what());
    return e.c;
  }

  // sanitize arguments
  if (verbose_) {
    int level = spdlog::level::warn - args::get(verbose_);
    std::cout << level << std::endl;
    if (level < 0)
      level = 0;
    spdlog::set_level(static_cast<spdlog::level::level_enum>(level));
  } else
    spdlog::set_level(spdlog::level::warn);

  return RUN_ALL_TESTS();
}