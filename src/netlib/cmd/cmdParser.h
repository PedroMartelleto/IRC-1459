#pragma once

#include "cmdManager.h"

class CommandParser
{
public:
    static Ref<CommandSpecs> Parse(const std::string& text);
};