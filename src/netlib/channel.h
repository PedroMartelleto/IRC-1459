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

    std::vector<std::string> nicknames;
    std::set<std::string> invites;
};





