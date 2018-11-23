#include "PmxDisplayPane.hpp"
#include "PmxHelper.hpp"
#include "PmxException.hpp"
#include "../StreamHelper.hpp"

namespace vitriol
{
	void PmxFrameData::Parse(std::istream& stream, PmxGlobalSettings settings)
	{
		stream.read(reinterpret_cast<char*>(&this->frameType), sizeof(this->frameType));
		const int size = this->frameType == PmxDisplayFrameType::Bone
			? settings.boneIndexSize
			: this->frameType == PmxDisplayFrameType::Morph
			? settings.morphIndexSize : throw PMX_EXCEPTION("Invalid frame type");

		this->index = readIndex(stream, size);
	}

	void PmxFrameData::Save(std::ostream& stream, PmxGlobalSettings settings) const
	{
		streamWrite(stream, this->frameType);
		const int size = this->frameType == PmxDisplayFrameType::Bone
			? settings.boneIndexSize
			: this->frameType == PmxDisplayFrameType::Morph
			? settings.morphIndexSize : throw PMX_EXCEPTION("Invalid frame type");
		writeIndex(stream, this->index, size);
	}

	void PmxDisplayData::Parse(std::istream& stream, PmxGlobalSettings settings)
	{
		this->nameLocal = readString(stream, settings.textEncoding);
		this->nameUniversal = readString(stream, settings.textEncoding);
		stream.read(reinterpret_cast<char*>(&this->isSpecialFrame), sizeof(this->isSpecialFrame));
		int32_t frameCount;
		stream.read(reinterpret_cast<char*>(&frameCount), sizeof(frameCount));
		this->frames.reserve(frameCount);
		for (auto i = 0; i < frameCount; i++)
		{
			PmxFrameData frameData;
			frameData.Parse(stream, settings);
			this->frames.emplace_back(frameData);
		}
	}

	void PmxDisplayData::Save(std::ostream& stream, PmxGlobalSettings settings)
	{
		writeString(stream, this->nameLocal, settings.textEncoding);
		writeString(stream, this->nameUniversal, settings.textEncoding);
		streamWrite(stream, this->isSpecialFrame);
		streamWrite(stream, static_cast<int32_t>(this->frames.size()));
		for (auto& frame : this->frames)
			frame.Save(stream, settings);
	}
}
