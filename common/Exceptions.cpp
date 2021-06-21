#include "Exceptions.h"

exception::exception(const std::string& msg_) noexcept
  : std::exception()
  , msg(msg_)
{}

const char*
exception::what() const noexcept
{
  return msg.c_str();
}

file_exception::file_exception(const std::string& msg_) noexcept
  : exception(msg_)
{}

exit_exception::exit_exception(int c) noexcept
  : exception("Exited")
  , c(c)
{}