#pragma once
#include <cstdint>
#include <string>
#include "PmxHelper.hpp"
#include "PmxGlobalSettings.hpp"

namespace vitriol
{
	enum class PmxRigidBodyShapeType : uint8_t
	{
		Sphere = 0,
		Box = 1,
		Capsule = 2
	};

	enum class PmxPhysicsMode : uint8_t
	{
		FollowBone = 0,
		Physics = 1,
		PhysicsAndBone = 2
	};

	class PmxRigidBodyData
	{
	public:
		std::string nameLocal;
		std::string nameUniversal;
		int32_t boneIndex;
		uint8_t groupId;
		uint16_t nonCollisionMask;
		PmxRigidBodyShapeType shapeType;
		Vec3 shapeSize;
		Vec3 shapePosition;
		Vec3 shapeRotation;
		float mass;
		float moveAttenuation;
		float rotationDampening;
		float repulsion;
		float frictionForce;
		PmxPhysicsMode physicsMode;

		void Parse(std::istream* stream, PmxGlobalSettings settings);
	};
}
