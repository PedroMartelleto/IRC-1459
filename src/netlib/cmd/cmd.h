#pragma once

#include "../commons.h"

enum class CommandResult : int
{
    SUCCESS = 0,
    PRINT_HELP = 1,
    TERMINATE = 2
};

using CommandArgs = std::vector<std::string>;
using CommandCallback = std::function<CommandResult(const CommandArgs&)>;

struct CommandSpecs
{
    static const CommandSpecs Empty;

    std::string name;
    std::vector<std::string> argNames;
    std::string description;
    CommandCallback callback;
    int minArgs;

    bool operator==(const CommandSpecs& other) const;
    void PrintUsage() const;
    void PrintNameAndDescription() const;
    void PrintAllInfo() const;
};

struct Command
{
    static const Command Invalid;

    Ref<CommandSpecs> specs;
    CommandArgs args;

    CommandResult Run();
};