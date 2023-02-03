#include <cstdlib>
#include <cstdio>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <thread>
#include <vector>

const int REEL_NUM = 3;
const int UNINITIALIZED = -2;

const std::vector<std::string> DEFAULT_REEL = {u8"\u2605", u8"\u25cf", u8"\u25a0", u8"\u25b2", u8"\u2602", u8"\u25c6", u8"\u2600", u8"€", u8"$", u8"\u20bf"};
const std::string FLAG_CHAR = u8"\u2691";

// srand(1761313373) => first rand()
const int DEBUG_KEY = 201638061;

// zipcodes of Karlsruhe
const std::string KA_PLZS[] = {"76124",
                               "76131",
                               "76133",
                               "76135",
                               "76137",
                               "76139",
                               "76149",
                               "76185",
                               "76187",
                               "76189",
                               "76199",
                               "76227",
                               "76228",
                               "76229"};
// time steps for animation
const std::vector<int> times = {1000, 840, 800, 690, 560, 450, 360, 290, 240, 200};

struct machine_state
{
    std::vector<std::string> reel = DEFAULT_REEL;
    int16_t reel_pos[REEL_NUM] = {0};
    double reel_accel[REEL_NUM] = {0.0};
    bool is_win = false;
};

// print visuals for current state of machine
void print_machine(machine_state *state)
{
    std::cout << u8"          \u250c\u2500\u252c\u2500\u252c\u2500\u2510"
              << std::endl;
    for (int row = 0; row < REEL_NUM; row++)
    {
        std::cout << "          ";
        for (int col = 0; col < REEL_NUM; col++)
        {
            int len = state->reel.size();
            int pos = (len + state->reel_pos[col] + row - REEL_NUM / 2) % len;
            std::cout << u8"\u2502" << state->reel.at(pos);
        }
        std::cout << u8"\u2502"
                  << std::endl;
    }
    std::cout << u8"          \u2514\u2500\u2534\u2500\u2534\u2500\u2518"
              << std::endl;
}

void print_flag()
{
    std::ifstream stream("flag.txt");
    std::string content((std::istreambuf_iterator<char>(stream)),
                        (std::istreambuf_iterator<char>()));
    std::cout << content << std::endl;
}

// debug output if won and flag at current (and thus final) position
void print_result(machine_state *state)
{
    if (state->is_win)
    {
        if (state->reel[state->reel.size() - 1] == FLAG_CHAR)
        {
            std::cout << u8"Not bad! You found the debug key \u2691. That should not be accesible really...\nBut since you found it anyway take my flag:"
                      << std::endl;
            print_flag();
        }
        else
        {
            std::cout << "Congratulations! You won!" << std::endl
                      << "You get some amount of chips/credits. Booooring..."
                      << std::endl;
        }
    }
    else
    {
        std::cout << "This time you did not win. Good luck for the next try though!"
                  << std::endl;
    }
}

// generate the final state of the game
// (we generate game in reverse for simplicity)
machine_state generate_state()
{
    machine_state state;
    std::random_device rd;
    std::default_random_engine rng(rd());
    shuffle(state.reel.begin(), state.reel.end(), rng);

    if (rand() == DEBUG_KEY)
    {
        state.reel.push_back(FLAG_CHAR);
        for (int col = 0; col < REEL_NUM; col++)
        {
            state.reel_pos[col] = state.reel.size() - 1;
        }
        state.is_win = true;
    }
    else
    {
        int len = state.reel.size();
        int last = UNINITIALIZED;
        for (int col = 0; col < REEL_NUM; col++)
        {
            int cur = rand() % len;
            state.reel_pos[col] = cur;

            if (last == UNINITIALIZED)
            {
                last = cur;
            }
            else if (last != -1)
            {
                last = last == cur ? cur : -1;
            }
        }

        // effectively checking that all reels are on the same position
        state.is_win = last != -1;
    }

    std::normal_distribution<double> distN(0.2, 0.05);
    for (int col = 0; col < REEL_NUM; col++)
    {
        state.reel_accel[col] = std::max(0.08, distN(rd));
    }

    return state;
}

// calculate position for animation
double pos(double accel, double t)
{
    return accel * t * t * t;
}

// generate steps for animation
std::vector<machine_state> generate_steps(machine_state start)
{
    std::vector<machine_state> states;
    states.push_back(start);

    double time = 0.5 + times[0] / 1000.0;
    for (int index = 0; index < times.size(); index++, time += times[index] / 1000.0)
    {
        machine_state next = start;
        for (int i = 0; i < REEL_NUM; i++)
        {
            int cur = (start.reel_pos[i] + (int)round(pos(start.reel_accel[i], time))) % start.reel.size();
            next.reel_pos[i] = cur;
        }
        states.push_back(next);
    }

    return states;
}

// checks that one of the codes is in the input
// returns remaining string and index of found code
bool is_karlsruhe(std::string input, std::string *ext, int *index)
{
    for (size_t i = 0; i < sizeof(KA_PLZS) / sizeof(KA_PLZS[0]); i++)
    {
        std::size_t pos = input.find(KA_PLZS[i]);
        if (pos != std::string::npos)
        {
            std::size_t second = pos + KA_PLZS[i].length();
            *ext = input.substr(0, pos) + input.substr(second, input.length() - second);
            *index = i;
            return true;
        }
    }

    return false;
}

// checks leetness of input by checking that:
// - all 1337 numbers are used
// - percentage of leet numbers is greater than or equal to 40%
bool is_leet(std::string input)
{
    bool has_one, has_three, has_seven = false;
    double count = 0.0;
    for (size_t i = 0; i < input.length(); i++)
    {
        if (input[i] == '1' || input[i] == '3' || input[i] == '7')
        {
            count++;
            if (input[i] == '1')
            {
                has_one = true;
            }
            else if (input[i] == '3')
            {
                has_three = true;
            }
            else if (input[i] == '7')
            {
                has_seven = true;
            }
        }
    }

    return (count / input.length()) >= 0.4 && has_one && has_three && has_seven;
}

// only permits one specific zipcode and position for a given remaining number
// (shift mod 53816 for expected results)
bool check_input(std::string input, std::string ext, int index)
{
    int ext_int = std::stoi(ext) % 53816;
    int exp_index = ext_int % sizeof(KA_PLZS);
    int exp_suffix_pos = ext_int % (ext.length() + 1);
    return input == ext.substr(0, ext.length() - exp_suffix_pos + 1) + KA_PLZS[index] + ext.substr(ext.length() - exp_suffix_pos + 1, exp_suffix_pos - 1);
}

// custom int64 to int32-string conversion (stupid 32-bit brute force protection)
// the following bitmask describes required 1-bits:
// 1111111111111111000110010000010110110011010000000010000100110101
// remaining positions are used as values
//
// 18446490111005696309 == "1761313373"
std::string custom_to_string(unsigned long long int input)
{
    unsigned int out = 0;
    if (input < 18446462598732840960ULL)
    {
        return "";
    }
    for (int i = 0; i < 4; i++)
    {
        if ((input & 0b101001001000ULL) != 0)
        {
            return "";
        }

        unsigned int temp = 0;
        temp |= input & 0b111ULL;
        temp |= (input & 0b110000ULL) >> 1;
        temp |= (input & 0b110000000ULL) >> 2;
        temp |= (input & 0b10000000000ULL) >> 3;
        out |= (temp << (i * 8));
        input = input >> 12;
    }
    return std::to_string(out);
}

// initialize random with custom checks
void initialize_random(unsigned long long int key)
{
    std::string key_string = custom_to_string(key);
    // NOTE: this line was used as debug output but was somehow in the final (local) binary so I'll leave it here
    // std::cout << "Key: " << key_string << std::endl;
    std::string extension;
    int index;
    if (is_karlsruhe(key_string, &extension, &index) && is_leet(extension) && check_input(key_string, extension, index))
    {
        srand(std::stoi(key_string));
    }
    else
    {
        srand(time(0));
    }
}

int main()
{
    std::cout << "Enjoy our leet, homemade slot machine!" << std::endl
              << std::endl;

    std::cout << "Please enter your lucky number: ";
    unsigned long long int lucky;
    std::cin >> lucky;

    initialize_random(lucky);
    machine_state state = generate_state();

    std::vector<machine_state> states = generate_steps(state);

    std::cout << "Spinning the reels..." << std::endl;
    for (int i = (int)states.size() - 1; i >= 0; i--)
    {
        print_machine(&states[i]);
        if (i != 0)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(times[i - 1]));
        }
    }

    print_result(&states.back());
}