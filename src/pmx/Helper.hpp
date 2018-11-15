#ifndef __VITRIOL_HELPER_H__
#define __VITRIOL_HELPER_H__
#include <string>
#include <istream>
#include "Pmx.hpp"

namespace Vitriol
{
    std::string ReadString(std::istream* stream, PMX_STR_ENCODING encoding);
}

#endif