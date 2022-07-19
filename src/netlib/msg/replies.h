#pragma once

#include "../utils.h"

static const std::map<std::string, std::string> RPL_CODES =
{
    // Not really an IRC reply, but useful for returning no errors in the validation functions.
    {"PIPE_VALID", "000"},

    {"RPL_WELCOME", "001"},
    {"RPL_YOURHOST", "002"},
    {"RPL_CREATED", "003"},
    {"RPL_MYINFO", "004"},

    {"RPL_MUTED", "010"},
    {"RPL_UNMUTED", "011"},
    {"RPL_KICKED", "012"},
    {"RPL_INVITED", "013"},

    {"RPL_TRACEUSER", "205"},
    {"RPL_TRACESERVER", "206"},
    {"RPL_ENDOFTRACE", "262"},

    {"RPL_STATSCOMMANDS", "212"},
    {"RPL_ENDOFSTATS", "219"},
    {"RPL_STATSOLINE", "243"},

    {"RPL_LUSERCLIENT", "251"},
    {"RPL_LUSEROP", "252"},
    {"RPL_LUSERUNKNOWN", "253"},
    {"RPL_LUSERCHANNELS", "254"},
    {"RPL_LUSERME", "255"},

    {"RPL_ADMINME", "256"},
    {"RPL_ADMINLOC1", "257"},
    {"RPL_ADMINLOC2", "258"},
    {"RPL_ADMINEMAIL", "259"},

    {"RPL_AWAY", "301"},
    {"RPL_UNAWAY", "305"},
    {"RPL_NOWAWAY", "306"},

    {"RPL_WHOISUSER", "311"},
    {"RPL_WHOISSERVER", "312"},
    {"RPL_WHOISOPERATOR", "313"},
    {"RPL_WHOISIDLE", "317"},
    {"RPL_ENDOFWHOIS", "318"},
    {"RPL_WHOISCHANNELS", "319"},

    {"RPL_WHOREPLY", "352"},
    {"RPL_ENDOFWHO", "315"},

    {"RPL_LISTSTART", "321"},
    {"RPL_LIST", "322"},
    {"RPL_LISTEND", "323"},

    {"RPL_CHANNELMODEIS", "324"},

    {"RPL_NOTOPIC", "331"},
    {"RPL_TOPIC", "332"},

    {"RPL_VERSION", "351"},
    {"RPL_NAMREPLY", "353"},
    {"RPL_LINKS", "364"},
    {"RPL_ENDOFLINKS", "365"},
    {"RPL_ENDOFNAMES", "366"},

    {"RPL_INFO", "371"},
    {"RPL_ENDOFINFO", "374"},
    {"RPL_MOTDSTART", "375"},
    {"RPL_MOTD", "372"},
    {"RPL_ENDOFMOTD", "376"},

    {"RPL_YOUREOPER", "381"},

    {"RPL_TIME", "391"}
};

static auto RPL_NAMES = Utils::ReverseMap(RPL_CODES);