#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <ios>

#include "libs/glm/glm.hpp"

class Helper
{
public:

	static size_t split(const std::string& txt, std::vector<std::string>& strs, char ch);

	static std::string trim(std::string& str);

	static void eraseSubStr(std::string& mainStr, const std::string& toErase);

	template <typename T>
	static std::string to_string_with_precision(const T a_value, const int n = 6)
	{
		std::ostringstream out;
		out.precision(n);
		out << std::fixed << a_value;
		return out.str();
	}

	static std::string glmVec4_to_string(glm::vec4 vector);

	static glm::vec4 string_to_glmVec4(std::string string);

	static std::string glmVec3_to_string(glm::vec3 vector);

	static glm::vec3 string_to_glmVec3(std::string string);

	static std::string glmVec2_to_string(glm::vec2 vector);

	static glm::vec2 string_to_glmVec2(std::string string);
};

