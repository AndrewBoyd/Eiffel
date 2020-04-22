#include "guid.h"
#include <sstream>
#include <random>
#include <string>
#include <fmt/format.h>

namespace guid
{
    unsigned char randomByte() 
    {
        static auto rng = std::mt19937(std::random_device()());
        static auto dist = std::uniform_int_distribution<>(0, 255);
        return dist(rng);
    }

    char asHexNibble(char nibble)
    {
        if (nibble > 9) return 'A' + (nibble - 10);
        return '0' + nibble;
    }

    void asHex(std::stringstream& stream, unsigned char byte) 
    {
        stream << asHexNibble(byte / 16);
        stream << asHexNibble(byte % 16);
    }

    std::string generateHex(int num_bytes) {
        auto stream = std::stringstream();
        for (auto i = 0; i < num_bytes; i++) {
            auto const byte = randomByte();
            asHex(stream, byte);
        }
        return stream.str();
    }

    Guid generateGuid() 
    {
        auto internal = fmt::format("{}-{}-{}-{}-{}",
            generateHex(4),
            generateHex(2),
            generateHex(2),
            generateHex(2),
            generateHex(6));

        return fmt::format("{}{}{}", "{", internal, "}");
    }
}
