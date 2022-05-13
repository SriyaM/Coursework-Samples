/*
 * Author: Sriya Mantena
 * supplemented by CS106b starter code
 */

#include <iostream>
#include <fstream>
#include "error.h"
#include "filelib.h"
#include "map.h"
#include "search.h"
#include "set.h"
#include "strlib.h"
#include "vector.h"
#include "simpio.h"
#include "testing/SimpleTest.h"
using namespace std;


/* This function takes in a string of characters and iterates
* through each character from the front and back of the string.
* It removes punctuation at the beginning and end of the string,
* changes all the letters to lowercase, and returns the newly
* cleaned string.
*/
string cleanToken(string s)
{
    for (int i = 0; i < s.length(); i++){
        if (ispunct(s[i])) {
            s.erase(i,1);
            i--;
        }
        else {
            break;
        }
    }
    for (int i = s.length() - 1; i >= 0 && i < s.length(); i--){
        if (ispunct(s[i])) {
            s.erase(i,1);
        }
        else {
            break;
        }
    }
    for (int i = 0; i < s.length(); i++) {
        if (isalpha(s[i])) {
            string final = toLowerCase(s);
            return final;
        }
    }
    string empty = "";
    return empty;
}

/* This function takes in a string of text and splits
 * each word by seperating the string at spaces. It then
 * uses the cleanToken function to standardize each words
 * and if the word isnt empty, it is added to a set of strings.
 * This set of cleaned strings is returned by the function.
*/
Set<string> gatherTokens(string text)
{
    Set<string> tokens;
    Vector<string> splitString = stringSplit(text, " ");
    for (string token : splitString){
        token = cleanToken(token);
        if (token != ""){
            tokens.add(token);
        }
    }
    return tokens;
}

/* This function takes in a string, dbfile, representing the
 * path to the reference file with wesite information. It also
 * takes in a map of reverse indexes of type Map<string, Set<string>>.
 * The function opens the file, reads the lines into a vector of strings,
 * and iterates through the vector. It maps the corresponding website
 * to each word in gatherTokens. The function then returns this
 * populated map of inverted indexes.
*/
int buildIndex(string dbfile, Map<string, Set<string>>& index)
{
    ifstream in;

    if (!openFile(in, dbfile))
        error("Cannot open file named " + dbfile);

    Vector<string> lines;
    readEntireFile(in, lines);

    for (int i = 0; i < lines.size(); i+=2){
        Set<string> tokens = gatherTokens(lines[i+1]);
        for (string token: tokens) {
            index[token].add(lines[i]);
        }
    }
    return (lines.size()/2);
}

/* This function takes in a map, the inverted index, and
 * a string, query, which is what is searches by the user.
 * The query is split by word, cleaned, and the wesited with
 * a matching word is determined from the inverted index. Then,
 * the first character of the uncleaned word is checked.
 * If there is a + or - it finds the difference or intersection of
 * the correspoding website with the previous word. If there is no
 * + or - at the beginning of the string it simply combines the
 * websites with the current and previous word in the search. The
 * function iterates through all the words in this manner and returns a
 * set of strings which are the websites that match the search parameters.
 *
*/
Set<string> findQueryMatches(Map<string, Set<string>>& index, string query)
{
    Set<string> curr;
    Set<string> prev;
    Vector<string> splitQuery = stringSplit(query, " ");

    for (string word : splitQuery){
        string cleanWord = cleanToken(word);
        curr = index.get(cleanWord);
        if (word.substr(0,1) == "+") {
            curr = curr.intersect(prev);
        }
        else if (word.substr(0,1) == "-") {
            curr = prev.difference(curr);
        }
        else {
            curr = curr.unionWith(prev);
        }
        prev = curr;
    }
    return curr;
}

/* This function takes in a string, the file path to
 * the reference file. It builds the inverted index based on the file
 * and prompts the user to enter a search. The findQueryMatches function
 * is used to find the corresponding websites which are printed along with
 * the number of wesites found. The user is promted to enter a search query
 * until they press enter. The function does not return anything.
*/
void searchEngine(string dbfile)
{
    Map<string, Set<string>> index;
    int nPages = buildIndex(dbfile, index);
    cout << nPages << endl;
    while (true) {
        string query = getLine("Enter query sentence (RETURN/ENTER to quit):");
        if (query == ""){
            cout << "all done" << endl;
            break;
        }
        Set<string> websites = findQueryMatches(index, query);
        cout << "Found " << websites.size() << " Matching Pages" << endl;
        cout << websites << endl;
    }

}

/* * * * * * Test Cases * * * * * */

PROVIDED_TEST("cleanToken on strings with no punctuation at beginning or end") {
    EXPECT_EQUAL(cleanToken("hello"), "hello");
    EXPECT_EQUAL(cleanToken("WORLD"), "world");
    EXPECT_EQUAL(cleanToken("CS*106B"), "cs*106b");
}

PROVIDED_TEST("cleanToken on strings with some punctuation at beginning and end") {
    EXPECT_EQUAL(cleanToken("/hello/"), "hello");
    EXPECT_EQUAL(cleanToken("~woRLD!"), "world");
}

PROVIDED_TEST("cleanToken on non-word strings (no letters)"){
    EXPECT_EQUAL(cleanToken("106"), "");
    EXPECT_EQUAL(cleanToken("~!106!!!"), "");
}

PROVIDED_TEST("gatherTokens from simple string") {
    Set<string> expected = {"go", "gophers"};
    EXPECT_EQUAL(gatherTokens("go go go gophers"), expected);
}

PROVIDED_TEST("gatherTokens correctly cleans tokens") {
    Set<string> expected = {"i", "love", "cs*106b"};
    EXPECT_EQUAL(gatherTokens("I _love_ CS*106B!"), expected);
}

PROVIDED_TEST("gatherTokens from seuss, 5 unique words, mixed case, punctuation") {
    Set<string> tokens = gatherTokens("One Fish Two Fish *Red* fish Blue fish ** 10 RED Fish?");
    EXPECT_EQUAL(tokens.size(), 5);
    EXPECT(tokens.contains("fish"));
    EXPECT(!tokens.contains("Fish"));
}

PROVIDED_TEST("buildIndex from tiny.txt, 4 pages, 11 unique tokens") {
    Map<string, Set<string>> index;
    int nPages = buildIndex("res/tiny.txt", index);
    EXPECT_EQUAL(nPages, 4);
    EXPECT_EQUAL(index.size(), 11);
    EXPECT(index.containsKey("fish"));
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
    EXPECT_EQUAL(matchesRedOrFish.size(), 4);
    Set<string> matchesRedAndFish = findQueryMatches(index, "red +fish");
    EXPECT_EQUAL(matchesRedAndFish.size(), 1);
    Set<string> matchesRedWithoutFish = findQueryMatches(index, "red -fish");
    EXPECT_EQUAL(matchesRedWithoutFish.size(), 1);
}


// STUDENT TEST CASES

STUDENT_TEST("cleanToken with upper case letters and lot's of punctuation)"){
    EXPECT_EQUAL(cleanToken("''SrIYA's cOOl?!"), "sriya's cool");
    EXPECT_EQUAL(cleanToken("LaVendeR!?"), "lavender");
}

STUDENT_TEST("cleanToken with one letter)"){
    EXPECT_EQUAL(cleanToken("!!.!S,!.!"), "s");
    EXPECT_EQUAL(cleanToken("(),()4t"), "4t");
}

STUDENT_TEST("cleanToken with no punctuation)"){
    EXPECT_EQUAL(cleanToken("whats up"), "whats up");
    EXPECT_EQUAL(cleanToken("66s88"), "66s88");
}

STUDENT_TEST("gatherTokens from strings with a lot of punctuation") {
    Set<string> expected = {"i'm", "wearing","2.shoes"};
    EXPECT_EQUAL(gatherTokens("!!I'm wearing// .2.shoes"), expected);
    Set<string> expected2 = {"trees", "are","gre!en"};
    EXPECT_EQUAL(gatherTokens("'Trees!! are. ??Gre!en!!"), expected2);
}

STUDENT_TEST("gatherTokens from strings with repeating words") {
    Set<string> expected = {"i", "love","lo've", "sleeping"};
    EXPECT_EQUAL(gatherTokens("I love .. love' lo've sleeping//"), expected);
}

STUDENT_TEST("gatherTokens from strings with letterless groups of characters") {
    Set<string> expected = {"there", "were","swans"};
    EXPECT_EQUAL(gatherTokens("There //were .... 3 Swans()"), expected);
}

STUDENT_TEST("buildIndex from website.txt, 32 pages, 3875 unique tokens") {
    Map<string, Set<string>> index;
    int nPages = buildIndex("res/website.txt", index);
    EXPECT_EQUAL(nPages, 32);
    EXPECT_EQUAL(index.size(), 3875);
    EXPECT(index.containsKey("motivation"));
}

STUDENT_TEST("buildIndex time tests") {
    Map<string, Set<string>> index;
    TIME_OPERATION("4 web pages", buildIndex("res/tiny.txt", index));
    TIME_OPERATION("32 web pages", buildIndex("res/website.txt", index));
}

STUDENT_TEST("findQueryMatches from tiny.txt, several operations, captial letters and characters") {
    Map<string, Set<string>> index;
    buildIndex("res/tiny.txt", index);
    Set<string> fishBlueRed = findQueryMatches(index, "fish -blue! -red/");
    EXPECT_EQUAL(fishBlueRed.size(), 2);
    Set<string> redBlueFish = findQueryMatches(index, "!red +/blue -!fish");
    EXPECT_EQUAL(redBlueFish.size(), 1);
    Set<string> milkFishBread = findQueryMatches(index, ",milk -fish, -Bread!");
    EXPECT_EQUAL(milkFishBread.size(), 0);
    Set<string> fishBreadMore = findQueryMatches(index, "fish, -bread>> -5lb_m&ms!! -red");
    EXPECT_EQUAL(fishBreadMore.size(), 1);
    Set<string> excludingWords = findQueryMatches(index, "fish, -bhbii>> -hello!! -war -!candy");
    EXPECT_EQUAL(excludingWords.size(), 3);
}
