#include "Helper.hpp"
#include <vector>
#include <string>
#include <locale> 
#include <codecvt> 

std::string Vitriol::ReadString(std::istream* stream, PMX_STR_ENCODING encoding)
{
    size_t size;
    stream->read((char*)&size, sizeof(size_t));

    if(encoding == PMX_STR_ENCODING_UTF16_LE)
    {
        std::vector<char16_t> buffer;
        buffer.reserve(size);

        stream->read((char*)buffer.data(), size);

        std::u16string source(buffer.data(), size);
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
        std::string dst = convert.to_bytes(source);
        return dst;
    }
    else if(encoding == PMX_STR_ENCODING_UTF8)
    {
        std::vector<char> buffer;
        buffer.reserve(size);

        stream->read((char*)buffer.data(), size);
        return std::string(buffer.data(), size);
    }

    throw;
}