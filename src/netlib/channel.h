#include <string>
#include <vector>
#include <set>

struct User {
    std::string nickname;
    bool isMuted;
    bool isAdmin;
};

class Channel {
public:

    const std::string name;
    
    bool isInviteOnly;

    std::vector<User> users;

    std::set<std::string> invites;
};





