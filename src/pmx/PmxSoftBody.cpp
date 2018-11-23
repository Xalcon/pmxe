#include "PmxSoftBody.hpp"
#include "PmxHelper.hpp"
#include "../StreamHelper.hpp"

namespace vitriol
{
	void PmxSoftBodyAnchorRigidBody::Parse(std::istream& stream, PmxGlobalSettings settings)
	{
		this->rigidBodyIndex = readIndex(stream, settings.rigidBodyIndexSize);
		this->vertexIndex = readIndex(stream, settings.vertexIndexSize);
		stream.read(reinterpret_cast<char*>(&this->nearMode), sizeof(this->nearMode));
	}

	void PmxSoftBodyAnchorRigidBody::Save(std::ostream& stream, PmxGlobalSettings settings) const
	{
		writeIndex(stream, this->rigidBodyIndex, settings.rigidBodyIndexSize);
		writeIndex(stream, this->vertexIndex, settings.vertexIndexSize);
		streamWrite(stream, this->nearMode);
	}

	void PmxSoftBodyVertexPin::Parse(std::istream& stream, PmxGlobalSettings settings)
	{
		this->vertexIndex = readIndex(stream, settings.vertexIndexSize);
	}

	void PmxSoftBodyVertexPin::Save(std::ostream& stream, PmxGlobalSettings settings) const
	{
		writeIndex(stream, this->vertexIndex, settings.vertexIndexSize);
	}

	void PmxSoftBodyData::Parse(std::istream& stream, PmxGlobalSettings settings)
	{
		this->nameLocal = readString(stream, settings.textEncoding);
		this->nameUniversal = readString(stream, settings.textEncoding);

		stream.read(reinterpret_cast<char*>(&this->shapeType), sizeof(this->shapeType));
		this->materialIndex = readIndex(stream, settings.materialIndexSize);
		stream.read(reinterpret_cast<char*>(&this->group), sizeof(this->group));
		stream.read(reinterpret_cast<char*>(&this->noCollisionMask), sizeof(this->noCollisionMask));
		stream.read(reinterpret_cast<char*>(&this->flags), sizeof(this->flags));
		stream.read(reinterpret_cast<char*>(&this->bLinkCreateDistance), sizeof(this->bLinkCreateDistance));
		stream.read(reinterpret_cast<char*>(&this->nClusters), sizeof(this->nClusters));
		stream.read(reinterpret_cast<char*>(&this->totalMass), sizeof(this->totalMass));
		stream.read(reinterpret_cast<char*>(&this->collisionMargin), sizeof(this->collisionMargin));
		stream.read(reinterpret_cast<char*>(&this->aeroModel), sizeof(this->aeroModel));
		stream.read(reinterpret_cast<char*>(&this->config), sizeof(this->config));

		int32_t count;
		stream.read(reinterpret_cast<char*>(&count), sizeof(count));
		this->anchorRigidBodies.reserve(count);
		for (auto i = 0; i < count; i++)
		{
			PmxSoftBodyAnchorRigidBody anchorRB;
			anchorRB.Parse(stream, settings);
			this->anchorRigidBodies.emplace_back(anchorRB);
		}

		stream.read(reinterpret_cast<char*>(&count), sizeof(count));
		this->vertexPins.reserve(count);
		for (auto i = 0; i < count; i++)
		{
			PmxSoftBodyVertexPin vertexPin;
			vertexPin.Parse(stream, settings);
			this->vertexPins.emplace_back(vertexPin);
		}
	}

	void PmxSoftBodyData::Save(std::ostream& stream, PmxGlobalSettings settings)
	{
		writeString(stream, this->nameLocal, settings.textEncoding);
		writeString(stream, this->nameUniversal, settings.textEncoding);
		streamWrite(stream, this->shapeType);
		writeIndex(stream, this->materialIndex, settings.materialIndexSize);
		streamWrite(stream, this->group);
		streamWrite(stream, this->noCollisionMask);
		streamWrite(stream, this->flags);
		streamWrite(stream, this->bLinkCreateDistance);
		streamWrite(stream, this->nClusters);
		streamWrite(stream, this->totalMass);
		streamWrite(stream, this->collisionMargin);
		streamWrite(stream, this->aeroModel);
		streamWrite(stream, this->config);

		streamWrite(stream, static_cast<int32_t>(this->anchorRigidBodies.size()));
		for (auto& anchorRigidBody : this->anchorRigidBodies)
			anchorRigidBody.Save(stream, settings);

		streamWrite(stream, static_cast<int32_t>(this->vertexPins.size()));
		for (auto& vertexPin : this->vertexPins)
			vertexPin.Save(stream, settings);
	}
}
