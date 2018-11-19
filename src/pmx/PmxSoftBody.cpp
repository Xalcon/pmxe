#include "PmxSoftBody.hpp"
#include "PmxHelper.hpp"

namespace vitriol
{
	void PmxSoftBodyAnchorRigidBody::Parse(std::istream* stream, PmxGlobalSettings settings)
	{
		this->rigidBodyIndex = ReadIndex(stream, settings.rigidBodyIndexSize);
		this->vertexIndex = ReadIndex(stream, settings.vertexIndexSize);
		stream->read(reinterpret_cast<char*>(&this->nearMode), sizeof(this->nearMode));

	}

	void PmxSoftBodyVertexPin::Parse(std::istream* stream, PmxGlobalSettings settings)
	{
		this->vertexIndex = ReadIndex(stream, settings.vertexIndexSize);
	}

	void PmxSoftBodyData::Parse(std::istream* stream, PmxGlobalSettings settings)
	{
		this->nameLocal = ReadString(stream, settings.textEncoding);
		this->nameUniversal = ReadString(stream, settings.textEncoding);

		stream->read(reinterpret_cast<char*>(&this->shapeType), sizeof(this->shapeType));
		this->materialIndex = ReadIndex(stream, settings.materialIndexSize);
		stream->read(reinterpret_cast<char*>(&this->group), sizeof(this->group));
		stream->read(reinterpret_cast<char*>(&this->noCollisionMask), sizeof(this->noCollisionMask));
		stream->read(reinterpret_cast<char*>(&this->flags), sizeof(this->flags));
		stream->read(reinterpret_cast<char*>(&this->bLinkCreateDistance), sizeof(this->bLinkCreateDistance));
		stream->read(reinterpret_cast<char*>(&this->nClusters), sizeof(this->nClusters));
		stream->read(reinterpret_cast<char*>(&this->totalMass), sizeof(this->totalMass));
		stream->read(reinterpret_cast<char*>(&this->collisionMargin), sizeof(this->collisionMargin));
		stream->read(reinterpret_cast<char*>(&this->aeroModel), sizeof(this->aeroModel));
		stream->read(reinterpret_cast<char*>(&this->config), sizeof(this->config));

		int count;
		stream->read(reinterpret_cast<char*>(&count), sizeof(count));
		this->anchorRigidBodies.reserve(count);
		for (auto i = 0; i < count; i++)
		{
			PmxSoftBodyAnchorRigidBody anchorRB;
			anchorRB.Parse(stream, settings);
			this->anchorRigidBodies.emplace_back(anchorRB);
		}

		stream->read(reinterpret_cast<char*>(&count), sizeof(count));
		this->vertexPins.reserve(count);
		for (auto i = 0; i < count; i++)
		{
			PmxSoftBodyVertexPin vertexPin;
			vertexPin.Parse(stream, settings);
			this->vertexPins.emplace_back(vertexPin);
		}
	}
}
