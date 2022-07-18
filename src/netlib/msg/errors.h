#pragma once

#include "../utils.h"

static const std::map<std::string, std::string> ERR_CODES =
{
    {"ERR_NOSUCHNICK", "401"},
    {"ERR_NOSUCHSERVER", "402"},
    {"ERR_NOSUCHCHANNEL", "403"},
    {"ERR_CANNOTSENDTOCHAN", "404"},
    {"ERR_NORECIPIENT", "411"},
    {"ERR_NOTEXTTOSEND", "412"},
    {"ERR_UNKNOWNCOMMAND", "421"},
    {"ERR_NOMOTD", "422"},
    {"ERR_NONICKNAMEGIVEN", "431"},
    {"ERR_ERRONEUSNICKNAME", "432"},
    {"ERR_NICKNAMEINUSE", "433"},
    {"ERR_NICKCOLLISION", "436"},
    {"ERR_USERNOTINCHANNEL", "441"},
    {"ERR_NOTONCHANNEL", "442"},
    {"ERR_NOTREGISTERED", "451"},
    {"ERR_NEEDMOREPARAMS", "461"},
    {"ERR_ALREADYREGISTRED", "462"},
    {"ERR_PASSWDMISMATCH", "464"},
    {"ERR_UNKNOWNMODE", "472"},
    {"ERR_NOPRIVILEGES", "481"},
    {"ERR_CHANOPRIVSNEEDED", "482"},
    {"ERR_NOOPERPRIVILEGES", "491"},
    {"ERR_UMODEUNKNOWNFLAG", "501"},
    {"ERR_USERSDONTMATCH", "502"}
};
    
static auto ERR_NAMES = Utils::ReverseMap(ERR_CODES);