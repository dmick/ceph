// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*- 
// vim: ts=8 sw=2 smarttab
#include <string>
#include <sstream>
#include <map>
#include <boost/variant.hpp>

// generate the interesting enums by the "include-with-macro-selection" trick
enum command_op {
	  CMD_INVALID,          // skip 0
#undef COMMAND
#define COMMAND(module, parsesig, op, helptext) op,
#include <mon/MonCommands.h>
};

typedef boost::variant<std::string, bool, int, uint64_t, double> cmd_vartype;

bool cmdmap_from_json(std::string cmd, std::map<std::string, cmd_vartype> *mapp, std::stringstream &ss);

// shorthand, assume cmdmap.  This is kind of ugly and
// should probably be replaced with template functions

#define getval(k, t)	boost::get<t>(cmdmap[k])
#define putval(k, t, v)	(cmdmap[k]) = t(v)

