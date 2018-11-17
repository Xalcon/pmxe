#include <cstdio>
#include <iostream>
#include <fstream>
#include "pmx/Pmx.hpp"

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

#ifndef EMSCRIPTEN_KEEPALIVE
#define EMSCRIPTEN_KEEPALIVE
#endif

extern "C" {
	
    EMSCRIPTEN_KEEPALIVE int ProcessFile(const char* filePath)
    {
        std::ifstream file;
        file.open(filePath, std::ifstream::in | std::ifstream::binary);

        file.seekg(0, std::ios::end);
        const int totalFileSize = file.tellg();
        file.seekg(0, std::ios::beg);

        Vitriol::Pmx pmx;
        pmx.Parse(&file);

        const int len = file.tellg();
        file.close();

        printf("Read %d bytes from %s (Total size: %d bytes)\n", len, filePath, totalFileSize);
        printf("Magic: '%.*s'\n", 4, pmx.magic);
        printf("Version: %.1f\n", pmx.version);
        printf(" - TextEncoding: %hhu\n", pmx.globalSettings.textEncoding);
        printf(" - AdditionalVec4: %hhu\n", pmx.globalSettings.additionalVectors);
        printf(" - vertexIndexSize: %hhu\n", pmx.globalSettings.vertexIndexSize);
        printf(" - textureIndexSize: %hhu\n", pmx.globalSettings.textureIndexSize);
        printf(" - materialIndexSize: %hhu\n", pmx.globalSettings.materialIndexSize);
        printf(" - boneIndexSize: %hhu\n", pmx.globalSettings.boneIndexSize);
        printf(" - morphIndexSize: %hhu\n", pmx.globalSettings.morphIndexSize);
        printf(" - rigidBodyIndexSize: %hhu\n", pmx.globalSettings.rigidBodyIndexSize);
        printf("LocalName: %s\n", pmx.nameLocal.c_str());
        printf("UniversalName: %s\n", pmx.nameUniversal.c_str());
        printf("LocalComment: %s\n", pmx.commentLocal.c_str());
        printf("UniversalComment: %s\n", pmx.commentUniversal.c_str());
        printf("Vertices: %lu\n", pmx.vertices.size());
        int bwt[5];
        for(int i = 0; i < pmx.vertices.size(); i++)
            bwt[static_cast<int>(pmx.vertices[i].skinningType)]++;
        printf("  BDEF1: %d\n  BDEF2: %d\n  BDEF4: %d\n  SDEF: %d\n  QDEF: %d\n", bwt[0], bwt[1], bwt[2], bwt[3], bwt[4]);
        printf("Polygons: %lu\n", pmx.vertexIndices.size() / 3);
        printf("Textures: \n");
        for(int i = 0; i < pmx.textures.size(); i++)
            printf("  %s\n", pmx.textures[i].c_str());
        printf("Materials(%lu): \n", pmx.materials.size());
        for(int i = 0; i < pmx.materials.size(); i++)
            printf("  %d: %s\n", i, pmx.materials[i].localName.c_str());
        printf("Bones(%lu): \n", pmx.bones.size());
        for(auto i = 0; i < pmx.bones.size(); i++)
            printf("  %d: %s\n", i, pmx.bones[i].boneNameLocal.c_str());
        printf("Morphs(%lu): \n", pmx.morphs.size());
        for(auto i = 0; i < pmx.morphs.size(); i++)
            printf("  %d: %s\n", i, pmx.morphs[i].morphNameLocal.c_str());
        return 0;
    }
}

#ifdef  _MSC_VER
#include <Windows.h>
void main()
{
	SetConsoleOutputCP(65001);
	ProcessFile(R"(F:\Dev\Emscripten\SAO ver1.2\SAO\Kirito(GGO)\Kirito_x.pmx)");
	std::cin;
}
#endif