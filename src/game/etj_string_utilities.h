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

#pragma once

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

#include <string>
#include <sstream>
#include <vector>
#include <fmt/printf.h>

namespace ETJump
{
	std::string hash(const std::string& input);
	std::string getBestMatch(const std::vector<std::string>& words, const std::string& current);
	std::string sanitize(const std::string& text, bool toLower = false);
	// returns the value if it's specified, else the default value
	std::string getValue(const char *value, const std::string& defaultValue = "");
	std::string getValue(const std::string& value, const std::string& defaultValue = "");

	template<typename... Targs>
	std::string stringFormat(const std::string& format, const Targs&... Fargs) {
		return fmt::sprintf(format, Fargs...);
	}

    std::string trimStart(const std::string& input);
    std::string trimEnd(const std::string& input);
    std::string trim(const std::string& input);

    std::vector<std::string> splitString(std::string &input, char separator, size_t maxLength);

	namespace StringUtil 
	{
		std::string toLowerCase(const std::string& input);
		std::string toUpperCase(const std::string& input);
		std::string eraseLast(const std::string& input, const std::string& substring);
		template <typename T>
		std::string join(const T &v, const std::string &delim) {
			std::ostringstream s;
			for (const auto& i : v) {
				if (&i != &v[0]) {
					s << delim;
				}
				s << i;
			}
			return s.str();
		}
		std::vector<std::string> split(const std::string& input, const std::string& delimiter);
		void replaceAll(std::string& input, const std::string& from, const std::string& to);
	}
}
