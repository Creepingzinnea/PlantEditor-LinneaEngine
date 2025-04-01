#pragma once
#include <random>

class Random
{
public:
    Random() = default;
    ~Random() = default;
    inline static void Initialize()
    {
        myGenerator.seed(myDevice());
    }
    inline static float GetRandomFloat(float min, float max)
    {
        std::uniform_real_distribution<float> dis(min, max);
        return dis(myGenerator);
    }
    inline static int GetRandomInt(int min, int max)
    {
        std::uniform_int_distribution<int> dis(min, max);
        return dis(myGenerator);
    }
private:
    static std::random_device myDevice;
    static std::mt19937 myGenerator;
};