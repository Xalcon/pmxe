#pragma once
#include <istream>
#include <vector>
#include "PmxGlobalSettings.hpp"

namespace vitriol
{
	class PmxGlobalSettings;

	enum class PmxBoneFlags : uint16_t
	{
		IndexedTailPosition = 1 << 0, // Is the tail position a vec3 or bone index	
		Rotatable = 1 << 1, // Enables rotation
		Translatable = 1 << 2, // Is the tail position a vec3 or bone index	
		IsVisible = 1 << 3, // ???
		Enabled = 1 << 4, // ???
		InverseKinematics = 1 << 5, // Use inverse kinematics (physics)	
		InheritRotation = 1 << 8, // Rotation inherits from another bone
		InheritTranslation = 1 << 9, // Translation inherits from another bone
		FixedAxis = 1 << 10, // The bone's shaft is fixed in a direction
		LocalCoordinate = 1 << 11, // ???
		PhysicsAfterDeform = 1 << 12, // ???
		ExternalParentDeform = 1 << 13, // ???
	};

	class PmxParentBone
	{
	public:
		int32_t parentIndex;
		float parentInfluence;

		void Parse(std::istream& stream, PmxGlobalSettings settings);
		void Save(std::ostream& stream, PmxGlobalSettings settings) const;
	};

	class PmxBoneFixedAxis
	{
	public:
		Vec3 axisDirection;

		void Parse(std::istream& stream);
		void Save(std::ostream& stream) const;
	};

	class PmxBoneLocalCoordinate
	{
	public:
		Vec3 x;
		Vec3 z;

		void Parse(std::istream& stream);
		void Save(std::ostream& stream) const;
	};

	class PmxBoneExternalParent
	{
	public:
		int32_t parentIndex;

		void Parse(std::istream& stream, PmxGlobalSettings settings);
		void Save(std::ostream& stream, PmxGlobalSettings settings) const;
	};

	class PmxBoneIkAngleLimit
	{
	public:
		Vec3 min; // minimum angle in radians
		Vec3 max; // maximum angle in radians

		void Parse(std::istream& stream);
		void Save(std::ostream& stream) const;
	};

	class PmxBoneIkLinks
	{
	public:
		int32_t boneIndex;
		uint8_t hasLimits; // When equal to 1, use angle limits
		PmxBoneIkAngleLimit angleLimits; // Used if has limits is 1. See IK Angle Limit

		void Parse(std::istream& stream, PmxGlobalSettings settings);
		void Save(std::ostream& stream, PmxGlobalSettings settings) const;
	};

	class PmxBoneIk
	{
	public:
		int32_t targetIndex;
		int32_t loopCount;
		float limitRadians;
		int32_t linkCount; // How many bones this IK links with
		std::vector<PmxBoneIkLinks> ikLinks;

		void Parse(std::istream& stream, PmxGlobalSettings settings);
		void Save(std::ostream& stream, PmxGlobalSettings settings) const;
	};

	class PmxBoneData// : private IPmxParsable
	{
	public:
		std::string nameLocal;
		std::string nameUniversal;
		Vec3 position; // The local translation of the bone
		int32_t parentBoneIndex;
		int32_t layer; // deformation hierarchy
		PmxBoneFlags flags;
		union { Vec3 tailPosition; int32_t tailPositionBoneIndex; };
		PmxParentBone inheritBone;
		PmxBoneFixedAxis fixedAxis;
		PmxBoneLocalCoordinate localCoordinate;
		PmxBoneExternalParent externalParent;
		PmxBoneIk ikBoneData;

		void Parse(std::istream& stream, PmxGlobalSettings settings);
		void Save(std::ostream& stream, PmxGlobalSettings settings);
	};
}