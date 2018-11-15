#ifndef __VITRIOL_PMX_H__
#define __VITRIOL_PMX_H__
#include "PmxGlobalSettings.hpp"

namespace Vitriol
{
    typedef enum PMX_STR_ENCODING
    {
        PMX_STR_ENCODING_UTF16_LE = 0,
        PMX_STR_ENCODING_UTF8 = 1
    } PMX_STR_ENCODING;

    class Pmx
    {
        public:
            float version;
            PmxGlobalSettings GlobalSettings;
    };
}

#endif