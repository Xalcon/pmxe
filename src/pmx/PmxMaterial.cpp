#include "PmxMaterial.hpp"
#include "PmxHelper.hpp"

namespace vitriol
{
	void PmxMaterial::Parse(std::istream* stream, PmxGlobalSettings settings)
	{
		this->nameLocal = ReadString(stream, settings.textEncoding);
		this->nameUniversal = ReadString(stream, settings.textEncoding);
		stream->read(reinterpret_cast<char*>(&this->diffuseColor), sizeof(this->diffuseColor));
		stream->read(reinterpret_cast<char*>(&this->specularColor), sizeof(this->specularColor));
		stream->read(reinterpret_cast<char*>(&this->specularStrength), sizeof(this->specularStrength));
		stream->read(reinterpret_cast<char*>(&this->ambientColor), sizeof(this->ambientColor));
		stream->read(reinterpret_cast<char*>(&this->flags), sizeof(this->flags));
		stream->read(reinterpret_cast<char*>(&this->edgeColor), sizeof(this->edgeColor));
		stream->read(reinterpret_cast<char*>(&this->edgeScale), sizeof(this->edgeScale));
		stream->read(reinterpret_cast<char*>(&this->textureIndex), settings.textureIndexSize);
		stream->read(reinterpret_cast<char*>(&this->environmentIndex), settings.textureIndexSize);
		stream->read(reinterpret_cast<char*>(&this->environmentBlendMode), sizeof(this->environmentBlendMode));
		stream->read(reinterpret_cast<char*>(&this->toonReference), sizeof(this->toonReference));

		int toonValueSize = this->toonReference == 1 ? sizeof(uint8_t) : settings.textureIndexSize;
		stream->read(reinterpret_cast<char*>(&this->toonValue), toonValueSize);
		this->note = ReadString(stream, settings.textEncoding);
		stream->read(reinterpret_cast<char*>(&this->surfaceCount), sizeof(this->surfaceCount));
	}
}
