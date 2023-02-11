#include <chrono>
#include <iostream>
#include <random>

#define MAX_UINT32 4294967295

// the deployed version used the key I commented out
// const int DEBUG_KEY = 1212832989;
const int DEBUG_KEY = 201638061;

int main(int argc, char const *argv[])
{
    auto t0 = std::chrono::steady_clock::now();
    for (unsigned int i = 0; i < MAX_UINT32; i++)
    {
        srand(i);
        if (rand() == DEBUG_KEY)
        {
            printf("Key: %u\n", i);
            break;
        }
    }
    auto t1 = std::chrono::steady_clock::now();
    auto d = (t1 - t0);
    std::cout << d.count() << "ps\n";
}
