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
    for (long unsigned int i = 0; i < users.size(); i++)
    {
        if (users[i]->nickname == nickname)
        {
            users.erase(users.begin() + i);
            return;
        }
    }
}