#pragma once

#include <string>
#include <vector>
#include <map>
#include <array>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <iomanip>
#include <sstream>
#include <thread>
#include <mutex>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <functional>
#include <cstring>
#include <memory>
#include <arpa/inet.h>

#include "msg/replies.h"
#include "msg/errors.h"
#include "utils.h"
#include "threads/logger.h"

template<typename T>
using Ref = std::shared_ptr<T>;

template<typename T, typename ... Args>
inline constexpr Ref<T> CreateRef(Args&& ... args)
{
	return std::make_shared<T>(std::forward<Args>(args)...);
}

template<typename T>
using Array = std::vector<T>;

template<typename K, typename V>
using RBTree = std::unordered_map<K, V>;

template<typename K, typename V>
using HashMap = std::map<K, V>;
