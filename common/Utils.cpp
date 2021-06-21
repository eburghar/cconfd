#include "Utils.h"

bool
endsWith(const std::string& str, const char* suffix, unsigned suffixLen)
{
  return str.size() >= suffixLen &&
         0 == str.compare(str.size() - suffixLen, suffixLen, suffix, suffixLen);
}

bool
endsWith(const std::string& str, const char* suffix)
{
  return endsWith(str, suffix, std::string::traits_type::length(suffix));
}

bool
startsWith(const std::string& str, const std::string& prefix)
{
  return std::equal(prefix.begin(), prefix.end(), str.begin());
}

// Update the input string.
void
autoExpandEnv(std::string & text) {
    static std::regex env("\\$\\{([^}]+)\\}");
    std::smatch match;
    while (std::regex_search( text, match, env)) {
        const char * s = getenv(match[1].str().c_str());
        const std::string var(s == NULL ? "" : s);
        text.replace( match[0].first, match[0].second, var );
    }
}

// Leave input alone and return new string.
std::string
expandEnv(const std::string & input) {
    std::string text = input;
    autoExpandEnv(text);
    return text;
}