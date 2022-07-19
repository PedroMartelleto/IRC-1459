#include "cmd.h"

const CommandCallback CommandSpecs::EmptyCallback = [](const CommandArgs&) { return CommandResult::ERR; };

const CommandSpecs CommandSpecs::Empty = CommandSpecs{"", {}, "", EmptyCallback, 0};
const Command Command::Invalid = Command{std::make_shared<CommandSpecs>(CommandSpecs::Empty), {}};

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
    std::cout << "  /" << name << std::endl;
    std::cout << "\t- " << description << std::endl;
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