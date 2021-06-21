#include "EngineJsonnet.h"
#include "common/Exceptions.h"

#include <fstream>
#include <list>
//#include <cstring>

extern "C"
{
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
}

namespace fs = std::filesystem;

namespace {
void
parseMultiOutput(const char* jsonnet_output,
                 std::map<std::string, std::string>* outputs)
{
  for (const char* c = jsonnet_output; *c != '\0';) {
    const char* filename = c;
    const char* c2 = c;
    while (*c2 != '\0')
      ++c2;
    ++c2;
    const char* json = c2;
    while (*c2 != '\0')
      ++c2;
    ++c2;
    c = c2;
    outputs->insert(std::make_pair(filename, json));
  }
}
} // namespaceautoExpandEnv(p.role);

EngineJsonnet::EngineJsonnet()
{}

EngineJsonnet::~EngineJsonnet()
{
  if (vm_ != nullptr) {
    ::jsonnet_destroy(vm_);
  }
}

bool
EngineJsonnet::init(const std::vector<std::string>& paths)
{
  vm_ = static_cast<struct JsonnetVm*>(::jsonnet_make());

  if (const char *jsonnet_path_env = getenv("JSONNET_PATH")) {
    std::list<std::string> jpath;
    std::istringstream iss(jsonnet_path_env);
    std::string path;
    while (std::getline(iss, path, ',')) {
        jpath.push_front(path);
    }
    for (const auto &path : jpath) {
        jsonnet_jpath_add(vm_, path.c_str());
    }
  }

  for (const auto &path : paths) {
    jsonnet_jpath_add(vm_, path.c_str());
  }

  jsonnet_string_output(vm_, 1);
  return vm_ != nullptr;
}

bool
EngineJsonnet::evaluateSnippetMulti(const std::string& filename,
                                    const std::string& snippet,
                                    std::map<std::string, std::string>* outputs)
{
  if (outputs == nullptr) {
    return false;
  }
  int error = 0;
  const char* jsonnet_output = ::jsonnet_evaluate_snippet_multi(
    vm_, filename.c_str(), snippet.c_str(), &error);
  if (error != 0) {
    last_error_.assign(jsonnet_output);
    return false;
  }
  parseMultiOutput(jsonnet_output, outputs);
  return true;
}

bool
EngineJsonnet::setFileAttr(
  const std::string filename,
  const std::string user,
  fs::perms perms)
{
  fs::permissions(fs::path(filename), perms);
  if (! user.empty()) {
    if (! geteuid()) {
      struct passwd *pw = getpwnam(user.c_str());
      if (pw)
        chown(filename.c_str(), pw->pw_uid, pw->pw_gid);
    } else {
      SPDLOG_ERROR("Can't change ownership of \"{0}\" to \"{1}\"", filename, user);
      return false;
    }
  }
  return true;
}

bool
EngineJsonnet::writeMultiOutputs(
  const std::map<std::string, std::string>& outputs,
  const std::string& output_dir,
  fs::perms perms,
  const std::string& user,
  const std::string& cmd)
{

  bool changed = false;
  for (const auto& pair : outputs) {
    const std::string& new_content = pair.second;
    // key is absolute or relative ?
    const std::string& filename = (pair.first.length() > 0 && pair.first[0] == '/') ? pair.first : output_dir + pair.first;
    {
      SPDLOG_INFO("Creating \"{}\"", filename);
      std::ifstream exists(filename.c_str());
      if (exists.good()) {
        std::string existing_content;
        existing_content.assign(std::istreambuf_iterator<char>(exists),
                                std::istreambuf_iterator<char>());
        if (existing_content == new_content) {
          // Do not bump the timestamp on the file if its content is
          // the same. This may trigger other tools (e.g. make) to do
          // unnecessary work.
          setFileAttr(filename, user, perms);
          continue;
        }
      }
      changed = true;
    }
    std::ofstream f;
    f.open(filename.c_str());
    if (!f.good()) {
      SPDLOG_ERROR("Opening output file \"{}\"", filename);
      return false;
    }
    f << new_content;
    f.close();
    if (!f.good()) {
      SPDLOG_ERROR("Writing to output file \"{}\"", filename);
      return false;
    }
    setFileAttr(filename, user, perms);
  }

  if (changed && cmd.length() != 0) {
    SPDLOG_INFO("Executing {}", cmd);
    system(cmd.c_str());
  }
  return true;
}

std::string
EngineJsonnet::lastError() const
{
  return last_error_;
}

fruit::Component<Engine>
getEngineJsonnetComponent()
{
  return fruit::createComponent().bind<Engine, EngineJsonnet>();
}