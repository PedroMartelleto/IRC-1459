#include "cmd.h"

const CommandSpecs CommandSpecs::Empty = CommandSpecs{"", {}, "", {}, -1};
const Command Command::Invalid = Command{nullptr, {}};

bool CommandSpecs::operator==(const CommandSpecs& other) const
{
    return name == other.name;
}

void CommandSpecs::PrintUsage() const
{
    std::cout << "Usage: " << name;

    for (const auto& arg : argNames)
    {
        std::cout << " [" << arg << "]";
    }

    std::cout << std::endl;
}

void CommandSpecs::PrintNameAndDescription() const
{
    std::cout << name << " " << description << std::endl;
}

void CommandSpecs::PrintAllInfo() const
{
    std::cout << "Command " << name << std::endl;
    PrintUsage();
    std::cout << description << std::endl;
}

CommandResult Command::Run()
{
    return specs->callback(args);
}