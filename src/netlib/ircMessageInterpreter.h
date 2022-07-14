#pragma once

#include "commons.h"

// auto Pipe = [](auto a, auto b){ return [&](auto c){ return b(a(c)); }; };

using IRCMessageArgs = std::vector<std::string>;
using IRCMessageCallback = std::function<std::string(const IRCMessageArgs&)>;
using IRCMessageValidator = std::function<std::string(const IRCMessageArgs&)>;

struct IRCMessageSpecs
{
    std::string command;
    std::vector<std::string> params;
    IRCMessageCallback callback;
    IRCMessageValidator validator;
};

class IRCMessageInterpreter
{
public:
    /**
     * @brief Registers a new message specification for the interpreter.
     * 
     * @param command The name of the message in upper case.
     * @param params The name of the parameters of the message.
     * @param callback Callback called when the arguments are valid.
     * @param validator A validation pipe called before the callback.
     */
    void RegisterMessage(const std::string& command,
                         const std::vector<std::string>& params,
                         const IRCMessageCallback& callback,
                         const IRCMessageValidator& validator);

    /**
     * @brief Interprets the given message, and calls the correct command.
     * 
     * @param msg Message string to interpret.
     * @param replyHandler Callback called after finishing the command.
     */
    void Interpret(const std::string& msg,
        const std::function<void(const std::string&)>& replyHandler,
        const std::function<void(const std::string&)>& defaultHandler);
private:
    std::map<std::string, Ref<IRCMessageSpecs>> m_messageSpecs;
};