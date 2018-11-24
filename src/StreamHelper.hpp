#include <iostream>

namespace vitriol
{
    template <class T>
    std::istream& streamRead(std::istream& stream, T& data)
    {
        return stream.read(reinterpret_cast<char*>(&data), sizeof(data));
    };

    template <class T>
    std::istream& streamRead(std::istream& stream, T& data, std::streamsize count)
    {
        return stream.read(reinterpret_cast<char*>(&data), count);
    };

    template <class T>
    std::istream& streamRead(std::istream& stream, T* data, std::streamsize count)
    {
        return stream.read(reinterpret_cast<char*>(data), count);
    };



	template <class T>
	std::ostream& streamWrite(std::ostream& stream, const T& data)
	{
		return stream.write(reinterpret_cast<const char*>(&data), sizeof(data));
	};

	template <class T>
	std::ostream& streamWrite(std::ostream& stream, const T& data, std::streamsize count)
	{
		return stream.write(reinterpret_cast<const char*>(&data), count);
	};

    template <class T>
    std::ostream& streamWrite(std::ostream& stream, T* data, std::streamsize count)
    {
        return stream.write(reinterpret_cast<const char*>(data), count);
    };
}