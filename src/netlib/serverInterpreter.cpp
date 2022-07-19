#include "serverInterpreter.h"

IRCMessageValidator Pipe(const IRCMessageValidator& a, const IRCMessageValidator& b)
{
    return [a, b](const IRCMessageArgs& args)
    {
        auto resultA = a(args);
        
        if (resultA != RPL_CODES.at("PIPE_VALID"))
        {
            return resultA;
        }
        else
        {
            return b(args);
        }
    };
};

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

    IRCMessageValidator sourceClientIsAdmin = [this](const IRCMessageArgs& args)
    {
        if (!m_client->isOperator)
        {
            return ERR_CODES.at("ERR_NOPRIVILEGES");
        }

        return RPL_CODES.at("PIPE_VALID");
    };

    IRCMessageValidator targetUserIsInTheSameChannel = [this](const IRCMessageArgs& args)
    {
        auto nickname = args[0];
        const auto& client = m_server->m_clients.at(nickname);

        if (!client->IsInAnyChannel() || client->channel != m_client->channel)
        {
            return ERR_CODES.at("ERR_USERNOTINCHANNEL");
        }

        return RPL_CODES.at("PIPE_VALID");
    };

    IRCMessageValidator channelArg2Exists = [this](const IRCMessageArgs& args)
    {
        auto channel = args[1];
        if (m_server->m_channels.find(channel) == m_server->m_channels.end())
        {
            return ERR_CODES.at("ERR_NOSUCHCHANNEL");
        }

        return RPL_CODES.at("PIPE_VALID");
    };

    IRCMessageValidator channelArg1Exists = [this](const IRCMessageArgs& args)
    {
        auto channel = args[0];
        if (m_server->m_channels.find(channel) == m_server->m_channels.end())
        {
            return ERR_CODES.at("ERR_NOSUCHCHANNEL");
        }

        return RPL_CODES.at("PIPE_VALID");
    };

    IRCMessageValidator channelNameIsValidArg1 = [this](const IRCMessageArgs& args)
    {
        auto channel = args[0];
        
        if (!Server::IsValidChannelName(channel))
        {
            return ERR_CODES.at("ERR_NOSUCHCHANNEL");
        }

        return RPL_CODES.at("PIPE_VALID");
    };

    IRCMessageValidator channelNameIsValidArg2 = [this](const IRCMessageArgs& args)
    {
        auto channel = args[1];
        
        if (!Server::IsValidChannelName(channel))
        {
            return ERR_CODES.at("ERR_NOSUCHCHANNEL");
        }

        return RPL_CODES.at("PIPE_VALID");
    };

    const auto sourceAdminAndTargetExistsValidator = Pipe(sourceClientIsAdmin, targetUserExistsValidator);
    
    const auto sameChannelAdminValidator = Pipe(sourceAdminAndTargetExistsValidator, targetUserIsInTheSameChannel);

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
            m_server->m_clientsMutex.lock();

            auto channel = Utils::StringTrim(args[0]);
            auto nickname = m_client->nickname;
            bool isNewChannel = false;

            // If the channel doesn't exist, create it and set the client as operator.
            if (m_server->m_channels.find(channel) == m_server->m_channels.end())
            {
                m_server->m_channels[channel] = CreateRef<Channel>(Channel{ channel, false, { }, { } });
                isNewChannel = true;
            }

            // Checks if the client is already in the channel.
            auto& currentUsers = m_server->m_channels[channel]->nicknames;

            if (std::find(currentUsers.begin(), currentUsers.end(), nickname) == currentUsers.end())
            {
                return ERR_CODES.at("RPL_NOTOPIC");
            }

            // Checks if the client is authorized to enter the channel.
            if (!m_server->m_channels[channel]->IsAuthorizedToEnter(nickname))
            {
                m_server->m_clientsMutex.unlock();
                return ERR_CODES.at("ERR_INVITEONLYCHAN");
            }

            // Removes the client from their previous channel.
            if (m_client->channel != "")
            {
                Ref<Channel> oldChannel = m_server->m_channels[m_client->channel];
                oldChannel->RemoveUser(m_client->nickname);
            }
            
            m_client->isOperator = isNewChannel;

            // Finally, adds the client to the new channel.
            m_server->m_channels[channel]->nicknames.push_back(m_client->nickname);
            m_server->m_clients[nickname]->channel = channel;
            m_server->BroadcastChannel(nickname + " has joined " + channel + ".", channel);
            
            m_server->m_clientsMutex.unlock();
            return RPL_CODES.at("RPL_NOTOPIC");
            
        },
        channelNameIsValidArg1
    );

    m_interpreter.RegisterMessage("WHOIS", { "nickname" },
        [this](const std::vector<std::string>& args)
        {
            auto nick = args[0];
            auto channel = args[1];
            // TODO: This
            return RPL_CODES.at("RPL_WELCOME");
        },
        sameChannelAdminValidator
    );

    m_interpreter.RegisterMessage("MUTE", { "nickname" },
        [this](const std::vector<std::string>& args)
        {
            m_server->m_clients[args[0]]->isMuted = true;
            return RPL_CODES.at("RPL_MUTED");
        },
        sameChannelAdminValidator
    );

    m_interpreter.RegisterMessage("UNMUTE", { "nickname" },
        [this](const std::vector<std::string>& args)
        {
            m_server->m_clients[args[0]]->isMuted = false;
            return RPL_CODES.at("RPL_UNMUTED");
        },
        sameChannelAdminValidator
    );

    m_interpreter.RegisterMessage("KICK", { "nickname" },
        [this](const std::vector<std::string>& args)
        {
            auto nick = args[0];
            auto client = m_server->m_clients[nick];

            auto& channel = m_server->m_channels[m_client->channel];

            // Removes the user from the channel
            auto pos = std::find(channel->nicknames.begin(), channel->nicknames.end(), nick);
            channel->nicknames.erase(pos);

            // Sets the user's channel to an empty string
            m_server->m_clients[nick]->channel = "";

            return RPL_CODES.at("RPL_KICKED");
        },
        sameChannelAdminValidator
    );

    m_interpreter.RegisterMessage("INVITE", { "nickname", "channel" },
        [this](const std::vector<std::string>& args)
        {
            auto nick = args[0];
            auto channel = args[1];
            
            // Checks if the channel exists. If it doesn't, creates one.
            if (m_server->m_channels.find(channel) == m_server->m_channels.end())
            {
                auto newChannel = CreateRef<Channel>(Channel{ channel, false, { }, { } });
                newChannel->invites.insert(m_client->nickname);
                newChannel->nicknames.push_back(m_client->nickname);
                m_client->isOperator = true;
                m_client->channel = channel;
            }
            // If the channel is not new, checks if the client is authorized to invite to it.
            else if (!m_client->isOperator)
            {
                return ERR_CODES.at("ERR_CHANOPRIVSNEEDED");
            }

            // Ensures that the target client is not already in the channel
            if (m_server->m_clients[nick]->channel != channel)
            {
                m_server->m_channels[channel]->invites.insert(nick);
                m_server->SendToClient(nick, "You have been invited to " + channel + " by " + m_client->nickname + ".");
                return RPL_CODES.at("RPL_INVITING");
            }
            else
            {
                // Otherwise, returns an error.
                return ERR_CODES.at("ERR_USERONCHANNEL");
            }
        },
        Pipe(targetUserExistsValidator, channelNameIsValidArg2)
    );

    m_interpreter.RegisterMessage("MODE", { "channel", "modes" },
        [this](const std::vector<std::string>& args)
        {
            auto channel = args[0];

            // Checks if the operator is in the channel.
            if (m_client->channel != channel)
            {
                return ERR_CODES.at("ERR_USERNOTINCHANNEL");
            }
            
            auto modes = Utils::StringTrim(args[1]);
            
            // Checks for give/remove operator mode.
            if (modes == "+o")
            {
                if (args.size() < 3) return ERR_CODES.at("ERR_NEEDMOREPARAMS");
                auto targetUser = args[2];

                if (m_server->m_clients[targetUser]->channel != channel)
                {
                    return ERR_CODES.at("ERR_USERNOTINCHANNEL");
                }

                m_server->m_clients[targetUser]->isOperator = true;
                return RPL_CODES.at("RPL_MODEUPDATED");
            }
            else if (modes == "-o")
            {
                if (args.size() < 3) return ERR_CODES.at("ERR_NEEDMOREPARAMS");
                auto targetUser = args[2];

                if (m_server->m_clients[targetUser]->channel != channel)
                {
                    return ERR_CODES.at("ERR_USERNOTINCHANNEL");
                }

                m_server->m_clients[targetUser]->isOperator = false;
                return RPL_CODES.at("RPL_MODEUPDATED");
            }
            // Checks for enable/disable invite only modes
            else if (modes == "+i")
            {
                m_server->m_channels[channel]->isInviteOnly = true;
                return RPL_CODES.at("RPL_MODEUPDATED");
            }
            else if (modes == "-i")
            {
                m_server->m_channels[channel]->isInviteOnly = false;
                return RPL_CODES.at("RPL_MODEUPDATED");
            }

            return ERR_CODES.at("ERR_UNKNOWNMODE");
        },
        Pipe(sourceClientIsAdmin, channelArg1Exists)
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

            if (m_client -> channel != ""){
                if (!m_client -> isMuted){
                    m_server->BroadcastChannel(m_client->nickname + ": " + msg, m_client->channel);
                }else{
                    m_client -> sock.Send("You are not authorized to send messages in this channel.");
                }
            }else{
                m_client -> sock.Send("Please join a channel to send messages.");
            }
            m_server->m_clientsMutex.unlock();
        }
    );
}