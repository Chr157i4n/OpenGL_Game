#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <ios>

class Helper
{
public:

	static size_t split(const std::string& txt, std::vector<std::string>& strs, char ch);

	static void eraseSubStr(std::string& mainStr, const std::string& toErase);

	template <typename T>
	static std::string to_string_with_precision(const T a_value, const int n = 6)
	{
		std::ostringstream out;
		out.precision(n);
		out << std::fixed << a_value;
		return out.str();
	}
};

