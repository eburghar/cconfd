#ifndef CCONFD_CONFIGDIR_H
#define CCONFD_CONFIGDIR_H

class ConfigDir
{
public:
  virtual bool generateConfigFiles(const std::string& dir,
                                   const std::string& url,
                                   const std::string& token,
                                   const std::string& cacert,
                                   int sleep,
                                   int readiness) = 0;
};

#endif // CCONFD_CONFIGDIR_H