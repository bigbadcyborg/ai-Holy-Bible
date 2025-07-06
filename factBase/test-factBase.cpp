#include <iostream>
#include "factBase.h"

using namespace std;

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		cout << "Usage: " << argv[0] << " \"search term\"" << endl;
		return 1;
	}

	string queryStr = argv[1];
	FactBase::loadBible("kjv.txt");

	vector<string> results = FactBase::query(queryStr);

	if (results.empty())
	{
		cout << "No verses found containing: \"" << queryStr << "\"" << endl;
	}
	else
	{
		for (const string &verse : results)
			cout << verse << endl;
	}

	return 0;
}