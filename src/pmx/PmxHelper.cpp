#include "PmxHelper.hpp"
#include <istream>
#include <locale>
#include <codecvt>
#include "PmxException.h"

namespace vitriol
{
	template <class T> T Limit(T value, T min, T max, T fallback)
	{
		return value >= min ? value <= max ? value : fallback : fallback;
	}


#if _MSC_VER >= 1900

	std::string utf16_to_utf8(std::u16string utf16_string)
	{
		// TODO: find something that is not deprecated :/
		std::wstring_convert<std::codecvt_utf8_utf16<int16_t>, int16_t> convert;
		// ReSharper restore CppDeprecatedEntity
		const auto p = reinterpret_cast<const int16_t *>(utf16_string.data());
		return convert.to_bytes(p, p + utf16_string.size());
	}

#else

	std::string utf16_to_utf8(std::u16string utf16_string)
	{
		// TODO: find something that is not deprecated :/
		std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
		return convert.to_bytes(utf16_string);
	}

#endif

	int32_t ReadIndex(std::istream* stream, uint8_t typeSize)
	{
		uint32_t tmp;
		stream->read(reinterpret_cast<char*>(&tmp), typeSize);

		// This is wrong for type4 vertices, since vertices have a upper limit of UINT32_MAX
		// but we treat them like the others with an upper limit of INT32_MAX
		// this means our parser can only properly parse models with less than 2 million vertices
		const uint32_t upperLimit = (2 << (6 + 8 * (typeSize - 1))) - 1;
		return static_cast<int32_t>(Limit(tmp, 0u, upperLimit, -1u));
	}

	std::string ReadString(std::istream* stream, PmxStringEncoding sourceEncoding)
	{
		uint32_t size;
		stream->read(reinterpret_cast<char*>(&size), sizeof(size));
		if (size == 0) return "";

		if (sourceEncoding == PmxStringEncoding::UTF16LittleEndian)
		{
			if (size % 2 != 0)
				throw PMX_EXCEPTION("UTF16 string with uneven amount of bytes!");

			std::vector<char16_t> buffer;
			buffer.resize(size);
			stream->read(reinterpret_cast<char*>(buffer.data()), size);

			const std::u16string source(buffer.begin(), buffer.end());
			auto dst = utf16_to_utf8(source);
			return dst;
		}
		else if (sourceEncoding == PmxStringEncoding::UTF8)
		{
			std::vector<char> buffer;
			buffer.resize(size);

			stream->read(static_cast<char*>(buffer.data()), size);
			return std::string(buffer.begin(), buffer.end());
		}

		throw PMX_EXCEPTION("Invalid string encoding");
	}
}
