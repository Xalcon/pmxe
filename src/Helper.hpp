namespace Vitriol
{
    template <class T> T Limit(T value, T min, T max, T fallback)
    {
        return value >= min ? value <= max ? value : fallback : fallback;
    }
}