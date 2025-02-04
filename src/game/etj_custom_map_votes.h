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

#ifndef CUSTOM_MAP_VOTES_HH
#define CUSTOM_MAP_VOTES_HH

#include <vector>
#include <string>

class MapStatistics;

class CustomMapVotes
{
public:
	struct MapType
	{
		std::string type;
		std::string callvoteText;
		std::vector<std::string> mapsOnServer;
		std::vector<std::string> otherMaps;
	};

	struct TypeInfo
	{
		TypeInfo(bool typeExists, const std::string& callvoteText) :
			typeExists(typeExists), callvoteText(callvoteText)
		{

		}
		TypeInfo() : typeExists(false)
		{
		}
		bool typeExists;
		std::string callvoteText;
	};

	CustomMapVotes(MapStatistics *mapStats);
	~CustomMapVotes();
	bool Load();
	TypeInfo GetTypeInfo(const std::string& type) const;
	const std::string RandomMap(const std::string& type);
	bool isValidMap(const std::string &mapName);
	std::string ListTypes() const;
	const std::vector<std::string> *ListInfo(const std::string& name);
	void GenerateVotesFile();
private:
	std::vector<MapType>           customMapVotes_;
	const std::vector<std::string> *_currentMapsOnServer;
	MapStatistics                  *_mapStats;
};

#endif
