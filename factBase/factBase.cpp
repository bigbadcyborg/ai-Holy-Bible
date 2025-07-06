#include "factBase.h"

static unordered_map<string, string> verseMap;

void FactBase::loadBible(const string &filename)
{
    ifstream file(filename);
    string line;
    while (getline(file, line))
    {
        istringstream iss(line);
        string word;
        vector<string> tokens;

        while (iss >> word)
        {
            tokens.push_back(word);
        }

        if (tokens.size() < 3)
            continue; // must be: BookName ... Chapter:Verse Text...

        // Find the token with ':' which is the chapter:verse
        int chapterVerseIndex = -1;
        for (int i = 0; i < tokens.size(); ++i)
        {
            if (tokens[i].find(':') != string::npos)
            {
                chapterVerseIndex = i;
                break;
            }
        }
        if (chapterVerseIndex <= 0 || chapterVerseIndex >= tokens.size() - 1)
            continue;

        // Build book name
        string book;
        for (int i = 0; i < chapterVerseIndex; ++i)
        {
            if (i > 0)
                book += " ";
            book += tokens[i];
        }
        string chapterVerse = tokens[chapterVerseIndex];
        string reference = book + " " + chapterVerse;

        // Build verse text
        string verse;
        for (int i = chapterVerseIndex + 1; i < tokens.size(); ++i)
        {
            if (i > chapterVerseIndex + 1)
                verse += " ";
            verse += tokens[i];
        }

        verseMap[reference] = verse;
    }

    cout << "Bible loaded with " << verseMap.size() << " verses." << endl;
}


string toLower(const string &s)
{
    string result = s;
    for (char &c : result)
        c = tolower(c);
    return result;
}


vector<string> FactBase::query(const string &keyword)
{
    vector<string> results;
    string search = toLower(keyword);

    // Try exact match (case-insensitive)
    for (const auto &[ref, verse] : verseMap)
    {
        if (toLower(ref) == search)
        {
            results.push_back(ref + " — " + verse);
            return results;
        }
    }

    // Build regex pattern for whole word match (case-insensitive)
    regex wordPattern("\\b" + search + "\\b", regex_constants::icase);

    for (const auto &[ref, verse] : verseMap)
    {
        if (regex_search(ref, wordPattern) || regex_search(verse, wordPattern))
        {
            results.push_back(ref + " — " + verse);
        }
    }

    return results;
}
