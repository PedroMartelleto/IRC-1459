#include "utils.h"
#include <fstream>
#include <sstream>

namespace Utils
{
	std::string GetDirectoryPath(const std::string& filePath)
	{
		size_t pos = filePath.find_last_of("\\/");
		return (std::string::npos == pos) ? "" : filePath.substr(0, pos);
	}

	std::string LoadFile(const std::string& filePath)
	{
		std::ifstream dataFile(filePath);
		std::stringstream buffer;
		buffer << dataFile.rdbuf();
		dataFile.close();
		return buffer.str();
	}

	static bool firstRand = true;

	int RandomInt(int min, int max)
	{
		if (firstRand)
		{
			srand((unsigned int) time(NULL));
			firstRand = false;
		}
		return min + rand() % ((max + 1) - min);
	}

	float FastRandomFloat(float min, float max)
	{
		if (firstRand)
		{
			srand((unsigned int)time(NULL));
			firstRand = false;
		}
		float r = static_cast <float>(rand()) / static_cast <float>(RAND_MAX);
		return min + (max - min) * r;
	}

	std::string StringJoin(const std::vector<std::string>& array, const std::string& separator)
	{
		std::stringstream stream;
		std::copy(array.begin(), array.end(), std::ostream_iterator<std::string>(stream, separator.c_str()));
		return stream.str();
	}

	std::string StringUpperCase(const std::string& str)
	{
		std::string result = str;
		std::transform(result.begin(), result.end(), result.begin(), ::toupper);
		return result;
	}

	std::vector<std::string> StringSplit(const std::string& str, const std::string& delimiter)
	{
		size_t pos_start = 0, pos_end, delim_len = delimiter.length();
		std::string token;
		std::vector<std::string> res;

		while ((pos_end = str.find(delimiter, pos_start)) != std::string::npos) {
			token = str.substr(pos_start, pos_end - pos_start);
			pos_start = pos_end + delim_len;
			res.push_back(token);
		}

		res.push_back(str.substr(pos_start));
		return res;
	}
	
	std::string StringLeftTrim(const std::string& str)
	{
		std::string s = std::string(str);
		s.erase(s.begin(), std::find_if(s.begin(), s.end(),
				std::not1(std::ptr_fun<int, int>(std::isspace))));
		return s;
	}

	std::string StringRightTrim(const std::string& str)
	{
		std::string s = std::string(str);
		s.erase(std::find_if(s.rbegin(), s.rend(),
				std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
		return s;
	}

	std::string StringTrim(const std::string &str)
	{
		return StringLeftTrim(StringRightTrim(str));
	}
};