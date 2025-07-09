#include "factBase.h"
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <tuple>
#include <regex>

using namespace std;

static unordered_map<string, string> verseMap;

unordered_map<string, int> bookOrder = {
	{"genesis", 1}, {"exodus", 2}, {"leviticus", 3}, {"numbers", 4}, {"deuteronomy", 5},
	{"joshua", 6}, {"judges", 7}, {"ruth", 8}, {"1 samuel", 9}, {"2 samuel", 10},
	{"1 kings", 11}, {"2 kings", 12}, {"1 chronicles", 13}, {"2 chronicles", 14},
	{"ezra", 15}, {"nehemiah", 16}, {"esther", 17}, {"job", 18}, {"psalms", 19},
	{"proverbs", 20}, {"ecclesiastes", 21}, {"song of solomon", 22}, {"isaiah", 23},
	{"jeremiah", 24}, {"lamentations", 25}, {"ezekiel", 26}, {"daniel", 27},
	{"hosea", 28}, {"joel", 29}, {"amos", 30}, {"obadiah", 31}, {"jonah", 32},
	{"micah", 33}, {"nahum", 34}, {"habakkuk", 35}, {"zephaniah", 36}, {"haggai", 37},
	{"zechariah", 38}, {"malachi", 39}, {"matthew", 40}, {"mark", 41}, {"luke", 42},
	{"john", 43}, {"acts", 44}, {"romans", 45}, {"1 corinthians", 46}, {"2 corinthians", 47},
	{"galatians", 48}, {"ephesians", 49}, {"philippians", 50}, {"colossians", 51},
	{"1 thessalonians", 52}, {"2 thessalonians", 53}, {"1 timothy", 54}, {"2 timothy", 55},
	{"titus", 56}, {"philemon", 57}, {"hebrews", 58}, {"james", 59}, {"1 peter", 60},
	{"2 peter", 61}, {"1 john", 62}, {"2 john", 63}, {"3 john", 64}, {"jude", 65},
	{"revelation", 66}
};

string toLower(const string& s) {
	string result = s;
	transform(result.begin(), result.end(), result.begin(), ::tolower);
	return result;
}

bool containsWholeWord(const string& text, const string& word) {
	istringstream iss(text);
	string token;
	string search = toLower(word);
	while (iss >> token) {
		token.erase(remove_if(token.begin(), token.end(), ::ispunct), token.end());
		if (toLower(token) == search) return true;
	}
	return false;
}

void FactBase::loadBible(const string& filename) {
	ifstream file(filename);
	string line;
	while (getline(file, line)) {
		istringstream iss(line);
		string word;
		vector<string> tokens;
		while (iss >> word)
			tokens.push_back(word);

		if (tokens.size() < 3) continue;

		int chapterVerseIndex = -1;
		for (int i = 0; i < tokens.size(); ++i) {
			if (tokens[i].find(':') != string::npos) {
				chapterVerseIndex = i;
				break;
			}
		}
		if (chapterVerseIndex <= 0 || chapterVerseIndex >= tokens.size() - 1) continue;

		string book;
		for (int i = 0; i < chapterVerseIndex; ++i) {
			if (i > 0) book += " ";
			book += tokens[i];
		}
		string chapterVerse = tokens[chapterVerseIndex];
		string reference = book + " " + chapterVerse;

		string verse;
		for (int i = chapterVerseIndex + 1; i < tokens.size(); ++i) {
			if (i > chapterVerseIndex + 1) verse += " ";
			verse += tokens[i];
		}

		verseMap[reference] = verse;
	}
	cout << "Bible loaded with " << verseMap.size() << " verses." << endl;
}

vector<string> FactBase::query(const string& keyword) {
	vector<pair<string, string>> matchedVerses;
	string search = toLower(keyword);

	for (const auto& [ref, verse] : verseMap) {
		if (toLower(ref) == search) {
			return {ref + " — " + verse};
		}
	}

	istringstream iss(keyword);
	vector<string> tokens;
	string word;
	while (iss >> word) tokens.push_back(word);
	if (tokens.empty()) return {};

	string book;
	int i = 0;
	while (i < tokens.size() && tokens[i].find_first_of("0123456789") == string::npos) {
		if (!book.empty()) book += " ";
		book += tokens[i++];
	}
	book = toLower(book);

	if (i == tokens.size()) {
		for (const auto& [ref, verse] : verseMap) {
			if (toLower(ref).rfind(book + " ", 0) == 0)
				matchedVerses.emplace_back(ref, verse);
		}
	}
	else {
		string rest;
		for (; i < tokens.size(); ++i) {
			if (!rest.empty()) rest += " ";
			rest += tokens[i];
		}
		rest = toLower(rest);

		smatch match;
		if (regex_match(rest, match, regex(R"((\d+)[–-](\d+))"))) {
			int start = stoi(match[1]);
			int end = stoi(match[2]);
			for (const auto& [ref, verse] : verseMap) {
				string lowerRef = toLower(ref);
				for (int c = start; c <= end; ++c) {
					string prefix = book + " " + to_string(c) + ":";
					if (lowerRef.rfind(prefix, 0) == 0)
						matchedVerses.emplace_back(ref, verse);
				}
			}
		}
		else if (regex_match(rest, match, regex(R"((\d+))"))) {
			string prefix = book + " " + match[1].str() + ":";
			for (const auto& [ref, verse] : verseMap) {
				if (toLower(ref).rfind(prefix, 0) == 0)
					matchedVerses.emplace_back(ref, verse);
			}
		}
	}

	if (matchedVerses.empty()) {
		for (const auto& [ref, verse] : verseMap) {
			if (containsWholeWord(ref, keyword) || containsWholeWord(verse, keyword)) {
				matchedVerses.emplace_back(ref, verse);
			}
		}
	}

	auto parseRef = [](const string& ref) {
		istringstream iss(ref);
		string book, chapterVerse;
		string word;
		while (iss >> word) {
			if (word.find(':') != string::npos) {
				chapterVerse = word;
				break;
			}
			if (!book.empty()) book += " ";
			book += word;
		}
		int chapter = 0, verse = 0;
		auto pos = chapterVerse.find(':');
		if (pos != string::npos) {
			chapter = stoi(chapterVerse.substr(0, pos));
			verse = stoi(chapterVerse.substr(pos + 1));
		}
		return make_tuple(bookOrder[toLower(book)], chapter, verse);
	};

	sort(matchedVerses.begin(), matchedVerses.end(), [&](auto& a, auto& b) {
		return parseRef(a.first) < parseRef(b.first);
	});

	vector<string> result;
	for (const auto& [ref, verse] : matchedVerses)
		result.push_back(ref + " — " + verse);

	return result;
}
