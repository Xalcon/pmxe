#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING 1

#include "PmxHelper.hpp"
#include <istream>
#include <locale>
#include <codecvt>
#include "PmxException.hpp"
#include "../StreamHelper.hpp"

namespace vitriol
{
	template <class T> T limit(T value, T min, T max, T fallback)
	{
		return value >= min ? value <= max ? value : fallback : fallback;
	}

#if _MSC_VER >= 1900
	// ReSharper disable CppDeprecatedEntity
	std::wstring_convert<std::codecvt_utf8_utf16<int16_t>, int16_t> convert;
	// ReSharper restore CppDeprecatedEntity

	std::string toUTF8(const std::u16string& str)
	{
		const auto p = reinterpret_cast<const int16_t *>(str.data());
		return convert.to_bytes(p, p + str.size());
	}

	std::u16string toUTF16(const std::string& str)
	{
		const auto p = reinterpret_cast<const char *>(str.data());
		auto x = convert.from_bytes(p, p + str.size());
		return std::u16string(x.begin(), x.end());
	}

#else
	std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;

	std::string toUTF8(const std::u16string& str)
	{
		return convert.to_bytes(str);
	}

	std::u16string toUTF16(const std::string& str)
	{
		const auto p = reinterpret_cast<const char *>(str.data());
		return convert.from_bytes(p, p + str.size());
	}

#endif

	int32_t readIndex(std::istream& stream, uint8_t typeSize)
	{
		uint32_t tmp = 0;
		stream.read(reinterpret_cast<char*>(&tmp), typeSize);
		
		// This is wrong for type4 vertices, since vertices have a upper limit of UINT32_MAX
		// but we treat them like the others with an upper limit of INT32_MAX
		// this means our parser can only properly parse models with less than 2 million vertices
		const uint32_t upperLimit = (2 << (6 + 8 * (typeSize - 1))) - 1;
#pragma warning(suppress: 4146)
		return static_cast<int32_t>(limit(tmp, 0u, upperLimit, -1u));
	}

	std::string readString(std::istream& stream, PmxStringEncoding sourceEncoding)
	{
		uint32_t size;
		stream.read(reinterpret_cast<char*>(&size), sizeof(size));
		if (size == 0) return "";

		if (sourceEncoding == PmxStringEncoding::UTF16LittleEndian)
		{
			if (size % 2 != 0)
				throw PMX_EXCEPTION("UTF16 string with uneven amount of bytes!");

			std::vector<char16_t> buffer;
			buffer.resize(size);
			stream.read(reinterpret_cast<char*>(buffer.data()), size);

			const std::u16string source(buffer.begin(), buffer.end());
			return toUTF8(source);
		}
		
		if (sourceEncoding == PmxStringEncoding::UTF8)
		{
			std::vector<char> buffer;
			buffer.resize(size);

			stream.read(static_cast<char*>(buffer.data()), size);
			return std::string(buffer.begin(), buffer.end());
		}

		throw PMX_EXCEPTION("Invalid string encoding");
	}

	void writeIndex(std::ostream& stream, const int32_t value, const uint8_t typeSize)
	{
		switch(typeSize)
		{
		case 1:
			streamWrite(stream, static_cast<int8_t>(value));
			return;
		case 2:
			streamWrite(stream, static_cast<int16_t>(value));
			return;
		case 4:
			streamWrite(stream, value);
			return;
		default:
			throw PMX_EXCEPTION("Invalid type size");
		}
	}

	void writeString(std::ostream& stream, std::string& str, PmxStringEncoding targetEncoding)
	{
		if(str.length() == 0)
		{
			streamWrite(stream, 0);
			return;
		}

		if(targetEncoding == PmxStringEncoding::UTF16LittleEndian)
		{
			auto uStr = toUTF16(str);
			streamWrite(stream, static_cast<int32_t>(uStr.length()));
			streamWrite(stream, uStr.c_str(), uStr.length());
			return;
		}

		if (targetEncoding == PmxStringEncoding::UTF8)
		{
			streamWrite(stream, static_cast<int32_t>(str.length()));
			streamWrite(stream, str.c_str(), str.length());
			return;
		}

		throw PMX_EXCEPTION("Invalid string encoding");
	}
}
