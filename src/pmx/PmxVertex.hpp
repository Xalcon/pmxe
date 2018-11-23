#pragma once
#include "PmxGlobalSettings.hpp"
#include "PmxTypes.hpp"

namespace vitriol
{
	enum class PmxVertexSkinningType : uint8_t
	{
		BDEF1 = 0,
		BDEF2 = 1,
		BDEF4 = 2,
		SDEF = 3,
		QDEF = 4,
	};

	class PmxVertexSkinning
	{
	public:
		virtual ~PmxVertexSkinning() {}
		virtual void Parse(std::istream& stream, PmxGlobalSettings settings) = 0;
		virtual void Save(std::ostream& stream, PmxGlobalSettings settings) = 0;
	};

	class PmxVertexSkinningBDEF1 : public PmxVertexSkinning
	{
	public:
		int32_t boneIndex;

		void Parse(std::istream& stream, PmxGlobalSettings settings) override;
		void Save(std::ostream& stream, PmxGlobalSettings settings) override;
	};

	class PmxVertexSkinningBDEF2 : public PmxVertexSkinning
	{
	public:
		int32_t boneIndex1;
		int32_t boneIndex2;
		// Bone 2 weight = 1.0 - Bone 1 weight
		float bone1Weight;

		void Parse(std::istream& stream, PmxGlobalSettings settings) override;
		void Save(std::ostream& stream, PmxGlobalSettings settings) override;
	};

	class PmxVertexSkinningBDEF4 : public PmxVertexSkinning
	{
	public:
		int32_t boneIndex1;
		int32_t boneIndex2;
		int32_t boneIndex3;
		int32_t boneIndex4;
		// total bone weight not guaranteed to be 1.0
		float bone1Weight;
		float bone2Weight;
		float bone3Weight;
		float bone4Weight;

		void Parse(std::istream& stream, PmxGlobalSettings settings) override;
		void Save(std::ostream& stream, PmxGlobalSettings settings) override;
	};

	// Spherical Deform Blending
	class PmxVertexSkinningSDEF : public PmxVertexSkinning
	{
	public:
		int32_t boneIndex1;
		int32_t boneIndex2;
		float bone1Weight;
		Vec3 c;
		Vec3 r0;
		Vec3 r1;

		void Parse(std::istream& stream, PmxGlobalSettings settings) override;
		void Save(std::ostream& stream, PmxGlobalSettings settings) override;
	};

	// Dual quaternion deform blending, v2.1+
	class PmxVertexSkinningQDEF : public PmxVertexSkinning
	{
	public:
		int32_t boneIndex1;
		int32_t boneIndex2;
		int32_t boneIndex3;
		int32_t boneIndex4;
		// total bone weight not guaranteed to be 1.0
		float bone1Weight;
		float bone2Weight;
		float bone3Weight;
		float bone4Weight;

		void Parse(std::istream& stream, PmxGlobalSettings settings) override;
		void Save(std::ostream& stream, PmxGlobalSettings settings) override;
	};

	class PmxVertex// : private IPmxParsable
	{
	public:
		Vec3 position;
		Vec3 normal;
		Vec2 uv;
		Vec4 additionalData[4];
		PmxVertexSkinningType skinningType;
		std::unique_ptr<PmxVertexSkinning> skinningInfo;
		float edgeScale;

		void Parse(std::istream& stream, PmxGlobalSettings settings);
		void Save(std::ostream& stream, PmxGlobalSettings settings) const;
	};
}
