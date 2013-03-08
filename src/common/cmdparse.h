// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*- 
// vim: ts=8 sw=2 smarttab
#include <string>
#include <sstream>
#include <map>
#include <boost/variant.hpp>

typedef boost::variant<std::string, bool, int, uint64_t, double> cmd_vartype;

bool cmdmap_from_json(std::string cmd, std::map<std::string, cmd_vartype> *mapp, std::stringstream &ss);

template <typename T>
bool
getval(std::map<std::string, cmd_vartype>& cmdmap, std::string k, T& val)
{
  // referencing a nonexistent key creates it, even as an rvalue;
  // we don't want that behavior for get.
  if (cmdmap.count(k)) {
    try {
      val = boost::get<T>(cmdmap[k]);
      return true;
    } catch (boost::bad_get) { }
  }
  // either not found or bad type, return false
  return false;
}

// with default
template <typename T>
void
getval(std::map<std::string, cmd_vartype>& cmdmap, std::string k, T& val,
       T defval)
{
  if (!getval(cmdmap, k, val))
    val = defval;
}

template <typename T>
void
putval(std::map<std::string, cmd_vartype>& cmdmap, std::string k, T val)
{
  cmdmap[k] = val;
}
