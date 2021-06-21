#ifndef CCONFD_EXCEPTIONS_H
#define CCONFD_EXCEPTIONS_H

#include <exception>
#include <string>

class exception : public std::exception
{
private:
  std::string msg;

public:
  exception(const std::string& msg) noexcept;
  virtual const char* what() const noexcept;
};

class file_exception : public exception
{
public:
  file_exception(const std::string& msg) noexcept;
};

class exit_exception : public exception
{
public: 
   int c;
   exit_exception(int c) noexcept; 
};

#endif // CCONFD_EXCEPTIONS_H