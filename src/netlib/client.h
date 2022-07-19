#pragma once

#include "./commons.h"
#include <random>

/**
 * @brief A client is anything connecting to a server that is not another
   server.  Each client is distinguished from other clients by a unique
   nickname having a maximum length of nine (50) characters.  See the
   protocol grammar rules for what may and may not be used in a
   nickname.  In addition to the nickname, all servers must have the
   following information about all clients: the real name of the host
   that the client is running on, the username of the client on that
   host, and the server to which the client is connected.
 */
class Client {
public:
	Client() {}

	void RegisterReplyCallback(const std::function<void(int)>& callback);
	void InterpretMessage(const std::string& message);

	bool HasNickname() const;
	void SetHasNickname(bool hasNickname);

	void Reset();
private:
	std::mutex m_replyCallbacksMutex;
	std::vector<std::function<void(int)>> m_replyCallbacks;
	bool m_hasNickname = false;
};