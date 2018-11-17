#include "PmxRigidBody.hpp"

namespace vitriol
{
	void PmxRigidBodyData::Parse(std::istream* stream, PmxGlobalSettings settings)
	{
		auto loc = stream->tellg();
		this->nameLocal = ReadString(stream, settings.textEncoding);
		this->nameUniversal = ReadString(stream, settings.textEncoding);
		this->boneIndex = ReadIndex(stream, settings.boneIndexSize);
		stream->read(reinterpret_cast<char*>(&this->groupId), sizeof(this->groupId));
		stream->read(reinterpret_cast<char*>(&this->nonCollisionMask), sizeof(this->nonCollisionMask));
		stream->read(reinterpret_cast<char*>(&this->shapeType), sizeof(this->shapeType));
		stream->read(reinterpret_cast<char*>(&this->shapeSize), sizeof(this->shapeSize));
		stream->read(reinterpret_cast<char*>(&this->shapePosition), sizeof(this->shapePosition));
		stream->read(reinterpret_cast<char*>(&this->shapeRotation), sizeof(this->shapeRotation));
		stream->read(reinterpret_cast<char*>(&this->mass), sizeof(this->mass));
		stream->read(reinterpret_cast<char*>(&this->moveAttenuation), sizeof(this->moveAttenuation));
		stream->read(reinterpret_cast<char*>(&this->rotationDampening), sizeof(this->rotationDampening));
		stream->read(reinterpret_cast<char*>(&this->repulsion), sizeof(this->repulsion));
		stream->read(reinterpret_cast<char*>(&this->frictionForce), sizeof(this->frictionForce));
		stream->read(reinterpret_cast<char*>(&this->physicsMode), sizeof(this->physicsMode));
	}
}
