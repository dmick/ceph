#include "common/errno.h"

#include <sstream>
#include <string>
#undef _GNU_SOURCE	// get int version of strerror_r
#include <string.h>

std::string cpp_strerror(int err)
{
  char buf[128];

  if (err < 0)
    err = -err;
  std::ostringstream oss;
  buf[0] = '\0';
  strerror_r(err, buf, sizeof(buf));
  oss << "(" << err << ") " << buf;

  return oss.str();
}
