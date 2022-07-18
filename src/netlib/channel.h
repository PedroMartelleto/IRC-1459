#pragma once

#include <string>
#include <vector>
#include <set>
#include "server.h"
#include "connectedClient.h"

class Channel {
public:

    const std::string name;
    
    bool isInviteOnly;

    std::vector<ConnectedClient> users;

    std::set<std::string> invites;
};





