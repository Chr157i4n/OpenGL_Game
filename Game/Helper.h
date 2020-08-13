#pragma once

#include <vector>
#include <string>
#include <algorithm>

class Helper
{
public:

	static size_t split(const std::string& txt, std::vector<std::string>& strs, char ch);

	static void eraseSubStr(std::string& mainStr, const std::string& toErase);
};

