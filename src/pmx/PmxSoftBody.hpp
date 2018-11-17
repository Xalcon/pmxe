#pragma once
#include <vector>
#include "PmxGlobalSettings.hpp"

namespace vitriol
{
	enum class PmxSoftBodyShapeType : uint8_t
	{
		TriMesh = 0,
		Rope = 1
	};

	enum class PmxSoftBodyFlags : uint8_t
	{
		BLink = 0,
		ClusterCreation = 1,
		LinkCrossing = 2
	};

	enum class PmxSoftBodyAeroDynamicsModel : int32_t
	{
		VPoint = 0,
		VPointTwoSided = 1,
		VPointOneSided = 2,
		TPointTwoSided = 3,
		TPointOneSided = 4,
	};

	class PmxSoftBodyAnchorRigidBody
	{
	public:
		int32_t rigidBodyIndex;
		int32_t vertexIndex;
		uint8_t nearMode;
		void Parse(std::istream* stream, PmxGlobalSettings settings);
	};

	class PmxSoftBodyVertexPin
	{
	public:
		int32_t vertexIndex;
		void Parse(std::istream* stream, PmxGlobalSettings settings);
	};

	typedef struct PmxSoftBodyConfig
	{
		float cfgVCF; // Velocities correction factor (Baumgarte)
		float cfgDP; // Damping coefficient
		float cfgDG; // Drag coefficient
		float cfgLF; // Lift coefficient
		float cfgPR; // Pressure coefficient
		float cfgVC; // Volume conversation coefficient
		float cfgDF; // Dynamic friction coefficient
		float cfgMT; // Pose matching coefficient
		float cfgCHR; // Rigid contacts hardness
		float cfgKHR; // Kinetic contacts hardness
		float cfgSHR; // Soft contacts hardness
		float cfgAHR; // Anchors hardness
		float clusterSRHR_CL; // Soft vs rigid hardness
		float clusterSKHR_CL; // Soft vs kinetic hardness
		float clusterSSHR_CL; // Soft vs soft hardness
		float clusterSR_SPLT_CL; // Soft vs rigid impulse split
		float clusterSK_SPLT_CL; // Soft vs kinetic impulse split
		float clusterSS_SPLT_CL; // Soft vs soft impulse split
		int iterationV_IT; // Velocities solver iterations
		int iterationP_IT; // Positions solver iterations
		int iterationD_IT; // Drift solver iterations
		int iterationC_IT; // Cluster solver iterations
		int materialLST; // Linear stiffness coefficient
		int materialAST; // Area / Angular stiffness coefficient
		int materialVST; // Volume stiffness coefficient
	} PmxSoftBodyConfig;

	class PmxSoftBodyData
	{
	public:
		std::string nameLocal;
		std::string nameUniversal;
		PmxSoftBodyShapeType shapeType;
		int32_t materialIndex;
		uint8_t group;
		uint16_t noCollisionMask;
		PmxSoftBodyFlags flags;
		int32_t bLinkCreateDistance;
		int32_t nClusters;
		float totalMass;
		float collisionMargin;
		PmxSoftBodyAeroDynamicsModel aeroModel;
		PmxSoftBodyConfig config;
		std::vector<PmxSoftBodyAnchorRigidBody> anchorRigidBodies;
		std::vector<PmxSoftBodyVertexPin> vertexPins;

		void Parse(std::istream* stream, PmxGlobalSettings settings);
	};
}
