#include "serverInterpreter.h"

ServerInterpreter::ServerInterpreter(Server* server, const Ref<ConnectedClient>& client)
{
    m_server = server;
    m_client = client;
    m_interpreter = IRCMessageInterpreter();
}

void ServerInterpreter::RegisterMessages()
{
    // MARK: - Validators

    IRCMessageValidator newNicknameValidator = [this](const IRCMessageArgs& args)
    {
        const auto nickname = args[0];
        if (!Server::IsValidNickname(nickname))
        {
            return ERR_CODES.at("ERR_ERRONEUSNICKNAME");
        }

        if (!m_server->IsNicknameUnique(nickname))
        {
            return ERR_CODES.at("ERR_NICKNAMEINUSE");
        }

        return RPL_CODES.at("PIPE_VALID");
    };

    IRCMessageValidator targetUserExistsValidator = [this](const IRCMessageArgs& args)
    {
        auto nickname = args[0];

        if (m_server->IsNicknameUnique(nickname))
        {
            return ERR_CODES.at("ERR_NOSUCHNICK");
        }

        return RPL_CODES.at("PIPE_VALID");
    };

    IRCMessageValidator noValidator = [](const IRCMessageArgs& args)
    {
        return RPL_CODES.at("PIPE_VALID");
    };


    // MARK: - Message handlers

    m_interpreter.RegisterMessage("NICK", { "nickname" },
        [this](const std::vector<std::string>& args)
        {
            auto nickname = Utils::StringTrim(args[0]);

            m_server->m_clientsMutex.lock();

            auto oldNickname = m_client->nickname;

            if (nickname == oldNickname)
            {
                return ERR_CODES.at("ERR_NICKCOLLISION");
            }

            m_client->nickname = nickname;
            m_server->m_clients[m_client->nickname] = m_client;
            m_server->m_clients.erase(oldNickname);
            m_server->m_clientsMutex.unlock();

            if (m_hasTemporaryNickname)
            {
                m_server->Broadcast(nickname + " has joined the chat.");
            }
            else
            {
                m_server->Broadcast(oldNickname + " changed their nickname to " + nickname + ".");
            }

            m_hasTemporaryNickname = false;

            return RPL_CODES.at("RPL_WELCOME");
        },
        newNicknameValidator
    );

    m_interpreter.RegisterMessage("PING", { },
        [](const std::vector<std::string>& args)
        {
            return "PONG";
        },
        noValidator
    );

    m_interpreter.RegisterMessage("JOIN", { "channel" },
        [this](const std::vector<std::string>& args)
        {
            // auto channel = Utils::StringTrim(args[0]);
            // auto nickname = m_client->nickname;

            // if (m_server->m_channels.find(channel) == m_server->m_channels.end())
            // {
            //     m_server->m_channels[channel] = CreateRef<Channel>(Channel{ channel });
            // }

            // m_server->m_channels[channel]->users[nickname] = nickname;
            // m_server->Broadcast(nickname + " has joined " + channel + ".", nullptr);

            // return RPL_CODES.at("RPL_TOPIC");
            return "";
        },
        noValidator
    );
}

void ServerInterpreter::Interpret(const std::string& msg) 
{
    m_interpreter.Interpret(msg,
        // Error handler
        [this](const std::string& reply)
        {
            // Sends a reply to the client
            m_server->m_clientsMutex.lock();
            m_server->SendToClient(reply, m_client->nickname);
            m_server->m_clientsMutex.unlock();
        },
        // Default message handler
        [this](const std::string& msg)
        {
            // Sends a reply to the client
            m_server->m_clientsMutex.lock();
            m_server->Broadcast(m_client->nickname + ": " + msg);
            m_server->m_clientsMutex.unlock();
        }
    );
}