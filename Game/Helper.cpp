#include "Helper.h"

size_t Helper::split(const std::string& txt, std::vector<std::string>& strs, char ch)
{
	size_t pos = txt.find(ch);
	size_t initialPos = 0;
	strs.clear();

	// Decompose statement
	while (pos != std::string::npos) {
		strs.push_back(txt.substr(initialPos, pos - initialPos));
		initialPos = pos + 1;

		pos = txt.find(ch, initialPos);
	}

	// Add the last one
	strs.push_back(txt.substr(initialPos, (std::min)(pos, txt.size()) - initialPos + 1));

	return strs.size();
}


std::string Helper::trim(std::string& str)
{
	int first = -1;
	int last = -1;
	
	first = str.find_first_not_of(' ');
	last = str.find_last_not_of(' ');

	if (first >= 0 && last >= 0)
		return str.substr(first, (last - first + 1));
	else
		return "";
}

void Helper::eraseSubStr(std::string& mainStr, const std::string& toErase)
{
	// Search for the substring in string
	size_t pos = mainStr.find(toErase);
	if (pos != std::string::npos)
	{
		// If found then erase it from string
		mainStr.erase(pos, toErase.length());
	}
}

std::string Helper::glmVec4_to_string(glm::vec4 vector)
{
	std::string data = "";
	data += std::to_string(vector.x) + ";";
	data += std::to_string(vector.y) + ";";
	data += std::to_string(vector.z) + ";";
	data += std::to_string(vector.w);

	return data;
}

glm::vec4 Helper::string_to_glmVec4(std::string string)
{
	std::vector<std::string> params;
	Helper::split(string, params, ';');
	return glm::vec4(std::stof(params[0]), std::stof(params[1]), std::stof(params[2]), std::stof(params[3]));
}

std::string Helper::glmVec3_to_string(glm::vec3 vector)
{
	std::string data = "";
	data += std::to_string(vector.x) + ";";
	data += std::to_string(vector.y) + ";";
	data += std::to_string(vector.z);

	return data;
}

glm::vec3 Helper::string_to_glmVec3(std::string string)
{
	std::vector<std::string> params;
	Helper::split(string, params, ';');
	return glm::vec3(std::stof(params[0]), std::stof(params[1]), std::stof(params[2]));
}

std::string Helper::glmVec2_to_string(glm::vec2 vector)
{
	std::string data = "";
	data += std::to_string(vector.x) + ";";
	data += std::to_string(vector.y);

	return data;
}

glm::vec2 Helper::string_to_glmVec2(std::string string)
{
	std::vector<std::string> params;
	Helper::split(string, params, ';');
	return glm::vec2(std::stof(params[0]), std::stof(params[1]));
}