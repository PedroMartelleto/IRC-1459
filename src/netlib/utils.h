#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <iterator>
#include <algorithm>
#include <map>

namespace Utils
{
	template <typename U, typename V>
	std::map<U, V> ReverseMap(const std::map<V, U>& map)
	{
		std::map<U, V> reversedMap;
		for (auto& pair : map)
		{
			reversedMap[pair.second] = pair.first;
		}
		return reversedMap;
	}

	std::string GetDirectoryPath(const std::string& filePath);

	std::string LoadFile(const std::string& filePath);

	int RandomInt(int min, int max);

	float FastRandomFloat(float min, float max);


	// MARK: - String utils

	std::string StringJoin(const std::vector<std::string>& array, const std::string& separator);

	std::string StringUpperCase(const std::string& str);

	std::vector<std::string> StringSplit(const std::string& str, const std::string& delimiter);

	std::string StringLeftTrim(const std::string& str);
	
	std::string StringRightTrim(const std::string& str);

	std::string StringTrim(const std::string &str);
};