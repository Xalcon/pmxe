#pragma once
#include <cstdint>
#include <istream>
#include <vector>

#include "PmxDisplayPane.hpp"
#include "PmxGlobalSettings.hpp"

namespace vitriol
{
	enum class PmxDisplayFrameType : uint8_t
	{
		Bone = 0,
		Morph = 1,
	};

	class PmxFrameData
	{
	public:
		PmxDisplayFrameType frameType;
		int32_t index;

		void Parse(std::istream* stream, PmxGlobalSettings settings);
	};

	class PmxDisplayData
	{
	public:
		std::string displayNameLocal;
		std::string displayNameUniversal;
		uint8_t isSpecialFrame;
		//int32_t frameCount;
		std::vector<PmxFrameData> frames;

		void Parse(std::istream* stream, PmxGlobalSettings settings);
	};
}
