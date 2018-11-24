#pragma once
#include <cstdint>
#include "../pmx/PmxBone.hpp"

namespace vmd
{
#pragma pack(push)
#pragma pack(1)

	typedef struct VmdHeader
	{
		char magic[30];
		char name[20];
	} VmdHeader;

	typedef struct VmdFrameData
	{
		uint8_t boneName[15];
		int32_t frameNumber;
		vitriol::Vec3 position;
		vitriol::Vec4 rotation;
		// Interpolation uses 4x 4pt bezier curves which are stored in the following format:
		//	X_ax, Y_ax, Z_ax, R_ax, X_ay, Y_ay, Z_ay, R_ay, X_bx, Y_bx, Z_bx, R_bx, X_by, Y_by, Z_by, R_by,
		//	Y_ax, Z_ax, R_ax, X_ay, Y_ay, Z_ay, R_ay, X_bx, Y_bx, Z_bx, R_bx, X_by, Y_by, Z_by, R_by,   01,
		//	Z_ax, R_ax, X_ay, Y_ay, Z_ay, R_ay, X_bx, Y_bx, Z_bx, R_bx, X_by, Y_by, Z_by, R_by,   01,   00,
		//	R_ax, X_ay, Y_ay, Z_ay, R_ay, X_bx, Y_bx, Z_bx, R_bx, X_by, Y_by, Z_by, R_by,   01,   00,   00
		// (0, 0), (ax, ay), (bx, by), (127, 127)
		// Note: I still dont understand why there is 4 times as much data though :/
		//       is this some kind of matrix?
		char interpolationData[4][4][4];
	} VmdFrameData;

	typedef struct VmdMorphData
	{
		char morphName[15];
		int32_t frameNumber;
		float weight;
	} VmdMorphData;

	typedef struct VmdCameraData
	{
		int32_t frameNumber;
		float length;
		vitriol::Vec3 position;
		vitriol::Vec3 rotation;
		char interpolation[6][4];
		int32_t viewingAngle;
		char perspective; // 0: on, 1: off
	} VmdCameraData;

	typedef struct VmdLightData
	{
		int32_t frameNumber;
		vitriol::Vec3 color;
		vitriol::Vec3 position;
	} VmdLightData;

	typedef struct VmdSelfShadowData
	{
		int32_t frameNumber;
		char mode;
		float distance;
	} VmdSelfShadowData;

#pragma pack(pop)

	class Vmd
	{
	public:
		VmdHeader header;
		std::vector<VmdFrameData> frames;
		std::vector<VmdMorphData> morphs;
		std::vector<VmdCameraData> cameras;
		std::vector<VmdLightData> lights;
		std::vector<VmdSelfShadowData> selfShadows;

		void Parse(std::istream& stream);
	};
}
