#include "client.h"

std::random_device Client::s_randomDevice;
std::mt19937 Client::s_generator;
std::vector<std::string> Client::s_nicknames;

std::string Client::GenerateNickname()
{
    if (s_nicknames.size() <= 0) {
        s_nicknames = Utils::StringSplit(Utils::LoadFile("./src/nicknames.txt"), "\n");
        s_generator = std::mt19937(s_randomDevice());
    }

    std::string element;
    std::sample(s_nicknames.begin(), s_nicknames.end(), &element, 1, s_generator);

    std::stringstream stream;

    for (int i = 0; i < 3; ++i) {
        stream << std::to_string(Utils::RandomInt(0, 9));
    }

    return element + stream.str();
}