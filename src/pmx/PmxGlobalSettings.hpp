#include <stdint.h>
#include <iostream>

namespace Vitriol
{
	class PmxGlobalSettings
    {
        public:
            PmxGlobalSettings() {}

			uint8_t textEncoding;
			uint8_t additionalUV;
			uint8_t vertexIndexSize;
			uint8_t textureIndexSize;
			uint8_t materialIndexSize;
			uint8_t boneIndexSize;
			uint8_t morphIndexSize;
			uint8_t rigidBodyIndexSize;

			void Parse(std::istream *stream);
    };
}