#pragma once

#include "ircMessageInterpreter.h"
#include "server.h"

class ServerInterpreter
{
public:
    ServerInterpreter(Server* server, const Ref<ConnectedClient>& client);

    void Interpret(const std::string& msg);
    void RegisterMessages();
private:
    Server* m_server;
    Ref<ConnectedClient> m_client;
    IRCMessageInterpreter m_interpreter;

    // Internal interpreter state
    bool m_hasTemporaryNickname = true;

};