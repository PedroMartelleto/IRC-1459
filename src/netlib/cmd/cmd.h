#pragma once

#include "../commons.h"

using CommandCallback = std::function<void(std::vector<std::string>)>;
using CommandArgs = std::vector<std::string>;

struct CommandSpecs
{
    std::string name;
    std::vector<std::string> args;
    CommandCallback callback;
};