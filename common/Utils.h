#ifndef CCONFD_UTILS_H
#define CCONFD_UTILS_H

#include <memory>
#include <string>
#include <algorithm> 
#include <cctype>
#include <locale>
#include <string>
#include <regex>
#include <cstdlib>

// STRING UTILS

// trim from start (in place)
static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}

// trim from start (copying)
static inline std::string ltrim_copy(std::string s) {
    ltrim(s);
    return s;
}

// trim from end (copying)
static inline std::string rtrim_copy(std::string s) {
    rtrim(s);
    return s;
}

// trim from both ends (copying)
static inline std::string trim_copy(std::string s) {
    trim(s);
    return s;
}

void
autoExpandEnv(std::string & text);

std::string
expandEnv(const std::string & input);

template<typename TO, typename FROM>
std::unique_ptr<TO>
static_unique_ptr_cast(std::unique_ptr<FROM>&& from)
{
  return std::unique_ptr<TO>{ static_cast<TO*>(from.release()) };
  // conversion: unique_ptr<FROM>->FROM*->TO*->unique_ptr<TO>
}

bool
endsWith(const std::string& str, const char* suffix, unsigned suffixLen);

bool
endsWith(const std::string& str, const char* suffix);

bool
startsWith(const std::string& str, const std::string& prefix);

#endif // CCONFD_UTILS_H