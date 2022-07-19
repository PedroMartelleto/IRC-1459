#include "channel.h"

bool Channel::IsAuthorized(const std::string& nickname)
{
    if (isInviteOnly)
    {
        return invites.find(nickname) != invites.end();
    }
    else
    {
        return true;
    }
}

void Channel::RemoveUser(const std::string& nickname){
    for (size_t i = 0; i < nicknames.size(); i++)
    {
        if (nicknames[i] == nickname)
        {
            nicknames.erase(nicknames.begin() + i);
            return;
        }
    }
}