#pragma once
#include <istream>
#include "PmxTypes.hpp"
#include "PmxGlobalSettings.hpp"

namespace vitriol
{
	class PmxGlobalSettings;

	enum class PmxJointType : uint8_t
	{
		Spring6DOF = 0,
		_6DOF = 1,
		P2P = 2,
		ConeTwist = 3,
		Slider = 4,
		Hinge = 5
	};

	class PmxJointData
	{
	public:
		std::string nameLocal;
		std::string nameUniversal;
		PmxJointType type;
		int32_t rigidBodyIndexA;
		int32_t rigidBodyIndexB;
		Vec3 position;
		Vec3 rotation;
		Vec3 positionMin;
		Vec3 positionMax;
		Vec3 rotationMin;
		Vec3 rotationMax;
		Vec3 positionSpring;
		Vec3 rotationSpring;

		void Parse(std::istream& stream, PmxGlobalSettings settings);
		void Save(std::ostream& stream, PmxGlobalSettings settings);
	};
}
