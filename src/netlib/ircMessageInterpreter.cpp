#include "ircMessageInterpreter.h"

void IRCMessageInterpreter::RegisterMessage(const std::string& command,
                                            const std::vector<std::string>& params,
                                            const IRCMessageCallback& callback,
                                            const IRCMessageValidator& validator)
{
    if (command != Utils::StringUpperCase(command))
    {
        Logger::Print("Command must be uppercase.\n");
        return;
    }

    if (m_messageSpecs.find(command) != m_messageSpecs.end())
    {
        Logger::Print("Command already registered.\n");
        return;
    }

    auto specs = CreateRef<IRCMessageSpecs>();
    specs->params = params;
    specs->callback = callback;
    specs->validator = validator;
    specs->command = command;
    m_messageSpecs[command] = specs;
}

void IRCMessageInterpreter::Interpret(const std::string& msg,
    const std::function<void(const std::string&)>& replyHandler,
    const std::function<void(const std::string&)>& defaultHandler)
{
    if (msg.length() == 0)
    {
        return;
    }

    auto tokens = Utils::StringSplit(msg, " ");
    if (tokens.size() == 0)
    {
        return;
    }

    auto command = tokens[0];
    
    if (m_messageSpecs.find(command) == m_messageSpecs.end())
    {
        defaultHandler(msg);
        return;
    }

    auto specs = m_messageSpecs[command];

    IRCMessageArgs args;
    for (size_t i = 1; i < tokens.size(); i++)
    {
        args.push_back(tokens[i]);
    }

    if (args.size() < specs->params.size())
    {
        Logger::Print("Not enough parameters.\n");
        return;
    }

    const auto validationResult = specs->validator(args);

    if (validationResult == RPL_CODES.at("PIPE_VALID"))
    {
        replyHandler(specs->callback(args));
    }
    else
    {
        replyHandler(validationResult);
    }
}