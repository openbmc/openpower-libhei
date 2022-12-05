#pragma once

#include <buildinfo.hpp>

// TODO: It would be preferred that this function exist in buildinfo.hpp.
//       However, due to meson limitations, we were not able to figure out how
//       to install that auto-generated header so it could be available to
//       the user applications.

namespace libhei
{

inline const char* getBuildInfo()
{
    return BUILDINFO;
}

} // end namespace libhei
