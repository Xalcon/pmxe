#include "Vmd.hpp"
#include "../StreamHelper.hpp"

namespace vmd
{
	void Vmd::Parse(std::istream& stream)
	{
		vitriol::streamRead(stream, this->header);
		
		int32_t count;

		vitriol::streamRead(stream, count);
		this->frames.resize(count);
		vitriol::streamRead(stream, this->frames.data(), count * sizeof(VmdFrameData));
		
		vitriol::streamRead(stream, count);
		this->morphs.resize(count);
		vitriol::streamRead(stream, this->morphs.data(), count * sizeof(VmdMorphData));

		
	}
}
