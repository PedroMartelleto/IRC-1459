#pragma once

#include <string>
#include <vector>
#include <set>
#include "connectedClient.h"

class Channel {
public:

    const std::string name;
    
    bool isInviteOnly;

    std::vector<Ref<ConnectedClient> > users;

    std::set<std::string> invites;

    bool IsAuthorized(const std::string& nickname);

    void RemoveUser(const std::string& nickname);
};





