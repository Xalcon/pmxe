#pragma once
#include <cstdint>
#include <istream>
#include <vector>
#include "PmxGlobalSettings.hpp"

namespace vitriol
{

	enum class PmxMorphType : uint8_t
	{
		Group = 0,
		Vertex = 1,
		Bone = 2,
		UV = 3,
		UVExt1 = 4,
		UVExt2 = 5,
		UVExt3 = 6,
		UVExt4 = 7,
		Material = 8,
		// 2.1
		Flip = 9,
		Impulse = 10
	};

	enum class PmxMorphPanelType : uint8_t
	{
		Hidden = 0, // None
		Eyebrows = 1, // Bottom Left
		Eyes = 2, // Top Left
		Mouth = 3, // Top right
		Other = 4 // Bottom right
	};

	class PmxMorphOffsetData
	{
	public:
		virtual ~PmxMorphOffsetData() {}
		virtual void Parse(std::istream* stream, PmxGlobalSettings settings) = 0;
	};

	class PmxGroupMorph : public PmxMorphOffsetData
	{
	public:
		int32_t morphIndex;
		float influence;

		void Parse(std::istream* stream, PmxGlobalSettings settings);
	};

	class PmxVertexMorph : public PmxMorphOffsetData
	{
	public:
		int32_t vertexIndex;
		Vec3 translation;

		void Parse(std::istream* stream, PmxGlobalSettings settings);
	};

	class PmxBoneMorph : public PmxMorphOffsetData
	{
	public:
		int32_t boneIndex;
		Vec3 translation;
		Vec4 rotation;

		void Parse(std::istream* stream, PmxGlobalSettings settings);
	};

	class PmxUVMorph : public PmxMorphOffsetData
	{
	public:
		int32_t vertexIndex;
		Vec4 data;

		void Parse(std::istream* stream, PmxGlobalSettings settings);
	};

	enum class PmxMaterialMorphValueBlendingMode : uint8_t
	{
		Multiplicative = 0, // values from the material morph are multiplied with the model values
		Additive = 1 // values from the material morph are added to the model values
	};

	class PmxMaterialMorph : public PmxMorphOffsetData
	{
	public:
		int32_t materialIndex;
		PmxMaterialMorphValueBlendingMode blendingMode;
		Vec4 diffuse;
		Vec3 specular;
		float specularStrength;
		Vec3 ambient;
		Vec4 edgeColor;
		float edgeSize;
		Vec4 textureTint;
		Vec4 environmentTint;
		Vec4 toonTint;

		void Parse(std::istream* stream, PmxGlobalSettings settings);
	};

	class PmxFlipMorph : public PmxMorphOffsetData
	{
	public:
		int32_t morphIndex;
		float influence;

		void Parse(std::istream* stream, PmxGlobalSettings settings);
	};

	class PmxImpulseMorph : public PmxMorphOffsetData
	{
	public:
		int32_t rigidBodyIndex;
		uint32_t localFlag;
		Vec3 movementSpeed;
		Vec3 rotationTorque;

		void Parse(std::istream* stream, PmxGlobalSettings settings);
	};

	class PmxMorphData// : private IPmxParsable
	{
	public:
		std::string nameLocal;
		std::string nameUniversal;
		PmxMorphPanelType panelType;
		PmxMorphType morphType;
		std::vector<std::unique_ptr<PmxMorphOffsetData>> dataList;

		void Parse(std::istream* stream, PmxGlobalSettings settings);
	};
}
