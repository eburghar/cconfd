#include "ConfigVault.h"
#include "common/Utils.h"
#include "components/EngineJsonnet.h"
//#include "components/VaultClientHttp.h"

#include <fstream>
#include <iostream>

namespace fs = std::filesystem;
using json = nlohmann::json;

static std::string
readFile(std::ifstream& in)
{
  return std::string(std::istreambuf_iterator<char>(in),
                     std::istreambuf_iterator<char>());
}

void
to_json(json& j, const config& p)
{
  j = json{ { "tmpl", p.tmpl }, { "dir", p.dir }, { "user", p.user },
            { "role", p.role }, { "mode", p.mode }, { "cmd", p.cmd },
            { "secrets", p.secrets }, { "paths", p.paths } };
}

template<typename T>
void
set_value(const json& j, const std::string& k, T& d)
{
  if (j.find(k) != j.end())
    j[k].get_to(d);
}

void
from_json(const json& j, config& p)
{
  if (j.is_null())
    return;

  set_value(j, "tmpl", p.tmpl);
  set_value(j, "dir", p.dir);
  set_value(j, "user", p.user);
  set_value(j, "role", p.role);
  set_value(j, "mode", p.mode);
  set_value(j, "cmd", p.cmd);
  set_value(j, "secrets", p.secrets);
  set_value(j, "paths", p.paths);

  if (p.dir.length() == 0)
    p.dir = "./";
  else if (p.dir[p.dir.length() - 1] != '/')
    p.dir += '/';

  autoExpandEnv(p.tmpl);
  autoExpandEnv(p.dir);
  autoExpandEnv(p.user);
  autoExpandEnv(p.role);
  autoExpandEnv(p.mode);
  autoExpandEnv(p.cmd);
}

ConfigVault::ConfigVault(VaultClientFactory vaultClientFactory,
                         Engine *engine,
                         const json& cfg,
                         const std::string& url,
                         const std::string& token,
                         const std::string& cacert,
                         int sleep)
  : engine(engine)
  , cfg(cfg)
  , vault(vaultClientFactory(url, this->cfg.role, token, cacert, sleep))
  , file(std::make_unique<VaultClientFile>())
  , env(std::make_unique<VaultClientEnv>())
{}

std::string
ConfigVault::dump() const
{
  json j = cfg;
  return j.dump();
}

bool
ConfigVault::generateConfigFiles()
{
  if (cfg.tmpl.length() == 0) {
    SPDLOG_ERROR("'tmpl' key not found in config");
    return false;
  }
  if (cfg.dir.length() == 0) {
    SPDLOG_ERROR("'dir' key not found in config");
    return false;
  }
  const std::string& tmpl = (cfg.tmpl[0] == '/') ? cfg.tmpl : cfg.dir + cfg.tmpl;
  std::ifstream in(tmpl);
  if (!in.good()) {
    SPDLOG_ERROR("Failed to read {0}{1} !", cfg.dir, cfg.tmpl);
    return false;
  }

  vault->logIn();

  // construct secret object
  json j;
  for (const auto &[k, v] : cfg.secrets) {
    auto colon = v.find(':');
    if (colon == std::string::npos) {
      SPDLOG_ERROR("no secret provider specified for {0}", k);
      return false;
    }
    if (colon == v.size()) {
      SPDLOG_ERROR("provider without a path for {0}", k);
      return false;
    }

    auto provider = v.substr(0, colon);
    auto sv = expandEnv(v.substr(colon + 1));
    if (provider == "vault") {
      vault->get(k, sv, j[k]);
    } else if (provider == "file") {
      file->get(k, sv, j[k]);
    } else if (provider == "env") {
      env->get(k, sv, j[k]);
    } else if (provider == "") {
      j[k].get_to(sv);
    } else {
      SPDLOG_ERROR("unknown secret provider {0} for {1}", provider, k);
      return false;
    }
  }

  // generate vault object
  const std::string input =
    "local secrets = " + j.dump() + ";\n" + readFile(in);

  SPDLOG_DEBUG("jsonnet input {0}", input);
  // evaluate and save files
  engine->init(cfg.paths);
  std::map<std::string, std::string> outputs;
  if (engine->evaluateSnippetMulti(cfg.tmpl, input, &outputs))
    engine->writeMultiOutputs(outputs, cfg.dir, fsPerms(), cfg.user, cfg.cmd);
  else {
    SPDLOG_ERROR(engine->lastError());
    return false;
  }

  return true;
}

fs::perms
ConfigVault::fsPerms() const
{
  // TODO: catch std::invalid_argument and std::out_of_range
  auto ps = std::stoi(cfg.mode, 0, 8);
  return static_cast<fs::perms>(ps);
}

fruit::Component<ConfigVaultFactory>
getConfigVaultComponent()
{
  return fruit::createComponent()
    .install(getVaultClientHttpComponent)
    .install(getEngineJsonnetComponent)
    .bind<Config, ConfigVault>();
}
