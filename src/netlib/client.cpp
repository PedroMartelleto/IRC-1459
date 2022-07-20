#include "client.h"

void Client::Reset()
{
    m_replyCallbacksMutex.unlock();
    m_replyCallbacks.clear();
    m_hasNickname = false;
}

void Client::InterpretMessage(const std::string& msg)
{
    const auto trimmedMsg = Utils::StringTrim(msg);

    // Checks for server respones with 3 digits.
    if (trimmedMsg.substr(0, 4) == "RPL ")
    {
        try
        {
            // Interprets the response and prints it.
            std::string codeStr = Utils::StringTrim(trimmedMsg.substr(4, 3));
            int code = std::stoi(codeStr);

            if (code < 1 || code > 1000)
            {
                Logger::Print("Server responded with invalid reply code: %s\n", codeStr.c_str());
                return;
            }
            
            if (code >= 1 && code < 400)
            {
                if (RPL_NAMES.find(codeStr) != RPL_NAMES.end())
                {
                    Logger::Print("[%s] %s\n", codeStr.c_str(), RPL_NAMES[codeStr].c_str());
                }
                else
                {
                    Logger::Print("Server responded with [%d].\n", code);
                }
            }
            else
            {
                if (ERR_NAMES.find(codeStr) != ERR_NAMES.end())
                {
                    Logger::Print("[%s] %s\n", codeStr.c_str(), ERR_NAMES[codeStr].c_str());
                }
                else
                {
                    Logger::Print("Server responded with error [%d].\n", code);
                }
            }

            if (m_replyCallbacks.size() > 0)
            {
                m_replyCallbacksMutex.lock();
                auto callback = m_replyCallbacks[m_replyCallbacks.size() - 1];
                m_replyCallbacks.pop_back();
                m_replyCallbacksMutex.unlock();
                callback(code);
            }

            return;
        }
        catch (const std::exception& e) {}
    }

    // Otherwise, just echoes the message.
    Logger::Print("%s\n", msg.c_str());
}

void Client::RegisterReplyCallback(const std::function<void(int)>& callback)
{
    m_replyCallbacksMutex.lock();
    m_replyCallbacks.push_back(callback);
    m_replyCallbacksMutex.unlock();
}

bool Client::HasNickname() const
{
    return m_hasNickname;
}

void Client::SetHasNickname(bool hasNickname)
{
    m_hasNickname = hasNickname;
}