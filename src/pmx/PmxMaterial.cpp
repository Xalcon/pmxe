#include "PmxMaterial.hpp"
#include "PmxHelper.hpp"
#include "../StreamHelper.hpp"

namespace vitriol
{
	void PmxMaterial::Parse(std::istream& stream, PmxGlobalSettings settings)
	{
		this->nameLocal = readString(stream, settings.textEncoding);
		this->nameUniversal = readString(stream, settings.textEncoding);
		stream.read(reinterpret_cast<char*>(&this->diffuseColor), sizeof(this->diffuseColor));
		stream.read(reinterpret_cast<char*>(&this->specularColor), sizeof(this->specularColor));
		stream.read(reinterpret_cast<char*>(&this->specularStrength), sizeof(this->specularStrength));
		stream.read(reinterpret_cast<char*>(&this->ambientColor), sizeof(this->ambientColor));
		stream.read(reinterpret_cast<char*>(&this->flags), sizeof(this->flags));
		stream.read(reinterpret_cast<char*>(&this->edgeColor), sizeof(this->edgeColor));
		stream.read(reinterpret_cast<char*>(&this->edgeScale), sizeof(this->edgeScale));
		this->textureIndex = readIndex(stream, settings.textureIndexSize);
		this->environmentIndex = readIndex(stream, settings.textureIndexSize);
		stream.read(reinterpret_cast<char*>(&this->environmentBlendMode), sizeof(this->environmentBlendMode));
		stream.read(reinterpret_cast<char*>(&this->toonReference), sizeof(this->toonReference));

		const int toonValueSize = this->toonReference == 1 ? sizeof(uint8_t) : settings.textureIndexSize;
		stream.read(reinterpret_cast<char*>(&this->toonValue), toonValueSize);
		this->note = readString(stream, settings.textEncoding);
		stream.read(reinterpret_cast<char*>(&this->surfaceCount), sizeof(this->surfaceCount));
	}

	void PmxMaterial::Save(std::ostream& stream, PmxGlobalSettings settings)
	{
		writeString(stream, this->nameLocal, settings.textEncoding);
		writeString(stream, this->nameUniversal, settings.textEncoding);
		streamWrite(stream, this->diffuseColor);
		streamWrite(stream, this->specularColor);
		streamWrite(stream, this->specularStrength);
		streamWrite(stream, this->ambientColor);
		streamWrite(stream, this->flags);
		streamWrite(stream, this->edgeColor);
		streamWrite(stream, this->edgeScale);
		writeIndex(stream, this->textureIndex, settings.textureIndexSize);
		writeIndex(stream, this->environmentIndex, settings.textureIndexSize);
		streamWrite(stream, this->environmentBlendMode);
		streamWrite(stream, this->toonReference);
		if (this->toonReference == 0)
			writeIndex(stream, this->toonValue, settings.textureIndexSize);
		else
			streamWrite(stream, static_cast<uint8_t>(this->toonValue));
		writeString(stream, this->note, settings.textEncoding);
		streamWrite(stream, this->surfaceCount);
	}
}
