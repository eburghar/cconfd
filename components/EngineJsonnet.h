#ifndef CCONFD_ENGINEJSONNET_H
#define CCONFD_ENGINEJSONNET_H

#include "common/Debug.h"
#include "common/Logger.h"
#include "interfaces/Engine.h"

#include <fruit/fruit.h>
extern "C"
{
#include <libjsonnet.h>
}

class EngineJsonnet : public Engine
{
public:
  INJECT(EngineJsonnet());
  ~EngineJsonnet();
  virtual bool evaluateSnippetMulti(
    const std::string& filename,
    const std::string& snippet,
    std::map<std::string, std::string>* outputs) override;
  virtual bool writeMultiOutputs(
    const std::map<std::string, std::string>& outputs,
    const std::string& output_dir,
    std::filesystem::perms perms,
    const std::string& user,
    const std::string& cmd) override;
  std::string lastError() const override;
  bool init(const std::vector<std::string>& paths) override;
  bool setFileAttr(
    const std::string filename,
    const std::string user,
    std::filesystem::perms perms);

private:
  struct JsonnetVm* vm_;
  std::string last_error_;
};

fruit::Component<Engine>
getEngineJsonnetComponent();

#endif // CCONFD_ENGINEJSONNET_H