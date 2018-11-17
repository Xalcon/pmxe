#include "PmxHelper.hpp"
#include "PmxJoints.hpp"

namespace vitriol
{
	void PmxJointData::Parse(std::istream* stream, PmxGlobalSettings settings)
	{
		auto loc = stream->tellg();
		this->nameLocal = ReadString(stream, settings.textEncoding);
		this->nameUniversal = ReadString(stream, settings.textEncoding);
		stream->read(reinterpret_cast<char*>(&this->type), sizeof(this->type));
		this->rigidBodyIndexA = ReadIndex(stream, settings.rigidBodyIndexSize);
		this->rigidBodyIndexB = ReadIndex(stream, settings.rigidBodyIndexSize);

		stream->read(reinterpret_cast<char*>(&this->position), sizeof(this->position));
		stream->read(reinterpret_cast<char*>(&this->rotation), sizeof(this->rotation));
		stream->read(reinterpret_cast<char*>(&this->positionMin), sizeof(this->positionMin));
		stream->read(reinterpret_cast<char*>(&this->positionMax), sizeof(this->positionMax));
		stream->read(reinterpret_cast<char*>(&this->rotationMin), sizeof(this->rotationMin));
		stream->read(reinterpret_cast<char*>(&this->rotationMax), sizeof(this->rotationMax));
		stream->read(reinterpret_cast<char*>(&this->positionSpring), sizeof(this->positionSpring));
		stream->read(reinterpret_cast<char*>(&this->rotationSpring), sizeof(this->rotationSpring));
	}
}
