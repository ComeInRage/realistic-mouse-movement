#include "ticks_generator.hpp"

#include <iostream>



namespace rm = real_mouse;

int main()
{
    using namespace std::chrono_literals;
    
    size_t iterations = 0;
    size_t ticks_passed_in_10_iterations = 0;

    for (auto ticks : rm::ticks_generator{ 2ns })
    {
        if (iterations == 10) { break; }

        ++iterations;

        ticks_passed_in_10_iterations += ticks;
    }

    std::cout << std::format("Ticks passed in 10 iterations: {}\nEach tick is 2 ns.", ticks_passed_in_10_iterations) << std::endl;

    return 0;
}