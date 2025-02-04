/*
 * MIT License
 * 
 * Copyright (c) 2022 ETJump team <zero@etjump.com>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <algorithm>
#include <cctype>

#include "etj_string_utilities.h"

extern "C" {
#include "../sha-1/sha1.h"
}


std::string ETJump::hash(const std::string& input)
{
	SHA1Context sha;

	SHA1Reset(&sha);
	SHA1Input(&sha, reinterpret_cast<const unsigned char *>(input.c_str()), input.length());

	if (!SHA1Result(&sha))
	{
		return "";
	}

	char buffer[60] = "";
	snprintf(buffer, sizeof(buffer), "%08X%08X%08X%08X%08X", sha.Message_Digest[0],
		sha.Message_Digest[1],
		sha.Message_Digest[2],
		sha.Message_Digest[3],
		sha.Message_Digest[4]);
	return buffer;
}

// https://en.wikipedia.org/wiki/Levenshtein_distance
// https://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/Levenshtein_distance#C.2B.2B
unsigned int levenshteinDistance(const std::string& s1, const std::string& s2)
{
	const std::size_t len1 = s1.size(), len2 = s2.size();
	std::vector<std::vector<unsigned int>> d(len1 + 1, std::vector<unsigned int>(len2 + 1));

	d[0][0] = 0;
	for (unsigned int i = 1; i <= len1; ++i) d[i][0] = i;
	for (unsigned int i = 1; i <= len2; ++i) d[0][i] = i;

	for (unsigned int i = 1; i <= len1; ++i)
		for (unsigned int j = 1; j <= len2; ++j)
			d[i][j] = std::min({ d[i - 1][j] + 1, d[i][j - 1] + 1, d[i - 1][j - 1] + (s1[i - 1] == s2[j - 1] ? 0 : 1) });
	return d[len1][len2];
}

std::string ETJump::getBestMatch(const std::vector<std::string>& words, const std::string& current)
{
	std::vector<std::pair<std::string, int>> distances;

	for (const auto & word : words)
	{
		distances.push_back(std::make_pair(word, levenshteinDistance(word, current)));
	}

	auto smallest = std::min_element(begin(distances), end(distances), [](const std::pair<std::string, int>& lhs, const std::pair<std::string, int>& rhs)
	{
		return lhs.second < rhs.second;
	});

	return smallest->first;
}

static void SanitizeConstString(const char *in, char *out, bool toLower)
{
	while (*in)
	{
		if (*in == 27 || *in == '^')
		{
			in++;       // skip color code
			if (*in)
			{
				in++;
			}
			continue;
		}

		if (*in < 32)
		{
			in++;
			continue;
		}

		*out++ = (toLower) ? tolower(*in++) : *in++;
	}

	*out = 0;
}

std::string ETJump::sanitize(const std::string& text, bool toLower)
{
	auto len = text.length();
	std::vector<char> out(len + 1);
	SanitizeConstString(text.c_str(), out.data(), toLower ? true : false);
	return std::string(out.data());
}

std::string ETJump::getValue(const char* value, const std::string& defaultValue)
{
	return strlen(value) > 0 ? value : defaultValue;
}

std::string ETJump::getValue(const std::string& value, const std::string& defaultValue)
{
	return value.length() > 0
		? value
		: defaultValue;
}

std::string ETJump::trimStart(const std::string& input)
{
	std::string str = input;
	str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](int ch) {
		return !std::isspace(ch);
	}));
	return str;
}

std::string ETJump::trimEnd(const std::string& input)
{
	std::string str = input;
	str.erase(std::find_if(str.rbegin(), str.rend(), [](int ch) {
		return !std::isspace(ch);
	}).base(), str.end());
	return str;
}

std::string ETJump::trim(const std::string& input)
{
	return trimEnd(trimStart(input));
}

// word-wrapper
std::vector<std::string> ETJump::splitString(std::string &input, char separator, size_t maxLength)
{
	std::vector<std::string> output;
	size_t lastPos = 0;

	if (input.size() <= maxLength)
	{
		output.push_back(input);
		return output;
	}

	while (true) 
	{
		auto pos = input.rfind(separator, lastPos + maxLength);
		/* separator not found */
		if (pos == std::string::npos)
		{
			/* split by length; */
			size_t numSplits = input.size() / maxLength;
			for (size_t i = 1; i <= numSplits; ++i)
			{
				output.push_back(input.substr(lastPos, maxLength));
				lastPos = (maxLength * i);
			}
			break;
		}
		pos += 1;
		/* no new separators were found */
		if (pos == lastPos)
		{
			break;
		}
		output.push_back(input.substr(lastPos, pos - lastPos));
		lastPos = pos;
	}
	/* add last bit if any */
	if (lastPos < input.size())
	{
		output.push_back(input.substr(lastPos));
	}
	return output;
}

std::string ETJump::StringUtil::toLowerCase(const std::string& input)
{
	std::string str = input;
	std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::tolower(c); });
	return str;
}

std::string ETJump::StringUtil::toUpperCase(const std::string& input)
{
	std::string str = input;
	std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::toupper(c); });
	return str;
}

std::string ETJump::StringUtil::eraseLast(const std::string& input, const std::string& substring)
{
	std::string str = input;
	auto pos = str.rfind(substring);
	if (pos != std::string::npos)
	{
		str.erase(pos, substring.size());
	}
	return str;
}

std::vector<std::string> ETJump::StringUtil::split(const std::string& input, const std::string& delimiter)
{
	size_t posStart = 0, posEnd, delimLen = delimiter.length();
	std::string token;
	std::vector<std::string> splits;

	while ((posEnd = input.find(delimiter, posStart)) != std::string::npos)
	{
		token = input.substr(posStart, posEnd - posStart);
		posStart = posEnd + delimLen;
		splits.push_back(token);
	}

	splits.push_back(input.substr(posStart));

	return splits;
}

void ETJump::StringUtil::replaceAll(std::string& input, const std::string& from, const std::string& to)
{
	size_t startPost = 0;
	while ((startPost = input.find(from, startPost)) != std::string::npos)
	{
		input.replace(startPost, from.length(), to);
		startPost += to.length();
	}
}
