#pragma once
#include <exception>
#include <string>
#include <utility>

namespace vitriol
{
	#define PMX_EXCEPTION(x) PmxException(x, __FILE__, __FUNCTION__, __LINE__)

	class PmxException : public std::runtime_error {
		const char* file;
		const char* func;
		int line;

	public:
		PmxException(const char* const msg, const char* aFile, const char* aFunc, const int aLine)
			: std::runtime_error(msg),
			  file(aFile),
			  func(aFunc),
			  line(aLine) { }

		const char* getFile() const { return file; }
		int getLine() const { return line; }
		const char* getFunc() const { return func; }
	};
}
