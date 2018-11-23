#include "PmxHelper.hpp"
#include "PmxJoints.hpp"
#include "../StreamHelper.hpp"

namespace vitriol
{
	void PmxJointData::Parse(std::istream& stream, PmxGlobalSettings settings)
	{
		this->nameLocal = readString(stream, settings.textEncoding);
		this->nameUniversal = readString(stream, settings.textEncoding);
		stream.read(reinterpret_cast<char*>(&this->type), sizeof(this->type));
		this->rigidBodyIndexA = readIndex(stream, settings.rigidBodyIndexSize);
		this->rigidBodyIndexB = readIndex(stream, settings.rigidBodyIndexSize);

		stream.read(reinterpret_cast<char*>(&this->position), sizeof(this->position));
		stream.read(reinterpret_cast<char*>(&this->rotation), sizeof(this->rotation));
		stream.read(reinterpret_cast<char*>(&this->positionMin), sizeof(this->positionMin));
		stream.read(reinterpret_cast<char*>(&this->positionMax), sizeof(this->positionMax));
		stream.read(reinterpret_cast<char*>(&this->rotationMin), sizeof(this->rotationMin));
		stream.read(reinterpret_cast<char*>(&this->rotationMax), sizeof(this->rotationMax));
		stream.read(reinterpret_cast<char*>(&this->positionSpring), sizeof(this->positionSpring));
		stream.read(reinterpret_cast<char*>(&this->rotationSpring), sizeof(this->rotationSpring));
	}

	void PmxJointData::Save(std::ostream& stream, PmxGlobalSettings settings)
	{
		writeString(stream, this->nameLocal, settings.textEncoding);
		writeString(stream, this->nameUniversal, settings.textEncoding);
		streamWrite(stream, this->type);
		writeIndex(stream, this->rigidBodyIndexA, settings.rigidBodyIndexSize);
		writeIndex(stream, this->rigidBodyIndexB, settings.rigidBodyIndexSize);

		streamWrite(stream, this->position);
		streamWrite(stream, this->rotation);
		streamWrite(stream, this->positionMin);
		streamWrite(stream, this->positionMax);
		streamWrite(stream, this->rotationMin);
		streamWrite(stream, this->rotationMax);
		streamWrite(stream, this->positionSpring);
		streamWrite(stream, this->rotationSpring);
	}
}
