#ifndef CCONFD_ENGINE_H
#define CCONFD_ENGINE_H

#include <chrono>
#include <filesystem>
#include <map>
#include <string>
#include <vector>

class Engine
{
public:
  virtual bool evaluateSnippetMulti(
    const std::string& filename,
    const std::string& snippet,
    std::map<std::string, std::string>* outputs) = 0;
  virtual bool writeMultiOutputs(
    const std::map<std::string, std::string>& outputs,
    const std::string& output_dir,
    std::filesystem::perms perms,
    const std::string& user,
    const std::string& cmd) = 0;
  virtual std::string lastError() const = 0;
  virtual bool init(const std::vector<std::string>& paths) = 0;
};

#endif // CCONFD_ENGINE_H