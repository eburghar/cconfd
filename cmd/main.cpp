#include "common/MainDebug.h"
#include "common/Exceptions.h"
#include "common/Logger.h"
#include "components/ConfigDirJson.h"

#include <args.hxx>
#include <chrono>
#include <iostream>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <string>
#include <thread>

auto
main(int argc, char* argv[]) -> int
{
  try {
    auto logger = spdlog::stderr_color_mt("cconfd");
    spdlog::set_default_logger(logger);

    args::ArgumentParser parser(
      "Generate files with jsonnet and vault secrets using kubernetes "
      "authentication",
      "");
    args::HelpFlag help_(
      parser, "help", "Display this help menu", { 'h', "help" });
    args::ValueFlag<std::string> dir_(
      parser, "dir", "The config directory", { 'd', "dir" });
    args::ValueFlag<std::string> url_(
      parser, "url", "The vault url", { 'u', "url" });
    args::ValueFlag<std::string> cacert_(
      parser,
      "path",
      "the vault ca certificate authority",
      { 'c', "cacert" },
      "/var/run/secrets/kubernetes.io/serviceaccount/ca.crt");
    args::ValueFlag<std::string> token_(
      parser,
      "path",
      "Path of kubernetes token",
      { 't', "token" },
      "/var/run/secrets/kubernetes.io/serviceaccount/token");
    args::ValueFlag<int> sleep_(
      parser, "seconds", "time to sleep in main loop", { 's', "sleep" }, 60);
    args::CounterFlag verbose_(
      parser, "verbose", "Level of verbosity", { 'v', "verbose" });
    args::ValueFlag<int> readiness_(
      parser, "readiness descriptor", "Descriptor for s6 readiness signaling", { 'r', "readiness" }, 0);
    try {
      parser.ParseCLI(argc, argv);
    } catch (const args::Help&) {
      std::cout << parser;
      return 0;
    } catch (const args::ParseError& e) {
      std::cerr << e.what() << parser;
      return 1;
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

    auto dir = args::get(dir_);
    auto url = args::get(url_);
    if (url[url.length() - 1] != '/') {
      url += '/';
    }
    auto cacert = args::get(cacert_);
    if (cacert != "" && !std::filesystem::exists(cacert)) {
      SPDLOG_CRITICAL("cacert {} doesn't exist", cacert);
      return 1;
    }
    auto token = args::get(token_);
    if (token != "" && !std::filesystem::exists(token)) {
      SPDLOG_CRITICAL("token {} doesn't exist", token);
      return 1;
    }
    auto sleep = args::get(sleep_);
    if (sleep == 0) {
      SPDLOG_CRITICAL("seconds to sleep should be > 0");
      return 1;
    }
    auto readiness = args::get(readiness_);

    catchsignals();

    fruit::Injector<ConfigDir> injector(getConfigDirJsonComponent);
    ConfigDir* configDir = injector.get<ConfigDir*>();

    // main loop
    while (true) {
      try {
        configDir->generateConfigFiles(dir, url, token, cacert, sleep, readiness);
        SPDLOG_INFO("Sleep for {}s", sleep);
        std::chrono::seconds timespan(sleep);
        std::this_thread::sleep_for(timespan);
      } catch (const exit_exception& e) {
        SPDLOG_CRITICAL(e.what());
        return e.c;
      } catch (const exception& e) {
        SPDLOG_CRITICAL(e.what());
        return 1;
      }
    }
  } catch (const spdlog::spdlog_ex& ex) {
    std::cerr << "Log initialization failed: " << ex.what() << std::endl;
  }

  return 0;
}
