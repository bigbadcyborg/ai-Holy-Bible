// factBase is a Global Object in a Shared Library and is saved as
//  persistent and serialized data if handled properly
//    Note: uses Automatic Linking with g++
#ifndef FACTBASE_H
#define FACTBASE_H

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cctype>
#include <regex>

using namespace std;

struct Fact
{
	string str;
	Fact(string s) { str = s; }
};

// a rule is a fact so ruleBase inherits factBase
class FactBase
{
private:
	// Private Static Attributes
	FactBase() = delete; // Prevent instantiation( this is a Global Static Class )

public:
	// Public Static Functions:
	static string toString() { return "factBase calls toString() ..."; }
	static void loadBible(const string &filename);
	static vector<string> query(const string &keyword);

	static void saveFactBase(); // save FactBase object as a JSON binary file
	static void loadFactBase(); // load a copy of the FactBase object from binary file
};

#endif