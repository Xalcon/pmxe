#include <iostream>
#include <fstream>
#include <emscripten.h>
#include "pmx/Helper.hpp"

typedef unsigned char byte;

typedef struct text_s {
	int32_t    size;
	
	union encoding_u {
		uint8_t *    utf8;  // sizeof( uint8_t ) * size
		int16_t *    utf16; // sizeof( int16_t ) * ( size / 2 )
	};
} text_t;

struct PMX_Header
{
    char magic[4];
    float version;
    byte nGLobals;
    byte* globals;
    text_s modelNameLocal;
    text_s modelNameUniversal;
    text_s commentLocal;
    text_s commentUniversal;
};

extern "C" {

    EMSCRIPTEN_KEEPALIVE int ProcessFile(const char* filePath)
    {
        PMX_Header header;

        std::ifstream file;
        file.open(filePath, std::ifstream::in | std::ifstream::binary);

        file.seekg(0, std::ios::end);
        const int totalFileSize = file.tellg();
        file.seekg(0, std::ios::beg);

        file.read((char*)&header, 9);
        header.globals = new byte[header.nGLobals];
        file.read((char*)header.globals, header.nGLobals);

        std::string localName = Vitriol::ReadString(&file, (Vitriol::PMX_STR_ENCODING)header.globals[0]);
        std::string universalName = Vitriol::ReadString(&file, (Vitriol::PMX_STR_ENCODING)header.globals[0]);

        const int len = file.tellg();
        file.close();


        printf("Read %d bytes from %s (Total size: %d bytes)\n", len, filePath, totalFileSize);
        printf("Magic: '%.*s'\n", 4, header.magic);
        printf("Version: %f\n", header.version);
        printf("Globals: %d\n", header.nGLobals);
        for(int i = 0; i < header.nGLobals; i++)
            printf("  Global%d: %d\n", i, header.globals[i]);
        printf("LocalName: %s\n", localName.c_str());
        printf("UniversalName: %s\n", universalName.c_str());

        delete header.globals;
        return 0;
    }
}
