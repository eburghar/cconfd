#ifndef CCONFD_MAINDEBUG_H
#define CCONFD_MAINDEBUG_H

#ifdef DEBUG
#include "common/Debug.h"
#include <backward.hpp>

void
stacker()
{
  backward::StackTrace st;
  st.load_here(32);
  backward::Printer p;
  p.print(st, stderr);
}
backward::SignalHandling sh;
#endif

#include <signal.h>
#include <spdlog/spdlog.h>
#include "common/Exceptions.h"

void
sighandler(int sig)
{
  SPDLOG_INFO("Signal {} caught", sig);
  #ifdef DEBUG
  if (sig == SIGABRT) {
    stacker();
    exit(1);
  }
  #endif
  throw exit_exception(1);
}

void catchsignals() {
  signal(SIGABRT, &sighandler);
  signal(SIGTERM, &sighandler);
  signal(SIGINT, &sighandler);
}

#endif // CCONFD_MAINDEBUG_H