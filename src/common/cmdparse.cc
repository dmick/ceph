#include "common/cmdparse.h"
#include "json_spirit/json_spirit.h"

// Parse JSON in cmd into a map from field to map of values
// (use mValue/mObject)
// 'cmd' should not disappear over lifetime of map
// 'mapp' points to the caller's map
// 'ss' captures any errors during JSON parsing; if function returns
// false, ss is valid

using namespace std;

bool
cmdmap_from_json(string cmd, map<string, cmd_vartype> *mapp, stringstream &ss)
{
  json_spirit::mValue v;

  try {
    if (!json_spirit::read(cmd, v))
      throw std::runtime_error("unparseable JSON" + cmd);
    if (v.type() != json_spirit::obj_type)
      throw(std::runtime_error("not JSON object" + cmd));

    // allocate new mObject (map) to return
    // make sure all contents are simple types (not arrays or objects)
    json_spirit::mObject o = v.get_obj();
    for (map<string, json_spirit::mValue>::iterator it = o.begin();
	 it != o.end(); it++) {

      // ok, marshal it into our string->cmd_vartype map, or throw an
      // exception if it's not a simple datatype.  This is kind of
      // annoying, since json_spirit has a boost::variant inside it
      // already, but it's not public.  Oh well.

      switch (it->second.type()) {

      case json_spirit::obj_type:
      case json_spirit::array_type:
      default:
	throw(std::runtime_error("JSON array/object not allowed" + cmd));
        break;

      case json_spirit::str_type:
	(*mapp)[it->first] = it->second.get_str();
	break;

      case json_spirit::bool_type:
	(*mapp)[it->first] = it->second.get_bool();
	break;

      case json_spirit::int_type:
	(*mapp)[it->first] = it->second.get_uint64();
	break;

      case json_spirit::real_type:
	(*mapp)[it->first] = it->second.get_real();
	break;
      }
    }
    return true;
  } catch (std::runtime_error e) {
    ss << e.what();
    return false;
  }
}
