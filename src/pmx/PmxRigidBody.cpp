#include "PmxRigidBody.hpp"
#include "../StreamHelper.hpp"

namespace vitriol
{
	void PmxRigidBodyData::Parse(std::istream& stream, PmxGlobalSettings settings)
	{
		this->nameLocal = readString(stream, settings.textEncoding);
		this->nameUniversal = readString(stream, settings.textEncoding);
		this->boneIndex = readIndex(stream, settings.boneIndexSize);
		stream.read(reinterpret_cast<char*>(&this->groupId), sizeof(this->groupId));
		stream.read(reinterpret_cast<char*>(&this->nonCollisionMask), sizeof(this->nonCollisionMask));
		stream.read(reinterpret_cast<char*>(&this->shapeType), sizeof(this->shapeType));
		stream.read(reinterpret_cast<char*>(&this->shapeSize), sizeof(this->shapeSize));
		stream.read(reinterpret_cast<char*>(&this->shapePosition), sizeof(this->shapePosition));
		stream.read(reinterpret_cast<char*>(&this->shapeRotation), sizeof(this->shapeRotation));
		stream.read(reinterpret_cast<char*>(&this->mass), sizeof(this->mass));
		stream.read(reinterpret_cast<char*>(&this->moveAttenuation), sizeof(this->moveAttenuation));
		stream.read(reinterpret_cast<char*>(&this->rotationDampening), sizeof(this->rotationDampening));
		stream.read(reinterpret_cast<char*>(&this->repulsion), sizeof(this->repulsion));
		stream.read(reinterpret_cast<char*>(&this->frictionForce), sizeof(this->frictionForce));
		stream.read(reinterpret_cast<char*>(&this->physicsMode), sizeof(this->physicsMode));
	}

	void PmxRigidBodyData::Save(std::ostream& stream, PmxGlobalSettings settings)
	{
		writeString(stream, this->nameLocal, settings.textEncoding);
		writeString(stream, this->nameUniversal, settings.textEncoding);
		writeIndex(stream, this->boneIndex, settings.boneIndexSize);
		streamWrite(stream, this->groupId);
		streamWrite(stream, this->nonCollisionMask);
		streamWrite(stream, this->shapeType);
		streamWrite(stream, this->shapeSize);
		streamWrite(stream, this->shapePosition);
		streamWrite(stream, this->shapeRotation);
		streamWrite(stream, this->mass);
		streamWrite(stream, this->moveAttenuation);
		streamWrite(stream, this->rotationDampening);
		streamWrite(stream, this->repulsion);
		streamWrite(stream, this->frictionForce);
		streamWrite(stream, this->physicsMode);
	}
}
