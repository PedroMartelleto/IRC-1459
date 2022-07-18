#include "server.h"
#include "commons.h"
#include "serverInterpreter.h"
#include "connectedClient.h"
#include <random>
#include <time.h>
#include <signal.h>
#include <set>


// MARK: - ConnectedClient

bool ConnectedClient::operator==(const ConnectedClient& other) const
{
    return nickname == other.nickname;
}

ConnectedClient& ConnectedClient::operator=(const ConnectedClient& other)
{
    nickname = other.nickname;
    listener = other.listener;
    sock = other.sock;
    channel = other.channel;
    isMuted = other.isMuted;
    isOperator = other.isOperator;
    return *this;
}

ConnectedClient& ConnectedClient::operator=(ConnectedClient& other)
{
    nickname = other.nickname;
    listener = other.listener;
    sock = other.sock;
    channel = other.channel;
    isMuted = other.isMuted;
    isOperator = other.isOperator;
    return *this;
}
