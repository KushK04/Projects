/* Kush Khanna
 * CS106B: Programming Abstractions
 * This file accomplishes the tasks relating to search engines
 * in assignment two.
 */
#include <iostream>
#include <fstream>
#include "error.h"
#include "filelib.h"
#include "map.h"
#include "search.h"
#include "simpio.h"
#include "set.h"
#include <string>
#include "strlib.h"
#include "vector.h"
#include "testing/SimpleTest.h"
using namespace std;


/* This function takes a token and removes any punctionation surrounding
 * alphabetical characters, and returns that string in lowercase.
 * If there are no alphabetifcal characters, the function
 * returns a string.
 */
string cleanToken(string s)
{
    bool containsAlpha = false;
    for(int i = 0; i < s.length(); i++){
        if(ispunct(s[i])){
            s.erase(i,1);
        }
        if(isalpha(s[i])){
            containsAlpha = true;
            i=s.length();
        }
    }
    for(int i = s.length()-1; i >= 0; i--){
        if(ispunct(s[i])){
            s.erase(i,1);
        }
        if(isalpha(s[i])){
            i = 0;
        }
    }
    s = toLowerCase(s);
    s = trim(s);
    if(!containsAlpha){
        return "";
    }
    return s;
}

/* This function takes a string and retuns a set of strings
 * which contains each unique word in the string that is cleaned
 * by cleanToken.
 */
Set<string> gatherTokens(string text)
{
    Set<string> tokens;
    Vector<string> seperatedString = stringSplit(text," ");
    for(string s: seperatedString){
       string cleanedWord = cleanToken(s);
       if(cleanedWord.length() > 0){
           tokens.add(cleanedWord);
       }
    }
    return tokens;
}

/* This function takes a file and map and reads the file into
 * the map, mapping words to urls. The function returns the
 * amount of web pages in the index.
 */
int buildIndex(string dbfile, Map<string, Set<string>>& index)
{
    ifstream in;
    if (!openFile(in, dbfile)){
        error("Cannot open file named " + dbfile);
    }
    Vector<string> lines;
    readEntireFile(in, lines);
    int nPages = 0;
    for(int i = 0; i < lines.size(); i+=2){
        nPages += 1;
        Set<string> tokens = gatherTokens(lines[i+1]);
        for(string s: tokens){
            index[s].add(lines[i]);
        }
    }

    return nPages;
}

/* This function takes an index and a sentence and returns
 * all the matches of a given query.
 */

Set<string> findQueryMatches(Map<string, Set<string>>& index, string sentence)
{
    Set<string> result;
    Vector<string> seperatedString = stringSplit(sentence," ");
    for(string& s: seperatedString){
        if(s.find("+") != -1 && isalpha(s[s.find("+")+1])){
            result *= index[cleanToken(s)];
        } else if(s.find("-") != -1 && isalpha(s[s.find("-")+1])){
            result -= index[cleanToken(s)];
        } else {
            string cleanString = cleanToken(s);
            if(!(cleanString == "")){
            result += index[cleanToken(s)];
            }
        }
    }
    return result;
}

/* This function searches a file and asks for input for queries to be searched
 * in the file with rever se index.
 */
void searchEngine(string dbfile)
{
    ifstream in;
    cout << "Stand by while building index..." << endl;
    Map<string, Set<string>> index;
    int nPages = buildIndex(dbfile, index);
    cout << "Indexed " << nPages << " pages containing " << index.size() << " unique terms" << endl << endl;

    string querySentence = getLine("Enter query sentence (RETURN/ENTER to quit): ");
    while(trim(querySentence) != ""){
        Set<string> urls = findQueryMatches(index, querySentence);
        cout << "Found " << urls.size() << " matching pages" << endl;
        cout << urls << endl << endl;
        querySentence = getLine("Enter query sentence (RETURN/ENTER to quit): ");
    }
}

/* * * * * * Test Cases * * * * * */

PROVIDED_TEST("cleanToken on tokens with no punctuation") {
    EXPECT_EQUAL(cleanToken("hello"), "hello");
    EXPECT_EQUAL(cleanToken("WORLD"), "world");
}

PROVIDED_TEST("cleanToken on tokens with some punctuation at beginning and end") {
    EXPECT_EQUAL(cleanToken("/hello/"), "hello");
    EXPECT_EQUAL(cleanToken("~woRLD!"), "world");
}

PROVIDED_TEST("cleanToken on non-word tokens"){
    EXPECT_EQUAL(cleanToken("106"), "");
    EXPECT_EQUAL(cleanToken("~!106!!!"), "");
}

PROVIDED_TEST("gatherTokens from simple string") {
    Set<string> tokens = gatherTokens("to be or not to be");
    EXPECT_EQUAL(tokens.size(), 4);
}

PROVIDED_TEST("gatherTokens from seuss, 5 unique words, mixed case, punctuation") {
    Set<string> seuss = gatherTokens("One Fish Two Fish Red fish Blue fish!!! 123");
    EXPECT_EQUAL(seuss.size(), 5);
    EXPECT(seuss.contains("fish"));
    EXPECT(!seuss.contains("Fish"));
}

PROVIDED_TEST("buildIndex from tiny.txt, 4 pages, 20 unique tokens") {
    Map<string, Set<string>> index;
    int nPages = buildIndex("res/tiny.txt", index);
    EXPECT_EQUAL(nPages, 4);
    EXPECT_EQUAL(index.size(), 20);
    EXPECT(index.containsKey("fish"));
    EXPECT(!index.containsKey("@"));
}

PROVIDED_TEST("findQueryMatches from tiny.txt, single word query") {
    Map<string, Set<string>> index;
    buildIndex("res/tiny.txt", index);
    Set<string> matchesRed = findQueryMatches(index, "red");
    EXPECT_EQUAL(matchesRed.size(), 2);
    EXPECT(matchesRed.contains("www.dr.seuss.net"));
    Set<string> matchesHippo = findQueryMatches(index, "hippo");
    EXPECT(matchesHippo.isEmpty());
}

PROVIDED_TEST("findQueryMatches from tiny.txt, compound queries") {
    Map<string, Set<string>> index;
    buildIndex("res/tiny.txt", index);
    Set<string> matchesRedOrFish = findQueryMatches(index, "red fish");
    EXPECT_EQUAL(matchesRedOrFish.size(), 3);
    Set<string> matchesRedAndFish = findQueryMatches(index, "red +fish");
    EXPECT_EQUAL(matchesRedAndFish.size(), 1);
    Set<string> matchesRedWithoutFish = findQueryMatches(index, "red -fish");
    EXPECT_EQUAL(matchesRedWithoutFish.size(), 1);
}

STUDENT_TEST("cleanToken on tokens with punctuation within the word and around it") {
    EXPECT_EQUAL(cleanToken("@hel'lo@"), "hel'lo");
    EXPECT_EQUAL(cleanToken("!WO!R!LD!!!!"), "wo!r!ld");
}

STUDENT_TEST("cleanToken on tokens with only punctuation"){
    EXPECT_EQUAL(cleanToken("#@$$#@(&*^*!^"), "");
}

STUDENT_TEST("cleanToken on tokens with punctuation within the word but not around it") {
    EXPECT_EQUAL(cleanToken("hel'lo"), "hel'lo");
    EXPECT_EQUAL(cleanToken("WO!R!LD"), "wo!r!ld");
}

STUDENT_TEST("gatherTokens from string with no repeats") {
    Set<string> tokens = gatherTokens("how are you doing");
    EXPECT_EQUAL(tokens.size(), 4);
}

STUDENT_TEST("gatherTokens from black string") {
    Set<string> tokens = gatherTokens("");
    EXPECT_EQUAL(tokens.size(), 0);
}

STUDENT_TEST("gatherTokens from a really long string") {
    Set<string> tokens = gatherTokens("When I went to the beach, I noticed a really big dog. The dog barked at me, but she went away to the beach.");
    EXPECT_EQUAL(tokens.size(), 17);
}

STUDENT_TEST("buildIndex from tiny.txt, 4 pages, 20 unique tokens, testing other keys") {
    Map<string, Set<string>> index;
    int nPages = buildIndex("res/tiny.txt", index);
    EXPECT_EQUAL(nPages, 4);
    EXPECT_EQUAL(index.size(), 20);
    EXPECT(index.containsKey("orange"));
    EXPECT(index.containsKey("eat"));
}

STUDENT_TEST("buildIndex from tiny.txt, 4 pages, 20 unique tokens, testing more keys") {
    Map<string, Set<string>> index;
    int nPages = buildIndex("res/tiny.txt", index);
    EXPECT_EQUAL(nPages, 4);
    EXPECT(index.containsKey("one"));
    EXPECT(index.containsKey("yellow"));
    EXPECT(index.containsKey("not"));
    EXPECT(index.containsKey("i'm"));
}

STUDENT_TEST("findQueryMatches from tiny.txt, single word query with multiple and singular matches") {
    Map<string, Set<string>> index;
    buildIndex("res/tiny.txt", index);
    Set<string> matchesFish = findQueryMatches(index, "fish");
    EXPECT_EQUAL(matchesFish.size(), 2);
    EXPECT(matchesFish.contains("www.dr.seuss.net"));
    EXPECT(matchesFish.contains("www.shoppinglist.com"));
    Set<string> matchesEat = findQueryMatches(index, "eat");
    EXPECT_EQUAL(matchesEat.size(), 1);
    EXPECT(matchesEat.contains("www.bigbadwolf.com"));
}

STUDENT_TEST("findQueryMatches from website.txt, multiple word query") {
    Map<string, Set<string>> index;
    buildIndex("res/website.txt", index);
    Set<string> matchesExample = findQueryMatches(index, "suitable +kits");
    EXPECT(matchesExample.contains("http://cs106b.stanford.edu/qt/troubleshooting.html"));
}

STUDENT_TEST("findQueryMatches from website.txt, multiple word query 2") {
    Map<string, Set<string>> index;
    buildIndex("res/website.txt", index);
    Set<string> matchesExample = findQueryMatches(index, "linux windows -mac");
    EXPECT(matchesExample.contains("http://cs106b.stanford.edu/qt/install-cs106.html"));
}
