#pragma once
#include "PmxTypes.hpp"
#include <istream>

namespace vitriol
{
	class PmxGlobalSettings
	{
	public:
		PmxStringEncoding textEncoding;
		uint8_t additionalVectors;
		uint8_t vertexIndexSize;
		uint8_t textureIndexSize;
		uint8_t materialIndexSize;
		uint8_t boneIndexSize;
		uint8_t morphIndexSize;
		uint8_t rigidBodyIndexSize;

		void Parse(std::istream& stream);
		void Save(std::ostream& stream);
	};
}
