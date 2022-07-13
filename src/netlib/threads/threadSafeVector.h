#pragma once

#include <mutex>
#include <vector>

/**
 * @brief A thread-safe vector.
 */
template <typename T>
class ThreadSafeVector
{
public:
    void Push(const T& element)
    {
        m_mutex.lock();
        m_vector.push_back(element);
        m_mutex.unlock();
    }

    void Iterate(const std::function<void(const T&)>& func)
    {
        m_mutex.lock();
        for (const T& element : m_vector)
        {
            func(element);
        }
        m_mutex.unlock();
    }

    void Clear()
    {
        m_mutex.lock();
        m_vector.clear();
        m_mutex.unlock();
    }

    size_t Size()
    {
        m_mutex.lock();
        size_t size = m_vector.size();
        m_mutex.unlock();
        return size;
    }

    bool IsEmpty()
    {
        m_mutex.lock();
        bool empty = m_vector.empty();
        m_mutex.unlock();
        return empty;
    }

    int IndexOf(const T& element)
    {
        m_mutex.lock();
        auto it = std::find(m_vector.begin(), m_vector.end(), element);

        if (it == m_vector.end())
        {
            m_mutex.unlock();
            return -1;
        }
        else
        {
            int dist = std::distance(m_vector.begin(), it);
            m_mutex.unlock();
            return dist;
        }
    }

    void RemoveAt(int index)
    {
        m_mutex.lock();
        m_vector.erase(m_vector.begin() + index);
        m_mutex.unlock();
    }

    T operator[](int index)
    {
        m_mutex.lock();
        T element = m_vector[index];
        m_mutex.unlock();
        return element;
    }

    void Lock()
    {
        m_mutex.lock();
    }

    void Unlock()
    {
        m_mutex.unlock();
    }
private:
    std::mutex m_mutex;
    std::vector<T> m_vector;
};